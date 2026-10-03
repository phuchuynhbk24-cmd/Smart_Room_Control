/**
 * @file    xpt2046.h
 * @brief   XPT2046 4-wire resistive touch controller driver (SPI2)
 * @author  Coder 1 - HMI / User Interface Team
 * @note    Hardware pin mapping (configured via STM32CubeMX):
 *          - CS:   PB12 (TOUCH_CS_GPIO_Port, TOUCH_CS_Pin)
 *          - IRQ:  PB11 (TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin)
 *          - SPI:  SPI2 (PB13 SCK, PB14 MISO, PB15 MOSI @ 2.25 Mbit/s)
 */

#ifndef XPT2046_H
#define XPT2046_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* XPT2046 Command Bytes (12-bit conversion, Differential mode, Power-Down between reads) */
#define XPT2046_CMD_READ_X          0xD0    /**< 12-bit differential ADC conversion for X coordinate */
#define XPT2046_CMD_READ_Y          0x90    /**< 12-bit differential ADC conversion for Y coordinate */
#define XPT2046_CMD_READ_Z1         0xB0    /**< Touch pressure Z1 */
#define XPT2046_CMD_READ_Z2         0xC0    /**< Touch pressure Z2 */

/* Screen physical resolution reference (ILI9341 portrait) */
#define LCD_PIXEL_WIDTH             240
#define LCD_PIXEL_HEIGHT            320

/* Default calibration values (ADC range: 200..3850) */
#define XPT2046_CAL_X_MIN           300
#define XPT2046_CAL_X_MAX           3800
#define XPT2046_CAL_Y_MIN           300
#define XPT2046_CAL_Y_MAX           3800

/**
 * @brief Touch event data structure
 */
typedef struct {
    uint16_t x;             /**< Calibrated screen X pixel (0 to LCD_PIXEL_WIDTH - 1) */
    uint16_t y;             /**< Calibrated screen Y pixel (0 to LCD_PIXEL_HEIGHT - 1) */
    uint16_t raw_x;         /**< Raw 12-bit ADC X value (0 to 4095) */
    uint16_t raw_y;         /**< Raw 12-bit ADC Y value (0 to 4095) */
    bool     is_pressed;    /**< True when touch surface is actively pressed */
} xpt2046_touch_t;

/* Public API */

/**
 * @brief  Initializes XPT2046 hardware interface (CS driven HIGH).
 */
void xpt2046_init(void);

/**
 * @brief  Checks if the touch screen is currently pressed via hardware IRQ pin.
 * @retval true if touched (PENIRQ is LOW), false otherwise.
 */
bool xpt2046_is_touched(void);

/**
 * @brief  Acquires raw 12-bit ADC coordinates with median filter over multiple samples.
 * @param  p_raw_x: Pointer to store filtered raw X.
 * @param  p_raw_y: Pointer to store filtered raw Y.
 * @retval true if valid touch data was obtained, false if noise or touch released.
 */
bool xpt2046_read_raw(uint16_t *p_raw_x, uint16_t *p_raw_y);

/**
 * @brief  Reads touch coordinates and maps them to LCD pixel space (240x320).
 * @param  p_x: Pointer to store mapped X pixel (0 to 239).
 * @param  p_y: Pointer to store mapped Y pixel (0 to 319).
 * @retval true if valid touch point detected, false otherwise.
 */
bool xpt2046_get_xy(uint16_t *p_x, uint16_t *p_y);

/**
 * @brief  Performs full touch scan and updates state structure.
 * @param  p_touch: Pointer to touch state structure.
 * @retval true if screen is pressed with valid coordinates.
 */
bool xpt2046_scan(xpt2046_touch_t *p_touch);

#ifdef __cplusplus
}
#endif

#endif /* XPT2046_H */
