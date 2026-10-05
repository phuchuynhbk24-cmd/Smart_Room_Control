/**
 * @file    button.c
 * @brief   Physical push buttons driver with software debounce filtering
 * @note    Mapped to KiCad Schematic:
 *          Button 1 (SW2) -> PA3  (Mode: AUTO / MANUAL)
 *          Button 2 (SW4) -> PB2  (Relay: FAN)
 *          Button 3 (SW3) -> PA10 (Relay: LIGHT 1)
 *          Button 4 (SW5) -> PA11 (Relay: DEHUMIDIFIER)
 *
 *          Hardware circuit: Active-LOW, 10k Pull-Up, 100nF to GND (tau = 1ms).
 *          Software debounce: 25ms threshold to ensure glitch-free triggering.
 */

#include "button.h"

#define BUTTON_DEBOUNCE_MS  25

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t stable_state;       /* 1 = pressed (pin LOW), 0 = released (pin HIGH) */
    uint8_t last_raw;           /* Previous raw read */
    uint32_t last_change_tick;  /* Timestamp of last raw change */
    bool was_pressed;           /* Edge trigger latch */
} button_dev_t;

static button_dev_t s_buttons[BTN_COUNT] = {
    [BTN_MODE]   = { .port = BTN_1_GPIO_Port, .pin = BTN_1_Pin },
    [BTN_FAN]    = { .port = BTN_2_GPIO_Port, .pin = BTN_2_Pin },
    [BTN_LIGHT1] = { .port = BTN_3_GPIO_Port, .pin = BTN_3_Pin },
    [BTN_DEHUM]  = { .port = BTN_4_GPIO_Port, .pin = BTN_4_Pin },
};

void button_init(void)
{
    uint32_t now = HAL_GetTick();

    for (uint8_t i = 0; i < BTN_COUNT; i++)
    {
        /* Active LOW: GPIO_PIN_RESET means button is pressed */
        uint8_t raw = (HAL_GPIO_ReadPin(s_buttons[i].port, s_buttons[i].pin) == GPIO_PIN_RESET) ? 1 : 0;
        s_buttons[i].stable_state = raw;
        s_buttons[i].last_raw = raw;
        s_buttons[i].last_change_tick = now;
        s_buttons[i].was_pressed = false;
    }
}

void button_update(void)
{
    uint32_t now = HAL_GetTick();

    for (uint8_t i = 0; i < BTN_COUNT; i++)
    {
        uint8_t raw = (HAL_GPIO_ReadPin(s_buttons[i].port, s_buttons[i].pin) == GPIO_PIN_RESET) ? 1 : 0;

        if (raw != s_buttons[i].last_raw)
        {
            s_buttons[i].last_raw = raw;
            s_buttons[i].last_change_tick = now;
        }

        if ((now - s_buttons[i].last_change_tick) >= BUTTON_DEBOUNCE_MS)
        {
            if (raw != s_buttons[i].stable_state)
            {
                s_buttons[i].stable_state = raw;
                if (s_buttons[i].stable_state == 1)
                {
                    s_buttons[i].was_pressed = true;
                }
            }
        }
    }
}

bool button_was_pressed(button_id_t id)
{
    if (id >= BTN_COUNT)
    {
        return false;
    }

    bool pressed = s_buttons[id].was_pressed;
    s_buttons[id].was_pressed = false;
    return pressed;
}

bool button_is_down(button_id_t id)
{
    if (id >= BTN_COUNT)
    {
        return false;
    }

    return (s_buttons[id].stable_state == 1);
}
