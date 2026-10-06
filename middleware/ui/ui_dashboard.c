/**
 * @file    ui_dashboard.c
 * @brief   Middleware HMI Dashboard service for Smart Room Control
 * @note    Interacts only with device/ili9341 display driver
 */

#include "ui_dashboard.h"
#include "ili9341.h"
#include <stdio.h>
#include <string.h>

/* 5x7 ASCII Bitmap Font Table (ASCII 32 ' ' to 127 '°') */
static const uint8_t s_font5x7[96][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, /* 32 ' ' */
    {0x00, 0x00, 0x5F, 0x00, 0x00}, /* 33 '!' */
    {0x00, 0x07, 0x00, 0x07, 0x00}, /* 34 '"' */
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, /* 35 '#' */
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, /* 36 '$' */
    {0x23, 0x13, 0x08, 0x64, 0x62}, /* 37 '%' */
    {0x36, 0x49, 0x55, 0x22, 0x50}, /* 38 '&' */
    {0x00, 0x05, 0x03, 0x00, 0x00}, /* 39 ''' */
    {0x00, 0x1C, 0x22, 0x41, 0x00}, /* 40 '(' */
    {0x00, 0x41, 0x22, 0x1C, 0x00}, /* 41 ')' */
    {0x14, 0x08, 0x3E, 0x08, 0x14}, /* 42 '*' */
    {0x08, 0x08, 0x3E, 0x08, 0x08}, /* 43 '+' */
    {0x00, 0x50, 0x30, 0x00, 0x00}, /* 44 ',' */
    {0x08, 0x08, 0x08, 0x08, 0x08}, /* 45 '-' */
    {0x00, 0x60, 0x60, 0x00, 0x00}, /* 46 '.' */
    {0x20, 0x10, 0x08, 0x04, 0x02}, /* 47 '/' */
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, /* 48 '0' */
    {0x00, 0x42, 0x7F, 0x40, 0x00}, /* 49 '1' */
    {0x42, 0x61, 0x51, 0x49, 0x46}, /* 50 '2' */
    {0x21, 0x41, 0x45, 0x4B, 0x31}, /* 51 '3' */
    {0x18, 0x14, 0x12, 0x7F, 0x10}, /* 52 '4' */
    {0x27, 0x45, 0x45, 0x45, 0x39}, /* 53 '5' */
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, /* 54 '6' */
    {0x01, 0x71, 0x09, 0x05, 0x03}, /* 55 '7' */
    {0x36, 0x49, 0x49, 0x49, 0x36}, /* 56 '8' */
    {0x06, 0x49, 0x49, 0x29, 0x1E}, /* 57 '9' */
    {0x00, 0x36, 0x36, 0x00, 0x00}, /* 58 ':' */
    {0x00, 0x56, 0x36, 0x00, 0x00}, /* 59 ';' */
    {0x08, 0x14, 0x22, 0x41, 0x00}, /* 60 '<' */
    {0x14, 0x14, 0x14, 0x14, 0x14}, /* 61 '=' */
    {0x00, 0x41, 0x22, 0x14, 0x08}, /* 62 '>' */
    {0x02, 0x01, 0x51, 0x09, 0x06}, /* 63 '?' */
    {0x32, 0x49, 0x79, 0x41, 0x3E}, /* 64 '@' */
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, /* 65 'A' */
    {0x7F, 0x49, 0x49, 0x49, 0x36}, /* 66 'B' */
    {0x3E, 0x41, 0x41, 0x41, 0x22}, /* 67 'C' */
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, /* 68 'D' */
    {0x7F, 0x49, 0x49, 0x49, 0x41}, /* 69 'E' */
    {0x7F, 0x09, 0x09, 0x09, 0x01}, /* 70 'F' */
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, /* 71 'G' */
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, /* 72 'H' */
    {0x00, 0x41, 0x7F, 0x41, 0x00}, /* 73 'I' */
    {0x20, 0x40, 0x41, 0x3F, 0x01}, /* 74 'J' */
    {0x7F, 0x08, 0x14, 0x22, 0x41}, /* 75 'K' */
    {0x7F, 0x40, 0x40, 0x40, 0x40}, /* 76 'L' */
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, /* 77 'M' */
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, /* 78 'N' */
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, /* 79 'O' */
    {0x7F, 0x09, 0x09, 0x09, 0x06}, /* 80 'P' */
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, /* 81 'Q' */
    {0x7F, 0x09, 0x19, 0x29, 0x46}, /* 82 'R' */
    {0x46, 0x49, 0x49, 0x49, 0x31}, /* 83 'S' */
    {0x01, 0x01, 0x7F, 0x01, 0x01}, /* 84 'T' */
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, /* 85 'U' */
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, /* 86 'V' */
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, /* 87 'W' */
    {0x63, 0x14, 0x08, 0x14, 0x63}, /* 88 'X' */
    {0x07, 0x08, 0x70, 0x08, 0x07}, /* 89 'Y' */
    {0x61, 0x51, 0x49, 0x45, 0x43}, /* 90 'Z' */
    {0x00, 0x7F, 0x41, 0x41, 0x00}, /* 91 '[' */
    {0x02, 0x04, 0x08, 0x10, 0x20}, /* 92 '\' */
    {0x00, 0x41, 0x41, 0x7F, 0x00}, /* 93 ']' */
    {0x04, 0x02, 0x01, 0x02, 0x04}, /* 94 '^' */
    {0x40, 0x40, 0x40, 0x40, 0x40}, /* 95 '_' */
    {0x00, 0x01, 0x02, 0x04, 0x00}, /* 96 '`' */
    {0x20, 0x54, 0x54, 0x54, 0x78}, /* 97 'a' */
    {0x7F, 0x48, 0x44, 0x44, 0x38}, /* 98 'b' */
    {0x38, 0x44, 0x44, 0x44, 0x20}, /* 99 'c' */
    {0x38, 0x44, 0x44, 0x48, 0x7F}, /* 100 'd' */
    {0x38, 0x54, 0x54, 0x54, 0x18}, /* 101 'e' */
    {0x08, 0x7E, 0x09, 0x01, 0x02}, /* 102 'f' */
    {0x0C, 0x52, 0x52, 0x52, 0x3E}, /* 103 'g' */
    {0x7F, 0x08, 0x04, 0x04, 0x78}, /* 104 'h' */
    {0x00, 0x44, 0x7D, 0x40, 0x00}, /* 105 'i' */
    {0x20, 0x40, 0x44, 0x3D, 0x00}, /* 106 'j' */
    {0x7F, 0x10, 0x28, 0x44, 0x00}, /* 107 'k' */
    {0x00, 0x41, 0x7F, 0x40, 0x00}, /* 108 'l' */
    {0x7C, 0x04, 0x18, 0x04, 0x78}, /* 109 'm' */
    {0x7C, 0x08, 0x04, 0x04, 0x78}, /* 110 'n' */
    {0x38, 0x44, 0x44, 0x44, 0x38}, /* 111 'o' */
    {0x7C, 0x14, 0x14, 0x14, 0x08}, /* 112 'p' */
    {0x08, 0x14, 0x14, 0x18, 0x7C}, /* 113 'q' */
    {0x7C, 0x08, 0x04, 0x04, 0x08}, /* 114 'r' */
    {0x48, 0x54, 0x54, 0x54, 0x20}, /* 115 's' */
    {0x04, 0x3F, 0x44, 0x40, 0x20}, /* 116 't' */
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, /* 117 'u' */
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, /* 118 'v' */
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, /* 119 'w' */
    {0x44, 0x28, 0x10, 0x28, 0x44}, /* 120 'x' */
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, /* 121 'y' */
    {0x44, 0x64, 0x54, 0x4C, 0x44}, /* 122 'z' */
    {0x00, 0x08, 0x36, 0x41, 0x00}, /* 123 '{' */
    {0x00, 0x00, 0x7F, 0x00, 0x00}, /* 124 '|' */
    {0x00, 0x41, 0x36, 0x08, 0x00}, /* 125 '}' */
    {0x08, 0x08, 0x2A, 0x1C, 0x08}, /* 126 '~' */
    {0x06, 0x09, 0x09, 0x06, 0x00}  /* 127 '°' */
};

