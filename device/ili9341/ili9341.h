/**
 * @file    ili9341.h
 * @brief   ILI9341 TFT LCD controller driver (SPI 16-bit RGB565)
 * @author  Smart Room Control Team
 * @note    Hardware pin mapping (configured via STM32CubeMX):
 *          - CS:   PA4  (LCD_CS_GPIO_Port, LCD_CS_Pin)
 *          - DC:   PA3  (LCD_DC_GPIO_Port, LCD_DC_Pin)
 *          - RST:  PA2  (LCD_RST_GPIO_Port, LCD_RST_Pin)
 *          - BL:   PA1  (LCD_BL_GPIO_Port, LCD_BL_Pin)
 *          - SPI:  SPI1 (PA5 SCK, PA6 MISO, PA7 MOSI @ 18 Mbit/s)
 */

#ifndef ILI9341_H
#define ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* Display dimensions (Portrait mode) */
#define ILI9341_WIDTH           240
#define ILI9341_HEIGHT          320

/* 16-bit RGB565 color definitions */
#define ILI9341_BLACK           0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY            0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN       0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN        0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON          0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE          0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE           0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY       0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY        0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE            0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN           0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN            0x07FF      /*   0, 255, 255 */
#define ILI9341_RED             0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA         0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW          0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE           0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE          0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK            0xFC18      /* 255, 130, 198 */

/* Core Driver API */

/**
 * @brief  Initializes ILI9341 controller, resets hardware, and configures registers.
 */
void ili9341_init(void);

/**
 * @brief  Transmits a single command byte over SPI (DC driven LOW).
 * @param  cmd: Command opcode.
 */
void ili9341_write_command(uint8_t cmd);

/**
 * @brief  Transmits data payload over SPI (DC driven HIGH).
 * @param  p_data: Pointer to data buffer.
 * @param  size: Number of bytes to transfer.
 */
void ili9341_write_data(uint8_t *p_data, uint16_t size);

/**
 * @brief  Draws a single pixel at specified coordinate.
 * @param  x: X coordinate (0 to ILI9341_WIDTH - 1).
 * @param  y: Y coordinate (0 to ILI9341_HEIGHT - 1).
 * @param  color: 16-bit RGB565 color.
 */
void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief  Fills a rectangular region with specified color.
 * @param  x: Origin X coordinate.
 * @param  y: Origin Y coordinate.
 * @param  width: Rectangle width in pixels.
 * @param  height: Rectangle height in pixels.
 * @param  color: 16-bit RGB565 color.
 */
void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

/**
 * @brief  Clears entire screen with specified color.
 * @param  color: 16-bit RGB565 color.
 */
void ili9341_fill_screen(uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_H */
