/**
 * @file    sensor.c
 * @brief   Default / Stub implementation of Sensor Device driver
 * @note    Architecture Layer: DEVICE
 *          - Marked __weak so Coder 2 can override any of these functions
 *            or replace this file directly with their sensor driver code.
 */

#include "sensor.h"

__attribute__((weak)) void sensor_init(void)
{
    /* Coder 2 initializes sensor hardware / GPIOs here */
}

__attribute__((weak)) void sensor_update(void)
{
    /* Coder 2 triggers sensor sampling / polling here */
}

__attribute__((weak)) float sensor_get_temperature(void)
{
    /* Default telemetry fallback */
    return 28.5f;
}

__attribute__((weak)) float sensor_get_humidity(void)
{
    /* Default telemetry fallback */
    return 65.0f;
}

__attribute__((weak)) float sensor_get_light(void)
{
    /* Default telemetry fallback */
    return 85.0f;
}

__attribute__((weak)) bool sensor_get_pir(void)
{
    /* Default telemetry fallback */
    return false;
}
