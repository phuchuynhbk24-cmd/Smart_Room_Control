/**
 * @file    input_mgr.c
 * @brief   Middleware input manager service coordinating physical buttons and TFT touch
 * @note    Enforces absolute priority of physical hardware buttons over touch screen
 */

#include "input_mgr.h"
#include "button.h"
#include "xpt2046.h"
#include <stdio.h>
#include <string.h>

static uint32_t s_last_touch_tick = 0;
static uint32_t s_last_btn_activity_tick = 0;
static bool s_touch_was_pressed = false;

void input_mgr_init(void)
{
    s_last_touch_tick = 0;
    s_last_btn_activity_tick = 0;
    s_touch_was_pressed = false;
}

bool input_mgr_poll(input_event_t *event, bool is_auto_mode)
{
    if (event == NULL)
    {
        return false;
    }

    event->action = INPUT_ACT_NONE;
    event->source = INPUT_SRC_NONE;
    event->has_banner = false;
    event->banner_text[0] = '\0';
    event->banner_bg = 0x0842;
    event->banner_fg = 0xFFFF;

    uint32_t now = HAL_GetTick();

    /* 1. Hardware Push Buttons Scanning (HIGHEST PRIORITY) */
    button_update();

    bool btn_any_down = button_is_down(BTN_MODE) || button_is_down(BTN_FAN) ||
                        button_is_down(BTN_LIGHT1) || button_is_down(BTN_DEHUM);
    bool btn_event_occurred = false;

    if (button_was_pressed(BTN_MODE))
    {
        btn_event_occurred = true;
        event->action = INPUT_ACT_MODE_TOGGLE;
        event->source = INPUT_SRC_BUTTON;
        event->has_banner = true;
        snprintf(event->banner_text, sizeof(event->banner_text), "[HW BTN] MODE TOGGLED");
        event->banner_bg = 0x0320;
        event->banner_fg = 0xFFFF;
    }
    else if (button_was_pressed(BTN_FAN))
    {
        btn_event_occurred = true;
        event->source = INPUT_SRC_BUTTON;
        event->has_banner = true;
        if (!is_auto_mode)
        {
            event->action = INPUT_ACT_FAN_TOGGLE;
            snprintf(event->banner_text, sizeof(event->banner_text), "[HW BTN] FAN TOGGLED");
            event->banner_bg = 0x0320;
            event->banner_fg = 0xFFFF;
        }
        else
        {
            snprintf(event->banner_text, sizeof(event->banner_text), "[AUTO-LOCK] BTN FAN");
            event->banner_bg = 0x4800;
            event->banner_fg = 0xFFE0; /* Yellow */
        }
    }
    else if (button_was_pressed(BTN_LIGHT1))
    {
        btn_event_occurred = true;
        event->source = INPUT_SRC_BUTTON;
        event->has_banner = true;
        if (!is_auto_mode)
        {
            event->action = INPUT_ACT_LIGHT1_TOGGLE;
            snprintf(event->banner_text, sizeof(event->banner_text), "[HW BTN] LIGHT1 TOGGLED");
            event->banner_bg = 0x0320;
            event->banner_fg = 0xFFFF;
        }
        else
        {
            snprintf(event->banner_text, sizeof(event->banner_text), "[AUTO-LOCK] BTN LIGHT 1");
            event->banner_bg = 0x4800;
            event->banner_fg = 0xFFE0;
        }
    }
    else if (button_was_pressed(BTN_DEHUM))
    {
        btn_event_occurred = true;
        event->source = INPUT_SRC_BUTTON;
        event->has_banner = true;
        if (!is_auto_mode)
        {
            event->action = INPUT_ACT_DEHUM_TOGGLE;
            snprintf(event->banner_text, sizeof(event->banner_text), "[HW BTN] DEHUM TOGGLED");
            event->banner_bg = 0x0320;
            event->banner_fg = 0xFFFF;
        }
        else
        {
            snprintf(event->banner_text, sizeof(event->banner_text), "[AUTO-LOCK] BTN DEHUM");
            event->banner_bg = 0x4800;
            event->banner_fg = 0xFFE0;
        }
    }

    if (btn_event_occurred)
    {
        s_last_btn_activity_tick = now;
        return true;
    }

    /* 2. TFT Touch Screen Scanning (LOWER PRIORITY than Physical Buttons) */
    if (xpt2046_is_touched())
    {
        /* Priority Arbitration: Lock out touch if physical button is held down or active recently */
        if (btn_any_down || (now - s_last_btn_activity_tick < 400))
        {
            if (!s_touch_was_pressed)
            {
                s_touch_was_pressed = true;
                event->source = INPUT_SRC_TOUCH;
                event->action = INPUT_ACT_NONE;
                event->has_banner = true;
                snprintf(event->banner_text, sizeof(event->banner_text), "[HW PRIORITY] BTN OVERRIDE TOUCH");
                event->banner_bg = 0x6008; /* Plum warning */
                event->banner_fg = 0xFFFF;
                return true;
            }
            return false;
        }

        uint16_t touch_x = 0;
        uint16_t touch_y = 0;

        if (xpt2046_get_xy(&touch_x, &touch_y))
        {
            if (!s_touch_was_pressed && (now - s_last_touch_tick > 180))
            {
                s_touch_was_pressed = true;
                s_last_touch_tick = now;
                event->source = INPUT_SRC_TOUCH;
                event->has_banner = true;

                /* Hitbox 1: Header Mode Button (Top-Right: y <= 50, x >= 110) */
                if (touch_y <= 50 && touch_x >= 110)
                {
                    event->action = INPUT_ACT_MODE_TOGGLE;
                    snprintf(event->banner_text, sizeof(event->banner_text), "%s | TOUCH:(%3d,%3d)",
                             is_auto_mode ? "MANU" : "AUTO", touch_x, touch_y);
                    event->banner_bg = 0x0842;
                    event->banner_fg = 0xFFE0;
                    return true;
                }
                /* Hitbox 2: Actuator Control Matrix (y: 190..295) */
                else if (touch_y >= 190 && touch_y <= 295)
                {
                    if (is_auto_mode)
                    {
                        /* Locked in Auto mode */
                        snprintf(event->banner_text, sizeof(event->banner_text), "[AUTO-LOCK] T:(%3d,%3d)", touch_x, touch_y);
                        event->banner_bg = 0x4800;
                        event->banner_fg = 0xFFE0;
                        return true;
                    }
                    else
                    {
                        if (touch_x < 120)
                        {
                            event->action = (touch_y < 242) ? INPUT_ACT_FAN_TOGGLE : INPUT_ACT_DEHUM_TOGGLE;
                        }
                        else
                        {
                            event->action = (touch_y < 242) ? INPUT_ACT_LIGHT1_TOGGLE : INPUT_ACT_LIGHT2_TOGGLE;
                        }

                        snprintf(event->banner_text, sizeof(event->banner_text), "MANU | TOUCH:(%3d,%3d)", touch_x, touch_y);
                        event->banner_bg = 0x0842;
                        event->banner_fg = 0xFFFF;
                        return true;
                    }
                }
                else
                {
                    /* Diagnostic coordinate echo */
                    snprintf(event->banner_text, sizeof(event->banner_text), "%s | TOUCH:(%3d,%3d)",
                             is_auto_mode ? "AUTO" : "MANU", touch_x, touch_y);
                    event->banner_bg = 0x0842;
                    event->banner_fg = 0x9CD3;
                    return true;
                }
            }
        }
    }
    else
    {
        s_touch_was_pressed = false;
    }

    return false;
}
