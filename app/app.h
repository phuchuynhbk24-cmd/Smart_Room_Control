/**
 * @file    app.h
 * @brief   Application layer for Smart Room Control system
 * @note    Contains top-level business and operational logic.
 *          No direct HAL hardware calls allowed.
 */

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * @brief  Initializes application states, services, and renders initial UI.
 */
void app_init(void);

/**
 * @brief  Main application executive loop step.
 *         Should be called continuously from main's while(1).
 */
void app_loop(void);

/**
 * @brief  Receives sensor telemetry from Coder 2 and updates system state & TFT.
 * @param  temp: Room temperature in degrees Celsius (float).
 * @param  humi: Relative humidity in percent (float).
 * @param  light: Ambient light intensity (float, 0.0 to 100.0%).
 * @param  pir: Motion detection state (bool, true: motion detected, false: clear).
 */
void app_set_sensor_data(float temp, float humi, float light, bool pir);

/**
 * @brief  Gets current sensor telemetry data stored in system.
 */
void app_get_sensor_data(float *temp, float *humi, float *light, bool *pir);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