static uint8_t s_char_stream[384];

void UI_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size)
{
    if (c < 32 || c > 127)
    {
        c = ' ';
    }
    uint8_t c_idx = (uint8_t)(c - 32);

    uint8_t c_hi = (uint8_t)(color >> 8);
    uint8_t c_lo = (uint8_t)(color & 0xFF);
    uint8_t b_hi = (uint8_t)(bg >> 8);
    uint8_t b_lo = (uint8_t)(bg & 0xFF);

    if (size == 1)
    {
        if ((x + 6) > ILI9341_WIDTH || (y + 8) > ILI9341_HEIGHT)
        {
            return;
        }

        uint16_t idx = 0;
        for (uint8_t r = 0; r < 8; r++)
        {
            uint8_t bit_mask = (uint8_t)(1 << r);
            for (uint8_t col = 0; col < 5; col++)
            {
                if (s_font5x7[c_idx][col] & bit_mask)
                {
                    s_char_stream[idx++] = c_hi;
                    s_char_stream[idx++] = c_lo;
                }
                else
                {
                    s_char_stream[idx++] = b_hi;
                    s_char_stream[idx++] = b_lo;
                }
            }
            s_char_stream[idx++] = b_hi;
            s_char_stream[idx++] = b_lo;
        }

        ili9341_draw_buffer(x, y, 6, 8, s_char_stream, 96);
    }
    else if (size == 2)
    {
        if ((x + 12) > ILI9341_WIDTH || (y + 16) > ILI9341_HEIGHT)
        {
            return;
        }

        uint16_t idx = 0;
        for (uint8_t r = 0; r < 16; r++)
        {
            uint8_t font_r = (uint8_t)(r >> 1);
            uint8_t bit_mask = (uint8_t)(1 << font_r);
            for (uint8_t col = 0; col < 10; col++)
            {
                uint8_t font_col = (uint8_t)(col >> 1);
                if (s_font5x7[c_idx][font_col] & bit_mask)
                {
                    s_char_stream[idx++] = c_hi;
                    s_char_stream[idx++] = c_lo;
                }
                else
                {
                    s_char_stream[idx++] = b_hi;
                    s_char_stream[idx++] = b_lo;
                }
            }
            s_char_stream[idx++] = b_hi;
            s_char_stream[idx++] = b_lo;
            s_char_stream[idx++] = b_hi;
            s_char_stream[idx++] = b_lo;
        }

        ili9341_draw_buffer(x, y, 12, 16, s_char_stream, 384);
    }
    else
    {
        for (int8_t i = 0; i < 5; i++)
        {
            uint8_t line = s_font5x7[c_idx][i];
            for (int8_t j = 0; j < 8; j++)
            {
                uint16_t pixel_color = (line & (1 << j)) ? color : bg;
                ili9341_fill_rect(x + (i * size), y + (j * size), size, size, pixel_color);
            }
        }
        ili9341_fill_rect(x + (5 * size), y, size, 8 * size, bg);
    }
}

