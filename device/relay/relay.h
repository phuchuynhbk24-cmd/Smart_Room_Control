/**
 * @file    relay.h
 * @brief   Device abstraction layer for 4-channel relay actuators
 * @note    Mapped to KiCad Schematic:
 *          - RELAY 1 (FAN):    PB4  (Pin 45)
 *          - RELAY 2 (LIGHT 1): PA15 (Pin 38)
 *          - RELAY 3 (LIGHT 2): PB6  (Pin 42)
 *          - RELAY 4 (DEHUM):  PA12 (Pin 33)
 */

#ifndef RELAY_H
#define RELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    RELAY_FAN = 0,      /**< Fan relay: PB4 */
    RELAY_LIGHT1,       /**< Light 1 relay: PA15 */
    RELAY_LIGHT2,       /**< Light 2 relay: PB6 */
    RELAY_DEHUM,        /**< Dehumidifier relay: PA12 */
    RELAY_COUNT
} relay_id_t;

/**
 * @brief  Initializes relay outputs to default OFF state.
 */
void relay_init(void);

/**
 * @brief  Sets a relay output state.
 * @param  id: Relay identifier.
 * @param  state: true = ON (energized), false = OFF (de-energized).
 */
void relay_set(relay_id_t id, bool state);

/**
 * @brief  Toggles a relay output state.
 * @param  id: Relay identifier.
 */
void relay_toggle(relay_id_t id);

/**
 * @brief  Gets current state of a relay.
 * @param  id: Relay identifier.
 * @retval true if ON, false if OFF.
 */
bool relay_get(relay_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* RELAY_H */
