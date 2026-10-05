/**
 * @file    input_mgr.h
 * @brief   Middleware input manager service coordinating physical buttons and TFT touch
 * @note    Enforces absolute priority of physical hardware buttons over touch screen
 */

#ifndef INPUT_MGR_H
#define INPUT_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    INPUT_ACT_NONE = 0,
    INPUT_ACT_MODE_TOGGLE,
    INPUT_ACT_FAN_TOGGLE,
    INPUT_ACT_LIGHT1_TOGGLE,
    INPUT_ACT_LIGHT2_TOGGLE,
    INPUT_ACT_DEHUM_TOGGLE,
} input_act_t;

typedef enum {
    INPUT_SRC_NONE = 0,
    INPUT_SRC_BUTTON,
    INPUT_SRC_TOUCH
} input_src_t;

typedef struct {
    input_act_t action;
    input_src_t source;
    bool has_banner;
    char banner_text[36];
    uint16_t banner_fg;
    uint16_t banner_bg;
} input_event_t;

/**
 * @brief  Initializes input manager states and timers.
 */
void input_mgr_init(void);

/**
 * @brief  Polls inputs, applies hardware priority arbitration, and produces events.
 * @param  event: Pointer to output event struct.
 * @param  is_auto_mode: true if system currently in AUTO mode, false if MANUAL.
 * @retval true if an action or banner event occurred, false otherwise.
 */
bool input_mgr_poll(input_event_t *event, bool is_auto_mode);

#ifdef __cplusplus
}
#endif

#endif /* INPUT_MGR_H */
