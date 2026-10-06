/**
 * @file    ui_dashboard.h
 * @brief   Middleware HMI Dashboard service for Smart Room Control
 * @note    Runs on top of device/ili9341 display driver (240x320 portrait)
 */

#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

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
    float    temp;           /**< Room temperature in degrees Celsius (float) */
    float    humi;           /**< Relative humidity in percent (float) */
    float    light;          /**< Ambient light intensity (float, 0.0 - 100.0%) */
    bool     pir_motion;     /**< PIR sensor motion status (bool, true: Detected, false: Clear) */
    uint8_t  relay_fan;      /**< Fan relay state (1: ON, 0: OFF) */
    uint8_t  relay_light1;   /**< Light 1 relay state (1: ON, 0: OFF) */
    uint8_t  relay_light2;   /**< Light 2 relay state (1: ON, 0: OFF) */
    uint8_t  relay_dehum;    /**< Dehumidifier relay state (1: ON, 0: OFF) */
    uint8_t  auto_mode;      /**< Operation mode (1: AUTO, 0: MANUAL) */
} UI_Dashboard_Data_t;

/* Public Middleware UI API */

void UI_Init_Dashboard(void);
void UI_Force_Redraw(void);

void UI_Draw_Dashboard(float temp, float humi, float light, bool pir_motion,
                       uint8_t relay_fan, uint8_t relay_light1, uint8_t relay_light2, uint8_t relay_dehum,
                       uint8_t auto_mode);

void UI_Display_Data(const UI_Dashboard_Data_t *data);

void UI_Draw_Banner(const char *msg, uint16_t fg_color, uint16_t bg_color);
void UI_Clear_Banner(void);

void UI_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size);
void UI_DrawCard(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t border_color, uint16_t bg_color);
void UI_DrawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t fg_color, uint16_t bg_color);

#ifdef __cplusplus
}
#endif

#endif /* UI_DASHBOARD_H */
