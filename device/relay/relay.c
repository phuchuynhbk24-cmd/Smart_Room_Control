/**
 * @file    relay.c
 * @brief   Device abstraction layer for 4-channel relay actuators
 * @note    Direct hardware interaction with GPIO pins configured in CubeMX:
 *          - RELAY_FAN:   PB4
 *          - RELAY_LIGHT1: PA15
 *          - RELAY_LIGHT2: PB6
 *          - RELAY_DEHUM:  PA12
 */

#include "relay.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    bool state;
} relay_hw_t;

static relay_hw_t s_relays[RELAY_COUNT] = {
    [RELAY_FAN]    = { .port = RELAY_FAN_GPIO_Port,    .pin = RELAY_FAN_Pin,    .state = false },
    [RELAY_LIGHT1] = { .port = RELAY_LIGHT1_GPIO_Port, .pin = RELAY_LIGHT1_Pin, .state = false },
    [RELAY_LIGHT2] = { .port = RELAY_LIGHT2_GPIO_Port, .pin = RELAY_LIGHT2_Pin, .state = false },
    [RELAY_DEHUM]  = { .port = RELAY_DEHUM_GPIO_Port,  .pin = RELAY_DEHUM_Pin,  .state = false },
};

void relay_init(void)
{
    for (uint8_t i = 0; i < RELAY_COUNT; i++)
    {
        s_relays[i].state = false;
        HAL_GPIO_WritePin(s_relays[i].port, s_relays[i].pin, GPIO_PIN_RESET);
    }
}

void relay_set(relay_id_t id, bool state)
{
    if (id >= RELAY_COUNT)
    {
        return;
    }

    s_relays[id].state = state;
    HAL_GPIO_WritePin(s_relays[id].port, s_relays[id].pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void relay_toggle(relay_id_t id)
{
    if (id >= RELAY_COUNT)
    {
        return;
    }

    relay_set(id, !s_relays[id].state);
}

bool relay_get(relay_id_t id)
{
    if (id >= RELAY_COUNT)
    {
        return false;
    }

    return s_relays[id].state;
}
