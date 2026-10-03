/**
 * @file    xpt2046.c
 * @brief   XPT2046 4-wire resistive touch screen controller driver implementation
 * @author  Coder 1 - HMI / User Interface Team
 */

#include "xpt2046.h"

extern SPI_HandleTypeDef hspi2;

/* Number of samples for median filtering */
#define XPT2046_SAMPLE_COUNT        7

/* Orientation configuration for 240x320 portrait mode */
#define XPT2046_SWAP_XY             0   /**< 0: Direct axis mapping (X=short edge 240, Y=long edge 320) */
#define XPT2046_INVERT_X            1   /**< 1: Invert X coordinate direction */
#define XPT2046_INVERT_Y            1   /**< 1: Invert Y coordinate direction */

/* Private Helpers */
static inline void xpt2046_select(void)
{
    HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
}

static inline void xpt2046_unselect(void)
{
    HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
}

static uint16_t xpt2046_read_adc_channel(uint8_t cmd)
{
    uint8_t tx_buf[3] = { cmd, 0x00, 0x00 };
    uint8_t rx_buf[3] = { 0x00, 0x00, 0x00 };

    xpt2046_select();
    HAL_SPI_TransmitReceive(&hspi2, tx_buf, rx_buf, 3, 50);
    xpt2046_unselect();

    /* 12-bit ADC data extraction (MSB first, bits [11:0]) */
    uint16_t raw_val = ((uint16_t)rx_buf[1] << 8) | rx_buf[2];
    raw_val = (raw_val >> 3) & 0x0FFF;

    return raw_val;
}

static void sort_array(uint16_t *arr, uint8_t n)
{
    for (uint8_t i = 0; i < n - 1; i++)
    {
        for (uint8_t j = i + 1; j < n; j++)
        {
            if (arr[i] > arr[j])
            {
                uint16_t tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
}

/* Public Functions */

void xpt2046_init(void)
{
    /* Ensure CS is de-asserted (Active-Low) */
    xpt2046_unselect();
}

bool xpt2046_is_touched(void)
{
    /* PENIRQ pin PB11 goes LOW when screen surface is pressed */
    return (HAL_GPIO_ReadPin(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin) == GPIO_PIN_RESET);
}

bool xpt2046_read_raw(uint16_t *p_raw_x, uint16_t *p_raw_y)
{
    /* 1. Fast hardware check: PENIRQ must be active (LOW) */
    if (!xpt2046_is_touched())
    {
        return false;
    }

    /* 2. Dummy read to discard stale charge and settle S/H capacitance */
    xpt2046_read_adc_channel(XPT2046_CMD_READ_X);
    xpt2046_read_adc_channel(XPT2046_CMD_READ_Y);

    uint16_t samples_x[XPT2046_SAMPLE_COUNT];
    uint16_t samples_y[XPT2046_SAMPLE_COUNT];

    /* 3. Sample coordinate readings without aborting on temporary IRQ glitches */
    for (uint8_t i = 0; i < XPT2046_SAMPLE_COUNT; i++)
    {
        samples_x[i] = xpt2046_read_adc_channel(XPT2046_CMD_READ_X);
        samples_y[i] = xpt2046_read_adc_channel(XPT2046_CMD_READ_Y);
    }

    /* 4. Median filter: sort arrays and pick center value to reject transient spikes */
    sort_array(samples_x, XPT2046_SAMPLE_COUNT);
    sort_array(samples_y, XPT2046_SAMPLE_COUNT);

    uint16_t med_x = samples_x[XPT2046_SAMPLE_COUNT / 2];
    uint16_t med_y = samples_y[XPT2046_SAMPLE_COUNT / 2];

    /* 5. Expanded sanity bounds check (accepts touches close to edge frames) */
    if (med_x < 50 || med_x > 4050 || med_y < 50 || med_y > 4050)
    {
        return false;
    }

    if (p_raw_x != NULL)
    {
        *p_raw_x = med_x;
    }
    if (p_raw_y != NULL)
    {
        *p_raw_y = med_y;
    }

    return true;
}

bool xpt2046_get_xy(uint16_t *p_x, uint16_t *p_y)
{
    return xpt2046_get_xy_and_raw(p_x, p_y, NULL, NULL);
}

bool xpt2046_get_xy_and_raw(uint16_t *p_x, uint16_t *p_y, uint16_t *p_raw_x, uint16_t *p_raw_y)
{
    uint16_t raw_x = 0;
    uint16_t raw_y = 0;

    if (!xpt2046_read_raw(&raw_x, &raw_y))
    {
        return false;
    }

    if (p_raw_x != NULL) *p_raw_x = raw_x;
    if (p_raw_y != NULL) *p_raw_y = raw_y;

    uint16_t in_x = raw_x;
    uint16_t in_y = raw_y;

#if (XPT2046_SWAP_XY == 1)
    in_x = raw_y;
    in_y = raw_x;
#endif

    /* Linear interpolation into pixel space */
    int32_t pixel_x = (int32_t)(in_x - XPT2046_CAL_X_MIN) * LCD_PIXEL_WIDTH / (XPT2046_CAL_X_MAX - XPT2046_CAL_X_MIN);
    int32_t pixel_y = (int32_t)(in_y - XPT2046_CAL_Y_MIN) * LCD_PIXEL_HEIGHT / (XPT2046_CAL_Y_MAX - XPT2046_CAL_Y_MIN);

    /* Clamp to LCD boundary */
    if (pixel_x < 0) pixel_x = 0;
    if (pixel_x >= LCD_PIXEL_WIDTH) pixel_x = LCD_PIXEL_WIDTH - 1;

    if (pixel_y < 0) pixel_y = 0;
    if (pixel_y >= LCD_PIXEL_HEIGHT) pixel_y = LCD_PIXEL_HEIGHT - 1;

#if (XPT2046_INVERT_X == 1)
    pixel_x = (LCD_PIXEL_WIDTH - 1) - pixel_x;
#endif

#if (XPT2046_INVERT_Y == 1)
    pixel_y = (LCD_PIXEL_HEIGHT - 1) - pixel_y;
#endif

    if (p_x != NULL)
    {
        *p_x = (uint16_t)pixel_x;
    }
    if (p_y != NULL)
    {
        *p_y = (uint16_t)pixel_y;
    }

    return true;
}

bool xpt2046_scan(xpt2046_touch_t *p_touch)
{
    if (p_touch == NULL)
    {
        return false;
    }

    p_touch->is_pressed = xpt2046_is_touched();

    if (!p_touch->is_pressed)
    {
        return false;
    }

    return xpt2046_get_xy(&p_touch->x, &p_touch->y);
}
