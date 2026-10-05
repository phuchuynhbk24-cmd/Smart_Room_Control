/**
 * @file    button.h
 * @brief   Physical push buttons driver with software debounce filtering
 * @note    Mapped to Schematic:
 *          Button 1 (PA3)  -> Mode (AUTO / MANUAL)
 *          Button 2 (PB2)  -> Fan Relay
 *          Button 3 (PA10) -> Light 1 Relay
 *          Button 4 (PA11) -> Dehumidifier Relay
 */

#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BTN_MODE = 0,   /**< Button 1: PA3 */
    BTN_FAN,        /**< Button 2: PB2 */
    BTN_LIGHT1,     /**< Button 3: PA10 */
    BTN_DEHUM,      /**< Button 4: PA11 */
    BTN_COUNT
} button_id_t;

/**
 * @brief  Initializes button GPIO states and filter buffers.
 */
void button_init(void);

/**
 * @brief  Periodic button scanner and debounce state machine.
 *         Should be called regularly in the main execution loop.
 */
void button_update(void);

/**
 * @brief  Checks if a button was pressed (single click event).
 * @param  id: Button identifier (BTN_MODE, BTN_FAN, BTN_LIGHT1, BTN_DEHUM).
 * @retval true if pressed since last call, false otherwise.
 */
bool button_was_pressed(button_id_t id);

/**
 * @brief  Checks current debounced steady-state of the button.
 * @param  id: Button identifier.
 * @retval true if button is currently being held down, false otherwise.
 */
bool button_is_down(button_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */
