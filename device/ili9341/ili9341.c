/**
 * @file    ili9341.c
 * @brief   ILI9341 TFT LCD controller driver implementation
 * @author  Smart Room Control Team
 */

#include "ili9341.h"

extern SPI_HandleTypeDef hspi1;

/* Burst buffer chunk size (256 pixels = 512 bytes) */
#define BURST_PIXEL_CHUNK   256
static uint8_t s_burst_buffer[BURST_PIXEL_CHUNK * 2];

/* ILI9341 Command Definitions */
#define ILI9341_CMD_SWRESET     0x01
#define ILI9341_CMD_SLPOUT      0x11
#define ILI9341_CMD_GAMMASET    0x26
#define ILI9341_CMD_DISPOFF     0x28
#define ILI9341_CMD_DISPON      0x29
#define ILI9341_CMD_CASET       0x2A
#define ILI9341_CMD_PASET       0x2B
#define ILI9341_CMD_RAMWR       0x2C
#define ILI9341_CMD_MADCTL      0x36
#define ILI9341_CMD_PIXFMT      0x3A
#define ILI9341_CMD_FRMCTR1     0xB1
#define ILI9341_CMD_DFUNCTR     0xB6
#define ILI9341_CMD_PWCTR1      0xC0
#define ILI9341_CMD_PWCTR2      0xC1
#define ILI9341_CMD_VMCTR1      0xC5
#define ILI9341_CMD_VMCTR2      0xC7
#define ILI9341_CMD_PWCTRA      0xCB
#define ILI9341_CMD_PWCTRB      0xCF
#define ILI9341_CMD_PGAMMA      0xE0
#define ILI9341_CMD_NGAMMA      0xE1
#define ILI9341_CMD_DTCA        0xE8
#define ILI9341_CMD_DTCB        0xEA
#define ILI9341_CMD_POSC        0xED
#define ILI9341_CMD_ENABLE_3G   0xF2
#define ILI9341_CMD_PUMPRATIO   0xF7

/* MADCTL bit definitions */
#define ILI9341_MADCTL_MY       0x80    /* Row address order (bottom to top) */
#define ILI9341_MADCTL_MX       0x40    /* Column address order (right to left) */
#define ILI9341_MADCTL_MV       0x20    /* Row/Column exchange */
#define ILI9341_MADCTL_ML       0x10    /* Vertical refresh order */
#define ILI9341_MADCTL_BGR      0x08    /* Color filter panel order (BGR) */
#define ILI9341_MADCTL_MH       0x04    /* Horizontal refresh order */

/* Low-level GPIO helpers */
static inline void ili9341_select(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}

static inline void ili9341_unselect(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static inline void ili9341_dc_command(void)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
}

static inline void ili9341_dc_data(void)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Performs a hardware reset pulse on LCD_RST line.
 */
static void ili9341_hardware_reset(void)
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
}

/**
 * @brief  Defines active drawing window (CASET/PASET) and issues RAMWR command.
 */
void ili9341_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t cmd_caset = ILI9341_CMD_CASET;
    uint8_t data_col[4] = { (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF), (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF) };
    uint8_t cmd_paset = ILI9341_CMD_PASET;
    uint8_t data_row[4] = { (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF), (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF) };
    uint8_t cmd_ramwr = ILI9341_CMD_RAMWR;

    ili9341_select();

    /* Column address set (CASET) */
    ili9341_dc_command();
    HAL_SPI_Transmit(&hspi1, &cmd_caset, 1, HAL_MAX_DELAY);
    ili9341_dc_data();
    HAL_SPI_Transmit(&hspi1, data_col, 4, HAL_MAX_DELAY);

    /* Page address set (PASET) */
    ili9341_dc_command();
    HAL_SPI_Transmit(&hspi1, &cmd_paset, 1, HAL_MAX_DELAY);
    ili9341_dc_data();
    HAL_SPI_Transmit(&hspi1, data_row, 4, HAL_MAX_DELAY);

    /* Memory write (RAMWR) */
    ili9341_dc_command();
    HAL_SPI_Transmit(&hspi1, &cmd_ramwr, 1, HAL_MAX_DELAY);

    ili9341_unselect();
}

void ili9341_write_command(uint8_t cmd)
{
    ili9341_dc_command();
    ili9341_select();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    ili9341_unselect();
}

void ili9341_write_data(uint8_t *p_data, uint16_t size)
{
    if (p_data == NULL || size == 0)
    {
        return;
    }

    ili9341_dc_data();
    ili9341_select();
    HAL_SPI_Transmit(&hspi1, p_data, size, HAL_MAX_DELAY);
    ili9341_unselect();
}

void ili9341_init(void)
{
#ifdef LCD_BL_Pin
    /* Enable display backlight */
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
#endif

    /* Ensure CS is de-asserted */
    ili9341_unselect();

    /* Hardware reset sequence */
    ili9341_hardware_reset();

    /* Software reset: wait 150ms for internal supply stabilization */
    ili9341_write_command(ILI9341_CMD_SWRESET);
    HAL_Delay(150);

    /* Power Control A */
    uint8_t pwctra[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
    ili9341_write_command(ILI9341_CMD_PWCTRA);
    ili9341_write_data(pwctra, sizeof(pwctra));

    /* Power Control B */
    uint8_t pwctrb[] = { 0x00, 0xC1, 0x30 };
    ili9341_write_command(ILI9341_CMD_PWCTRB);
    ili9341_write_data(pwctrb, sizeof(pwctrb));

    /* Driver Timing Control A */
    uint8_t dtca[] = { 0x85, 0x00, 0x78 };
    ili9341_write_command(ILI9341_CMD_DTCA);
    ili9341_write_data(dtca, sizeof(dtca));

    /* Driver Timing Control B */
    uint8_t dtcb[] = { 0x00, 0x00 };
    ili9341_write_command(ILI9341_CMD_DTCB);
    ili9341_write_data(dtcb, sizeof(dtcb));

    /* Power on Sequence Control */
    uint8_t posc[] = { 0x64, 0x03, 0x12, 0x81 };
    ili9341_write_command(ILI9341_CMD_POSC);
    ili9341_write_data(posc, sizeof(posc));

    /* Pump Ratio Control */
    uint8_t pumpratio[] = { 0x20 };
    ili9341_write_command(ILI9341_CMD_PUMPRATIO);
    ili9341_write_data(pumpratio, sizeof(pumpratio));

    /* Power Control 1 */
    uint8_t pwctr1[] = { 0x23 };
    ili9341_write_command(ILI9341_CMD_PWCTR1);
    ili9341_write_data(pwctr1, sizeof(pwctr1));

    /* Power Control 2 */
    uint8_t pwctr2[] = { 0x10 };
    ili9341_write_command(ILI9341_CMD_PWCTR2);
    ili9341_write_data(pwctr2, sizeof(pwctr2));

    /* VCOM Control 1 */
    uint8_t vmctr1[] = { 0x3E, 0x28 };
    ili9341_write_command(ILI9341_CMD_VMCTR1);
    ili9341_write_data(vmctr1, sizeof(vmctr1));

    /* VCOM Control 2 */
    uint8_t vmctr2[] = { 0x86 };
    ili9341_write_command(ILI9341_CMD_VMCTR2);
    ili9341_write_data(vmctr2, sizeof(vmctr2));

    /* Pixel format: 16-bit / pixel (RGB565) */
    uint8_t pixfmt[] = { 0x55 };
    ili9341_write_command(ILI9341_CMD_PIXFMT);
    ili9341_write_data(pixfmt, sizeof(pixfmt));

    /* Frame rate control: 79 Hz */
    uint8_t frmctr1[] = { 0x00, 0x18 };
    ili9341_write_command(ILI9341_CMD_FRMCTR1);
    ili9341_write_data(frmctr1, sizeof(frmctr1));

    /* Display Function Control */
    uint8_t dfunctr[] = { 0x08, 0x82, 0x27 };
    ili9341_write_command(ILI9341_CMD_DFUNCTR);
    ili9341_write_data(dfunctr, sizeof(dfunctr));

    /* Enable 3G */
    uint8_t enable3g[] = { 0x00 };
    ili9341_write_command(ILI9341_CMD_ENABLE_3G);
    ili9341_write_data(enable3g, sizeof(enable3g));

    /* Gamma curve set */
    uint8_t gammaset[] = { 0x01 };
    ili9341_write_command(ILI9341_CMD_GAMMASET);
    ili9341_write_data(gammaset, sizeof(gammaset));

    /* Positive Gamma correction */
    uint8_t pgamma[] = {
        0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
        0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00
    };
    ili9341_write_command(ILI9341_CMD_PGAMMA);
    ili9341_write_data(pgamma, sizeof(pgamma));

    /* Negative Gamma correction */
    uint8_t ngamma[] = {
        0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
        0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F
    };
    ili9341_write_command(ILI9341_CMD_NGAMMA);
    ili9341_write_data(ngamma, sizeof(ngamma));

    /* Memory Access Control: Portrait mode with BGR color filter panel */
    uint8_t madctl[] = { ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR };
    ili9341_write_command(ILI9341_CMD_MADCTL);
    ili9341_write_data(madctl, sizeof(madctl));

    /* Exit sleep mode: minimum 120ms delay before issuing display ON */
    ili9341_write_command(ILI9341_CMD_SLPOUT);
    HAL_Delay(120);

    /* Turn display on */
    ili9341_write_command(ILI9341_CMD_DISPON);
    HAL_Delay(20);

    /* Initial screen clear */
    ili9341_fill_screen(ILI9341_BLACK);
}

void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    /* Boundary check */
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT)
    {
        return;
    }

    ili9341_set_address_window(x, y, x, y);

    uint8_t pixel_data[2];
    pixel_data[0] = (uint8_t)(color >> 8);
    pixel_data[1] = (uint8_t)(color & 0xFF);

    ili9341_write_data(pixel_data, 2);
}

