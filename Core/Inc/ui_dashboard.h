/**
 * @file    ui_dashboard.h
 * @brief   Smart Room Control HMI Dashboard definitions and function prototypes
 * @author  Coder 1 - HMI / User Interface Team
 * @note    Designed for 2.8" ILI9341 TFT Display (240x320 portrait)
 */

#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* UI Theme 16-bit RGB565 Color Palette */
#define UI_COLOR_BG          0x0821  /**< Deep dark background */
#define UI_COLOR_CARD_BG     0x18C3  /**< Charcoal container background */
#define UI_COLOR_CARD_BD     0x39E7  /**< Subtle card border */
#define UI_COLOR_TEXT_MAIN   0xFFFF  /**< Primary text (White) */
#define UI_COLOR_TEXT_MUTED  0x9CD3  /**< Secondary text (Slate / Light Grey) */
#define UI_COLOR_TEMP        0xFD20  /**< Temperature accent (Warm Orange) */
#define UI_COLOR_HUMI        0x07FF  /**< Humidity accent (Vibrant Cyan) */
#define UI_COLOR_LIGHT       0xFFE0  /**< Ambient light accent (Yellow) */
#define UI_COLOR_ON          0x07E0  /**< Relay Active / ON (Vivid Green) */
#define UI_COLOR_OFF         0x4208  /**< Relay Inactive / OFF (Dark Slate) */
#define UI_COLOR_ALERT       0xF800  /**< Motion detected badge (Alert Red) */
#define UI_COLOR_IDLE        0x10C2  /**< Motion clear badge (Calm Teal) */

/**
 * @brief Dashboard sensor & actuator telemetry data structure
 */
typedef struct {
    float    temp;           /**< Room temperature in degrees Celsius (e.g. 28.5) */
    float    humi;           /**< Relative humidity in percent (e.g. 65.0) */
    uint8_t  light_percent;  /**< Ambient light intensity (0 - 100%) */
    uint8_t  pir_motion;     /**< PIR sensor motion status (1: Detected, 0: Clear) */
    uint8_t  relay_fan;      /**< Fan relay state (1: ON, 0: OFF) */
    uint8_t  relay_light1;   /**< Light 1 relay state (1: ON, 0: OFF) */
    uint8_t  relay_light2;   /**< Light 2 relay state (1: ON, 0: OFF) */
    uint8_t  relay_dehum;    /**< Dehumidifier relay state (1: ON, 0: OFF) */
    uint8_t  auto_mode;      /**< Operation mode (1: AUTO, 0: MANUAL) */
} UI_Dashboard_Data_t;

/* Public UI API Prototypes */

/**
 * @brief  Draws a single 5x7 ASCII character with scaling and background overwrite.
 * @param  x: Top-left X coordinate.
 * @param  y: Top-left Y coordinate.
 * @param  c: ASCII character (32 to 127).
 * @param  color: Foreground 16-bit RGB565 color.
 * @param  bg: Background 16-bit RGB565 color.
 * @param  size: Multiplier scale (1 = 6x8 px, 2 = 12x16 px).
 */
void UI_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);

/**
 * @brief  Draws a null-terminated string.
 * @param  x: Origin X coordinate.
 * @param  y: Origin Y coordinate.
 * @param  str: String pointer.
 * @param  color: Foreground color.
 * @param  bg: Background color.
 * @param  size: Scale factor.
 */
void UI_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size);

/**
 * @brief  Draws a card container with border outline.
 */
void UI_DrawCard(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t border_color, uint16_t bg_color);

/**
 * @brief  Draws a horizontal progress bar for percentage representation.
 */
void UI_DrawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t fg_color, uint16_t bg_color);

/**
 * @brief  Renders static elements of dashboard (header, card backgrounds, labels).
 */
void UI_Init_Dashboard(void);

/**
 * @brief  Forces full dashboard reinitialization on next update.
 */
void UI_Force_Redraw(void);

/**
 * @brief  Primary UI function: Renders complete Smart Room telemetry dashboard.
 *         Automatically initializes static layout on first run and executes
 *         flicker-free partial updates on subsequent invocations.
 * @param  temp: Room temperature (e.g. 28.5 C).
 * @param  humi: Room humidity (e.g. 65.0 %).
 * @param  light_percent: Ambient brightness (0 - 100 %).
 * @param  pir_motion: Presence detector (1: Motion detected, 0: Clear).
 * @param  relay_fan: Fan relay status (1: ON, 0: OFF).
 * @param  relay_light1: Light 1 relay status (1: ON, 0: OFF).
 * @param  relay_light2: Light 2 relay status (1: ON, 0: OFF).
 * @param  relay_dehum: Dehumidifier relay status (1: ON, 0: OFF).
 * @param  auto_mode: Control mode (1: AUTO, 0: MANUAL).
 */
void UI_Draw_Dashboard(float temp, float humi, uint8_t light_percent, uint8_t pir_motion,
                       uint8_t relay_fan, uint8_t relay_light1, uint8_t relay_light2, uint8_t relay_dehum,
                       uint8_t auto_mode);

/**
 * @brief  Convenience wrapper for UI_Draw_Dashboard accepting structured telemetry.
 * @param  data: Pointer to telemetry data structure.
 */
void UI_Display_Data(const UI_Dashboard_Data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* UI_DASHBOARD_H */