void UI_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size)
{
    if (str == NULL)
    {
        return;
    }

    uint16_t cursor_x = x;
    uint8_t char_width = (uint8_t)(6 * size);

    while (*str)
    {
        if (cursor_x + char_width > ILI9341_WIDTH)
        {
            break;
        }
        UI_DrawChar(cursor_x, y, *str, color, bg, size);
        cursor_x += char_width;
        str++;
    }
}

void UI_DrawCard(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t border_color, uint16_t bg_color)
{
    ili9341_fill_rect(x, y, w, 1, border_color);
    ili9341_fill_rect(x, y + h - 1, w, 1, border_color);
    ili9341_fill_rect(x, y, 1, h, border_color);
    ili9341_fill_rect(x + w - 1, y, 1, h, border_color);
    ili9341_fill_rect(x + 1, y + 1, w - 2, h - 2, bg_color);
}

void UI_DrawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t fg_color, uint16_t bg_color)
{
    if (percent > 100)
    {
        percent = 100;
    }

    uint16_t fill_w = (uint16_t)((w * percent) / 100);
    uint16_t rest_w = (uint16_t)(w - fill_w);

    if (fill_w > 0)
    {
        ili9341_fill_rect(x, y, fill_w, h, fg_color);
    }
    if (rest_w > 0)
    {
        ili9341_fill_rect(x + fill_w, y, rest_w, h, bg_color);
    }
}