void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    /* Boundary check and clipping */
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT)
    {
        return;
    }
    if ((x + width) > ILI9341_WIDTH)
    {
        width = ILI9341_WIDTH - x;
    }
    if ((y + height) > ILI9341_HEIGHT)
    {
        height = ILI9341_HEIGHT - y;
    }
    if (width == 0 || height == 0)
    {
        return;
    }

    /* Configure target drawing rectangle */
    ili9341_set_address_window(x, y, x + width - 1, y + height - 1);

    /* Pre-fill static burst buffer with color payload (Big-Endian) */
    uint8_t color_high = (uint8_t)(color >> 8);
    uint8_t color_low  = (uint8_t)(color & 0xFF);

    for (uint16_t i = 0; i < BURST_PIXEL_CHUNK; i++)
    {
        s_burst_buffer[2 * i]     = color_high;
        s_burst_buffer[2 * i + 1] = color_low;
    }

    /* Stream pixel chunks over SPI bus */
    uint32_t total_pixels = (uint32_t)width * height;

    ili9341_dc_data();
    ili9341_select();

    while (total_pixels > 0)
    {
        uint16_t current_chunk = (total_pixels > BURST_PIXEL_CHUNK) ? BURST_PIXEL_CHUNK : (uint16_t)total_pixels;
        HAL_SPI_Transmit(&hspi1, s_burst_buffer, current_chunk * 2, HAL_MAX_DELAY);
        total_pixels -= current_chunk;
    }

    ili9341_unselect();
}

void ili9341_fill_screen(uint16_t color)
{
    ili9341_fill_rect(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
}

void ili9341_draw_fast_h_line(uint16_t x, uint16_t y, uint16_t width, uint16_t color)
{
    ili9341_fill_rect(x, y, width, 1, color);
}

void ili9341_draw_fast_v_line(uint16_t x, uint16_t y, uint16_t height, uint16_t color)
{
    ili9341_fill_rect(x, y, 1, height, color);
}

void ili9341_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if (width == 0 || height == 0)
    {
        return;
    }
    ili9341_draw_fast_h_line(x, y, width, color);
    ili9341_draw_fast_h_line(x, y + height - 1, width, color);
    ili9341_draw_fast_v_line(x, y, height, color);
    ili9341_draw_fast_v_line(x + width - 1, y, height, color);
}

void ili9341_draw_buffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p_bytes, uint16_t byte_count)
{
    if (p_bytes == NULL || byte_count == 0 || width == 0 || height == 0)
    {
        return;
    }
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT)
    {
        return;
    }

    ili9341_set_address_window(x, y, x + width - 1, y + height - 1);

    ili9341_dc_data();
    ili9341_select();
    HAL_SPI_Transmit(&hspi1, (uint8_t *)p_bytes, byte_count, HAL_MAX_DELAY);
    ili9341_unselect();
}

void ili9341_invert_colors(bool invert)
{
    ili9341_write_command(invert ? 0x21 : 0x20);
}
