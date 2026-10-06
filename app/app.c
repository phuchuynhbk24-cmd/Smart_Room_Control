/**
 * @file    app.c
 * @brief   Application layer for Smart Room Control system
 * @note    Architecture Layer: APP
 *          - Interacts strictly with Middleware (input_mgr, ui_dashboard)
 *            and Device (relay) services.
 *          - Strictly NO HAL_GPIO_WritePin, HAL_SPI_*, HAL_ADC_* calls.
 */

#include "app.h"
#include "input_mgr.h"
#include "ui_dashboard.h"
#include "relay.h"
#include "sensor.h"
#include "main.h" /* For HAL_GetTick() system tick reference */
#include <stdbool.h>
#include <stdint.h>

/* Application Room Telemetry State */
static float s_temp = 28.5f;
static float s_humi = 65.0f;
static float s_light = 85.0f;
static bool  s_pir = false;

/* Operational Control Mode: 1 = AUTO, 0 = MANUAL (Default: MANUAL) */
static uint8_t s_mode = 0;

/* Executive Timers */
static uint32_t s_last_sensor_tick = 0;
static uint32_t s_last_banner_tick = 0;
static bool     s_banner_active = false;

void app_set_sensor_data(float temp, float humi, float light, bool pir)
{
    s_temp = temp;
    s_humi = humi;
    s_light = light;
    s_pir = pir;

    /* In AUTO Mode: Environmental thresholds autonomously command actuators */
    if (s_mode == 1)
    {
        relay_set(RELAY_FAN, (s_temp >= 28.5f));
        relay_set(RELAY_DEHUM, (s_humi >= 70.0f));
        relay_set(RELAY_LIGHT1, (s_light < 60.0f || s_pir));
    }

    /* Refresh Dashboard UI with live sensor data */
    UI_Draw_Dashboard(s_temp, s_humi, s_light, s_pir,
                      relay_get(RELAY_FAN),
                      relay_get(RELAY_LIGHT1),
                      relay_get(RELAY_LIGHT2),
                      relay_get(RELAY_DEHUM),
                      s_mode);
}

void app_get_sensor_data(float *temp, float *humi, float *light, bool *pir)
{
    if (temp)  *temp  = s_temp;
    if (humi)  *humi  = s_humi;
    if (light) *light = s_light;
    if (pir)   *pir   = s_pir;
}

void app_init(void)
{
    /* Initialize Middleware & Device Services */
    sensor_init();
    input_mgr_init();

    /* Initial state of relays is already OFF from relay_init() */
    s_last_banner_tick = HAL_GetTick();
    s_last_sensor_tick = HAL_GetTick();
    s_banner_active = false;

    /* Read initial sensor telemetry */
    s_temp  = sensor_get_temperature();
    s_humi  = sensor_get_humidity();
    s_light = sensor_get_light();
    s_pir   = sensor_get_pir();

    /* Render initial UI dashboard */
    UI_Draw_Dashboard(s_temp, s_humi, s_light, s_pir,
                      relay_get(RELAY_FAN),
                      relay_get(RELAY_LIGHT1),
                      relay_get(RELAY_LIGHT2),
                      relay_get(RELAY_DEHUM),
                      s_mode);
}

void app_loop(void)
{
    bool need_ui_refresh = false;
    uint32_t now = HAL_GetTick();

    /* 1. Poll High-Level Inputs via Middleware */
    input_event_t evt;
    if (input_mgr_poll(&evt, (s_mode == 1)))
    {
        switch (evt.action)
        {
            case INPUT_ACT_MODE_TOGGLE:
                s_mode = !s_mode;
                if (s_mode == 1)
                {
                    relay_set(RELAY_FAN, (s_temp >= 28.5f));
                    relay_set(RELAY_DEHUM, (s_humi >= 70.0f));
                    relay_set(RELAY_LIGHT1, (s_light < 60.0f || s_pir));
                }
                need_ui_refresh = true;
                break;

            case INPUT_ACT_FAN_TOGGLE:
                relay_toggle(RELAY_FAN);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_LIGHT1_TOGGLE:
                relay_toggle(RELAY_LIGHT1);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_LIGHT2_TOGGLE:
                relay_toggle(RELAY_LIGHT2);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_DEHUM_TOGGLE:
                relay_toggle(RELAY_DEHUM);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_NONE:
            default:
                break;
        }

        if (evt.has_banner)
        {
            UI_Draw_Banner(evt.banner_text, evt.banner_fg, evt.banner_bg);
            s_banner_active = true;
            s_last_banner_tick = now;
        }
    }

    /* 2. Periodic Sensor Sampling from Coder 2's Device Library (Every 1000ms) */
    if (now - s_last_sensor_tick >= 1000)
    {
        s_last_sensor_tick = now;
        sensor_update();

        float new_temp  = sensor_get_temperature();
        float new_humi  = sensor_get_humidity();
        float new_light = sensor_get_light();
        bool  new_pir   = sensor_get_pir();

        /* If sensor values changed, update state, evaluate AUTO rules, and refresh UI */
        if (new_temp != s_temp || new_humi != s_humi || new_light != s_light || new_pir != s_pir)
        {
            s_temp  = new_temp;
            s_humi  = new_humi;
            s_light = new_light;
            s_pir   = new_pir;

            if (s_mode == 1)
            {
                relay_set(RELAY_FAN, (s_temp >= 28.5f));
                relay_set(RELAY_DEHUM, (s_humi >= 70.0f));
                relay_set(RELAY_LIGHT1, (s_light < 60.0f || s_pir));
            }
            need_ui_refresh = true;
        }
    }

    /* 3. Auto-clear diagnostic footer banner after 2.5 seconds */
    if (s_banner_active && (now - s_last_banner_tick >= 2500))
    {
        s_banner_active = false;
        UI_Clear_Banner();
    }

    /* 4. Refresh Dashboard UI when mode, sensor, or relay states change */
    if (need_ui_refresh)
    {
        UI_Draw_Dashboard(s_temp, s_humi, s_light, s_pir,
                          relay_get(RELAY_FAN),
                          relay_get(RELAY_LIGHT1),
                          relay_get(RELAY_LIGHT2),
                          relay_get(RELAY_DEHUM),
                          s_mode);
    }
}