void UI_Init_Dashboard(void)
{
    ili9341_fill_screen(UI_COLOR_BG);

    /* 1. Top Navigation Bar */
    ili9341_fill_rect(0, 0, ILI9341_WIDTH, 36, 0x0000);
    ili9341_fill_rect(0, 36, ILI9341_WIDTH, 2, 0x2104);
    UI_DrawString(12, 10, "SMART ROOM", UI_COLOR_TEXT_MAIN, 0x0000, 2);

    /* 2. Sensor Card Grid */
    UI_DrawCard(8, 44, 110, 68, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(16, 50, "TEMP", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    UI_DrawCard(122, 44, 110, 68, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(130, 50, "HUMIDITY", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    UI_DrawCard(8, 118, 110, 68, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(16, 124, "LIGHT", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    UI_DrawCard(122, 118, 110, 68, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(130, 124, "MOTION", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    /* 3. Actuator Control Center Card */
    UI_DrawCard(8, 192, 224, 94, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);

    UI_DrawString(16, 219, "FAN", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);
    UI_DrawString(16, 253, "DEHUM", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);
    UI_DrawString(126, 219, "LIGHT 1", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);
    UI_DrawString(126, 253, "LIGHT 2", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);

    /* 4. Bottom System Status Bar */
    UI_Clear_Banner();
}

static bool s_dashboard_initialized = false;

void UI_Force_Redraw(void)
{
    s_dashboard_initialized = false;
}

void UI_Draw_Banner(const char *msg, uint16_t fg_color, uint16_t bg_color)
{
    if (msg == NULL)
    {
        return;
    }
    ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, bg_color);
    UI_DrawString(8, 301, msg, fg_color, bg_color, 1);
}

void UI_Clear_Banner(void)
{
    ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
    UI_DrawString(12, 301, "SYS: RUNNING | STM32F103", UI_COLOR_TEXT_MUTED, 0x0842, 1);
}

void UI_Draw_Dashboard(float temp, float humi, float light, bool pir_motion,
                       uint8_t relay_fan, uint8_t relay_light1, uint8_t relay_light2, uint8_t relay_dehum,
                       uint8_t auto_mode)
{
    if (!s_dashboard_initialized)
    {
        UI_Init_Dashboard();
        s_dashboard_initialized = true;
    }

    /* Auto / Manual Mode Badge */
    if (auto_mode)
    {
        ili9341_fill_rect(156, 8, 74, 20, 0x001F);
        UI_DrawString(164, 11, "AUTO  ", UI_COLOR_TEXT_MAIN, 0x001F, 1);
    }
    else
    {
        ili9341_fill_rect(156, 8, 74, 20, 0x6320);
        UI_DrawString(164, 11, "MANUAL", UI_COLOR_TEXT_MAIN, 0x6320, 1);
    }

    char buf[16];

    /* Temperature */
    snprintf(buf, sizeof(buf), "%2d.%1d", (int)temp, ((int)(temp * 10)) % 10);
    UI_DrawString(16, 68, buf, UI_COLOR_TEMP, UI_COLOR_CARD_BG, 2);
    UI_DrawChar(76, 68, 127, UI_COLOR_TEMP, UI_COLOR_CARD_BG, 1);
    UI_DrawChar(84, 68, 'C', UI_COLOR_TEMP, UI_COLOR_CARD_BG, 1);

    uint8_t temp_bar = (uint8_t)((temp > 40.0f) ? 100 : ((temp < 10.0f) ? 0 : (uint8_t)((temp - 10.0f) * 3.33f)));
    UI_DrawProgressBar(16, 96, 94, 6, temp_bar, UI_COLOR_TEMP, 0x0821);

    /* Humidity */
    snprintf(buf, sizeof(buf), "%2d.%1d", (int)humi, ((int)(humi * 10)) % 10);
    UI_DrawString(130, 68, buf, UI_COLOR_HUMI, UI_COLOR_CARD_BG, 2);
    UI_DrawChar(190, 68, '%', UI_COLOR_HUMI, UI_COLOR_CARD_BG, 1);

    uint8_t humi_bar = (humi > 100.0f) ? 100 : (uint8_t)humi;
    UI_DrawProgressBar(130, 96, 94, 6, humi_bar, UI_COLOR_HUMI, 0x0821);

    /* Light (float percentage: e.g. 85.5% or 100%) */
    if (light >= 99.9f)
    {
        snprintf(buf, sizeof(buf), "100%%");
    }
    else
    {
        snprintf(buf, sizeof(buf), "%2d.%1d%%", (int)light, ((int)(light * 10.0f)) % 10);
    }
    UI_DrawString(16, 142, buf, UI_COLOR_LIGHT, UI_COLOR_CARD_BG, 2);
    uint8_t light_bar = (uint8_t)((light > 100.0f) ? 100 : ((light < 0.0f) ? 0 : light));
    UI_DrawProgressBar(16, 170, 94, 6, light_bar, UI_COLOR_LIGHT, 0x0821);

    /* Motion Badge */
    if (pir_motion)
    {
        ili9341_fill_rect(130, 142, 94, 20, UI_COLOR_ALERT);
        UI_DrawString(142, 145, "ACTIVE ", UI_COLOR_TEXT_MAIN, UI_COLOR_ALERT, 1);
    }
    else
    {
        ili9341_fill_rect(130, 142, 94, 20, UI_COLOR_IDLE);
        UI_DrawString(142, 145, "CLEAR  ", UI_COLOR_TEXT_MUTED, UI_COLOR_IDLE, 1);
    }

    /* Actuators Header Label */
    if (auto_mode)
    {
        UI_DrawString(14, 200, "RELAY ACTUATORS [AUTO LOCK]", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);
    }
    else
    {
        UI_DrawString(14, 200, "RELAY ACTUATORS [MANUAL]   ", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);
    }

    /* Fan Relay */
    if (relay_fan)
    {
        ili9341_fill_rect(64, 214, 46, 18, UI_COLOR_ON);
        UI_DrawString(73, 219, "ON ", ILI9341_BLACK, UI_COLOR_ON, 1);
    }
    else
    {
        ili9341_fill_rect(64, 214, 46, 18, UI_COLOR_OFF);
        UI_DrawString(70, 219, "OFF", UI_COLOR_TEXT_MUTED, UI_COLOR_OFF, 1);
    }

    /* Light 1 Relay */
    if (relay_light1)
    {
        ili9341_fill_rect(174, 214, 46, 18, UI_COLOR_ON);
        UI_DrawString(183, 219, "ON ", ILI9341_BLACK, UI_COLOR_ON, 1);
    }
    else
    {
        ili9341_fill_rect(174, 214, 46, 18, UI_COLOR_OFF);
        UI_DrawString(180, 219, "OFF", UI_COLOR_TEXT_MUTED, UI_COLOR_OFF, 1);
    }

    /* Dehumidifier Relay */
    if (relay_dehum)
    {
        ili9341_fill_rect(64, 248, 46, 18, UI_COLOR_ON);
        UI_DrawString(73, 253, "ON ", ILI9341_BLACK, UI_COLOR_ON, 1);
    }
    else
    {
        ili9341_fill_rect(64, 248, 46, 18, UI_COLOR_OFF);
        UI_DrawString(70, 253, "OFF", UI_COLOR_TEXT_MUTED, UI_COLOR_OFF, 1);
    }

    /* Light 2 Relay */
    if (relay_light2)
    {
        ili9341_fill_rect(174, 248, 46, 18, UI_COLOR_ON);
        UI_DrawString(183, 253, "ON ", ILI9341_BLACK, UI_COLOR_ON, 1);
    }
    else
    {
        ili9341_fill_rect(174, 248, 46, 18, UI_COLOR_OFF);
        UI_DrawString(180, 253, "OFF", UI_COLOR_TEXT_MUTED, UI_COLOR_OFF, 1);
    }
}

void UI_Display_Data(const UI_Dashboard_Data_t *data)
{
    if (data == NULL)
    {
        return;
    }
    UI_Draw_Dashboard(data->temp, data->humi, data->light, data->pir_motion,
                      data->relay_fan, data->relay_light1, data->relay_light2, data->relay_dehum,
                      data->auto_mode);
}
