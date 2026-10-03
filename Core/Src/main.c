/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341.h"
#include "xpt2046.h"
#include "ui_dashboard.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
volatile bool g_exti_btn_mode_flag  = false;
volatile bool g_exti_btn_fan_flag   = false;
volatile bool g_exti_btn_light_flag = false;
volatile bool g_exti_btn_dehum_flag = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
    {0x08, 0x04, 0x08, 0x10, 0x08}, /* 126 '~' */
    {0x00, 0x06, 0x09, 0x09, 0x06}  /* 127 '°' (degree symbol) */
};

/* Internal UI State */
static bool s_dashboard_init_done = false;

/* Helper: Compact float to string conversion (avoids heavy libc printf float) */
static void format_float_1dec(float val, char *out, size_t max_len)
{
    if (val < 0.0f)
    {
        val = 0.0f;
    }
    int int_part = (int)val;
    int frac_part = (int)((val - (float)int_part) * 10.0f + 0.5f);
    if (frac_part >= 10)
    {
        int_part += 1;
        frac_part = 0;
    }
    snprintf(out, max_len, "%2d.%d", int_part, frac_part);
}

/* Static pixel stream buffer for accelerated character rasterization (max 12x16 size 2: 192 pixels = 384 bytes) */
static uint8_t s_char_stream[384];

void UI_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size)
{
    if (x >= ILI9341_WIDTH || y >= ILI9341_HEIGHT)
    {
        return;
    }
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
            /* Spacing column */
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
            /* Two spacing columns */
            s_char_stream[idx++] = b_hi;
            s_char_stream[idx++] = b_lo;
            s_char_stream[idx++] = b_hi;
            s_char_stream[idx++] = b_lo;
        }

        ili9341_draw_buffer(x, y, 12, 16, s_char_stream, 384);
    }
    else
    {
        /* Fallback for generic scales */
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
    uint16_t cur_x = x;
    uint8_t char_step = (size == 1) ? 6 : (6 * size);

    while (*str)
    {
        if (cur_x + char_step > ILI9341_WIDTH)
        {
            break;
        }
        UI_DrawChar(cur_x, y, *str, color, bg, size);
        cur_x += char_step;
        str++;
    }
}

void UI_DrawCard(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t border_color, uint16_t bg_color)
{
    ili9341_fill_rect(x, y, w, h, bg_color);
    ili9341_draw_rect(x, y, w, h, border_color);
}

void UI_DrawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t fg_color, uint16_t bg_color)
{
    if (percent > 100)
    {
        percent = 100;
    }

    /* Outer border frame using fast rectangle outline */
    ili9341_draw_rect(x, y, w, h, UI_COLOR_CARD_BD);

    uint16_t inner_w = (w > 2) ? (w - 2) : 0;
    uint16_t inner_h = (h > 2) ? (h - 2) : 0;
    uint16_t fill_w = (inner_w * percent) / 100;

    /* Dynamic active fill */
    if (fill_w > 0)
    {
        ili9341_fill_rect(x + 1, y + 1, fill_w, inner_h, fg_color);
    }
    /* Remaining inactive bar space */
    if (fill_w < inner_w)
    {
        ili9341_fill_rect(x + 1 + fill_w, y + 1, inner_w - fill_w, inner_h, bg_color);
    }
}

void UI_Init_Dashboard(void)
{
    /* Clear display with deep navy background */
    ili9341_fill_screen(UI_COLOR_BG);

    /* 1. Header Bar (0..28) */
    ili9341_fill_rect(0, 0, ILI9341_WIDTH, 28, ILI9341_NAVY);
    ili9341_fill_rect(0, 28, ILI9341_WIDTH, 2, ILI9341_CYAN);
    UI_DrawString(8, 6, "SMART ROOM", UI_COLOR_TEXT_MAIN, ILI9341_NAVY, 2);

    /* 2. Environment Cards (Temp & Humi side-by-side) */
    /* Left Card: Temperature */
    UI_DrawCard(6, 36, 110, 66, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(14, 42, "TEMP (DHT22)", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    /* Right Card: Humidity */
    UI_DrawCard(124, 36, 110, 66, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(132, 42, "HUMI (DHT22)", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    /* 3. Ambient & Motion Card */
    UI_DrawCard(6, 108, 228, 80, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(14, 114, "AMBIENT & MOTION", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);
    UI_DrawString(14, 126, "LIGHT:", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    /* 4. Relay Actuators Card */
    UI_DrawCard(6, 194, 228, 92, UI_COLOR_CARD_BD, UI_COLOR_CARD_BG);
    UI_DrawString(14, 200, "RELAY ACTUATORS", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);

    /* Channel Labels (Grid 2x2) */
    UI_DrawString(14, 218, "FAN   :", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);
    UI_DrawString(124, 218, "LIGHT1:", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);
    UI_DrawString(14, 252, "DEHUM :", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);
    UI_DrawString(124, 252, "LIGHT2:", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);

    /* 5. Footer Bar */
    ili9341_fill_rect(0, 292, ILI9341_WIDTH, 1, UI_COLOR_CARD_BD);
    ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
    UI_DrawString(12, 301, "SYS: RUNNING | STM32F103", UI_COLOR_TEXT_MUTED, 0x0842, 1);

    s_dashboard_init_done = true;
}

void UI_Force_Redraw(void)
{
    s_dashboard_init_done = false;
}

void UI_Draw_Dashboard(float temp, float humi, uint8_t light_percent, uint8_t pir_motion,
                       uint8_t relay_fan, uint8_t relay_light1, uint8_t relay_light2, uint8_t relay_dehum,
                       uint8_t auto_mode)
{
    /* Ensure static layout is initialized once */
    if (!s_dashboard_init_done)
    {
        UI_Init_Dashboard();
    }

    char str_buf[16];

    /* 1. Header Mode Badge (x=164, y=5, w=68, h=18) */
    if (auto_mode)
    {
        ili9341_fill_rect(164, 5, 68, 18, ILI9341_DARKGREEN);
        UI_DrawString(170, 10, "[ AUTO ]", ILI9341_WHITE, ILI9341_DARKGREEN, 1);
    }
    else
    {
        ili9341_fill_rect(164, 5, 68, 18, ILI9341_ORANGE);
        UI_DrawString(170, 10, "[ MANU ]", ILI9341_BLACK, ILI9341_ORANGE, 1);
    }

    /* 2. Temperature Value (Large Font Scale 2) */
    format_float_1dec(temp, str_buf, sizeof(str_buf));
    strncat(str_buf, " \x7F" "C", sizeof(str_buf) - strlen(str_buf) - 1);
    UI_DrawString(19, 62, str_buf, UI_COLOR_TEMP, UI_COLOR_CARD_BG, 2);

    /* 3. Humidity Value (Large Font Scale 2) */
    format_float_1dec(humi, str_buf, sizeof(str_buf));
    strncat(str_buf, " %", sizeof(str_buf) - strlen(str_buf) - 1);
    UI_DrawString(143, 62, str_buf, UI_COLOR_HUMI, UI_COLOR_CARD_BG, 2);

    /* 4. Light Level Percentage & Progress Bar */
    if (light_percent > 100)
    {
        light_percent = 100;
    }
    snprintf(str_buf, sizeof(str_buf), "%3d %%", light_percent);
    UI_DrawString(58, 126, str_buf, UI_COLOR_LIGHT, UI_COLOR_CARD_BG, 1);
    UI_DrawProgressBar(14, 138, 212, 10, light_percent, UI_COLOR_LIGHT, 0x0821);

    /* 5. Motion PIR Badge (x=14, y=154, w=212, h=24) */
    if (pir_motion)
    {
        ili9341_fill_rect(14, 154, 212, 24, UI_COLOR_ALERT);
        UI_DrawString(46, 162, "* MOTION DETECTED *", ILI9341_WHITE, UI_COLOR_ALERT, 1);
    }
    else
    {
        ili9341_fill_rect(14, 154, 212, 24, UI_COLOR_IDLE);
        UI_DrawString(52, 162, "NO MOTION (IDLE)", ILI9341_GREENYELLOW, UI_COLOR_IDLE, 1);
    }

    /* 6. Relay Actuators (4 Channels) */
    if (auto_mode)
    {
        UI_DrawString(14, 200, "RELAY ACTUATORS [LOCKED] ", UI_COLOR_TEXT_MUTED, UI_COLOR_CARD_BG, 1);
    }
    else
    {
        UI_DrawString(14, 200, "RELAY ACTUATORS [MANUAL] ", UI_COLOR_TEXT_MAIN, UI_COLOR_CARD_BG, 1);
    }

    /* Fan (PB5) */
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

    /* Light 1 (PB6) */
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

    /* Dehumidifier (PB8) */
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

    /* Light 2 (PB7) */
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
    UI_Draw_Dashboard(data->temp, data->humi, data->light_percent, data->pir_motion,
                      data->relay_fan, data->relay_light1, data->relay_light2, data->relay_dehum,
                      data->auto_mode);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize ILI9341 TFT display driver (turns backlight HIGH on PA1) */
  ili9341_init();
  /* Initialize XPT2046 resistive touch controller (SPI2) */
  xpt2046_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  float sim_temp = 28.5f;
  float sim_humi = 65.0f;
  uint8_t sim_light = 85;
  uint8_t sim_pir = 0;
  uint8_t sim_fan = 1;
  uint8_t sim_light1 = 1;
  uint8_t sim_light2 = 0;
  uint8_t sim_dehum = 0;
  uint8_t sim_mode = 1;
  uint32_t last_telemetry_tick = 0;
  uint32_t last_touch_tick = 0;
  uint32_t last_lockout_tick = 0;
  bool touch_was_pressed = false;
  bool lockout_banner_visible = false;

  /* Initial dashboard render */
  UI_Draw_Dashboard(sim_temp, sim_humi, sim_light, sim_pir,
                    sim_fan, sim_light1, sim_light2, sim_dehum,
                    sim_mode);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint16_t touch_x = 0;
    uint16_t touch_y = 0;
    bool need_ui_refresh = false;

    /* 1. Touch Screen Processing (Sampled safely) */
    if (xpt2046_is_touched())
    {
      if (xpt2046_get_xy(&touch_x, &touch_y))
      {
        if (!touch_was_pressed && (HAL_GetTick() - last_touch_tick > 200))
        {
          touch_was_pressed = true;
          last_touch_tick = HAL_GetTick();

          /* Hitbox 1: Header Mode Button (Top-Right: y <= 40, x >= 130) */
          if (touch_y <= 40 && touch_x >= 130)
          {
            sim_mode = !sim_mode;
            need_ui_refresh = true;
          }
          /* Hitbox 2: Motion Alert Card (y: 140..190) */
          else if (touch_y >= 140 && touch_y <= 190)
          {
            sim_pir = !sim_pir;
            need_ui_refresh = true;
          }
          /* Hitbox 3: Relay Control Matrix (y: 190..295) - Partitioned 2x2 Grid */
          else if (touch_y >= 190 && touch_y <= 295)
          {
            if (sim_mode == 1)
            {
              /* AUTO MODE: Relays are locked to automated sensors. Show alert banner */
              lockout_banner_visible = true;
              last_lockout_tick = HAL_GetTick();
              ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
              UI_DrawString(8, 301, "[AUTO] CHUYEN MANU DE CHINH!", ILI9341_ORANGE, 0x0842, 1);
            }
            else
            {
              /* MANUAL MODE: Relay actuation enabled */
              if (touch_x < 120)
              {
                /* Left Column: Upper = FAN, Lower = DEHUM */
                if (touch_y < 240)
                {
                  sim_fan = !sim_fan;
                }
                else
                {
                  sim_dehum = !sim_dehum;
                }
              }
              else
              {
                /* Right Column: Upper = LIGHT 1, Lower = LIGHT 2 */
                if (touch_y < 240)
                {
                  sim_light1 = !sim_light1;
                }
                else
                {
                  sim_light2 = !sim_light2;
                }
              }
              need_ui_refresh = true;

              /* Display live coordinate diagnostic on Footer Bar */
              char footer_dbg[24];
              snprintf(footer_dbg, sizeof(footer_dbg), "TOUCH: (%3d,%3d)", touch_x, touch_y);
              UI_DrawString(12, 301, footer_dbg, UI_COLOR_LIGHT, 0x0842, 1);
            }
          }
        }
      }
    }

    /* Release latch when finger lifts up */
    if (!xpt2046_is_touched())
    {
      touch_was_pressed = false;
    }

    /* 2. Process Physical Hardware Push Button Events (From Non-blocking EXTI Callbacks) */
    if (g_exti_btn_mode_flag)
    {
      g_exti_btn_mode_flag = false;
      sim_mode = !sim_mode;
      need_ui_refresh = true;
    }
    if (g_exti_btn_fan_flag)
    {
      g_exti_btn_fan_flag = false;
      if (sim_mode == 1)
      {
        lockout_banner_visible = true;
        last_lockout_tick = HAL_GetTick();
        ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
        UI_DrawString(8, 301, "[AUTO] CHUYEN MANU DE CHINH!", ILI9341_ORANGE, 0x0842, 1);
      }
      else
      {
        sim_fan = !sim_fan;
        need_ui_refresh = true;
      }
    }
    if (g_exti_btn_light_flag)
    {
      g_exti_btn_light_flag = false;
      if (sim_mode == 1)
      {
        lockout_banner_visible = true;
        last_lockout_tick = HAL_GetTick();
        ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
        UI_DrawString(8, 301, "[AUTO] CHUYEN MANU DE CHINH!", ILI9341_ORANGE, 0x0842, 1);
      }
      else
      {
        sim_light1 = !sim_light1;
        need_ui_refresh = true;
      }
    }
    if (g_exti_btn_dehum_flag)
    {
      g_exti_btn_dehum_flag = false;
      if (sim_mode == 1)
      {
        lockout_banner_visible = true;
        last_lockout_tick = HAL_GetTick();
        ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
        UI_DrawString(8, 301, "[AUTO] CHUYEN MANU DE CHINH!", ILI9341_ORANGE, 0x0842, 1);
      }
      else
      {
        sim_dehum = !sim_dehum;
        need_ui_refresh = true;
      }
    }

    /* 3. Periodic Background Sensor Simulation & Autonomous Logic (every 2.5 seconds) */
    if (HAL_GetTick() - last_telemetry_tick >= 2500)
    {
      last_telemetry_tick = HAL_GetTick();

      sim_temp += 0.2f;
      if (sim_temp > 33.0f) sim_temp = 26.5f;

      sim_humi += 0.8f;
      if (sim_humi > 85.0f) sim_humi = 58.0f;

      sim_light = (sim_light >= 95) ? 35 : (sim_light + 10);

      /* In AUTO Mode: Environmental thresholds autonomously command actuators */
      if (sim_mode == 1)
      {
        sim_fan = (sim_temp >= 28.5f) ? 1 : 0;
        sim_dehum = (sim_humi >= 70.0f) ? 1 : 0;
        sim_light1 = (sim_light < 60 || sim_pir == 1) ? 1 : 0;
      }

      need_ui_refresh = true;
    }

    /* 4. Auto-clear lockout banner after 2.5 seconds */
    if (lockout_banner_visible && (HAL_GetTick() - last_lockout_tick >= 2500))
    {
      lockout_banner_visible = false;
      ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
      UI_DrawString(12, 301, "SYS: RUNNING | STM32F103", UI_COLOR_TEXT_MUTED, 0x0842, 1);
    }

    /* 4. Instant UI Refresh and Hardware Relay/LED Synchronization */
    if (need_ui_refresh)
    {
      /* Drive Physical Relay GPIOs and Feedback LEDs */
      HAL_GPIO_WritePin(RELAY_FAN_GPIO_Port, RELAY_FAN_Pin, sim_fan ? GPIO_PIN_SET : GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED_CH1_GPIO_Port, LED_CH1_Pin, sim_fan ? GPIO_PIN_SET : GPIO_PIN_RESET);

      HAL_GPIO_WritePin(RELAY_LIGHT1_GPIO_Port, RELAY_LIGHT1_Pin, sim_light1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED_CH2_GPIO_Port, LED_CH2_Pin, sim_light1 ? GPIO_PIN_SET : GPIO_PIN_RESET);

      HAL_GPIO_WritePin(RELAY_LIGHT2_GPIO_Port, RELAY_LIGHT2_Pin, sim_light2 ? GPIO_PIN_SET : GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED_CH3_GPIO_Port, LED_CH3_Pin, sim_light2 ? GPIO_PIN_SET : GPIO_PIN_RESET);

      HAL_GPIO_WritePin(RELAY_DEHUM_GPIO_Port, RELAY_DEHUM_Pin, sim_dehum ? GPIO_PIN_SET : GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED_CH4_GPIO_Port, LED_CH4_Pin, sim_dehum ? GPIO_PIN_SET : GPIO_PIN_RESET);

      /* Update HMI Screen */
      UI_Draw_Dashboard(sim_temp, sim_humi, sim_light, sim_pir,
                        sim_fan, sim_light1, sim_light2, sim_dehum,
                        sim_mode);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* Enable AFIO clock and disable JTAG (keep SWD) to free PA15, PB3, PB4 for Buttons */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SYS_HEARTBEAT_GPIO_Port, SYS_HEARTBEAT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_BL_Pin|LCD_RST_Pin|LCD_DC_Pin|LCD_CS_Pin|DHT22_DATA_Pin
                          |LED_CH1_Pin|LED_CH2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_CH3_Pin|LED_CH4_Pin|TOUCH_CS_Pin|RELAY_FAN_Pin
                          |RELAY_LIGHT1_Pin|RELAY_LIGHT2_Pin|RELAY_DEHUM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SYS_HEARTBEAT_Pin */
  GPIO_InitStruct.Pin = SYS_HEARTBEAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SYS_HEARTBEAT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_BL_Pin LCD_RST_Pin LCD_DC_Pin LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_BL_Pin|LCD_RST_Pin|LCD_DC_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_CH3_Pin LED_CH4_Pin RELAY_FAN_Pin RELAY_LIGHT1_Pin
                           RELAY_LIGHT2_Pin RELAY_DEHUM_Pin */
  GPIO_InitStruct.Pin = LED_CH3_Pin|LED_CH4_Pin|RELAY_FAN_Pin|RELAY_LIGHT1_Pin
                          |RELAY_LIGHT2_Pin|RELAY_DEHUM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PIR_INPUT_Pin */
  GPIO_InitStruct.Pin = PIR_INPUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PIR_INPUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TOUCH_IRQ_Pin BTN_LIGHT_Pin BTN_FAN_Pin BTN_DEHUM_Pin */
  GPIO_InitStruct.Pin = TOUCH_IRQ_Pin|BTN_LIGHT_Pin|BTN_FAN_Pin|BTN_DEHUM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TOUCH_CS_Pin */
  GPIO_InitStruct.Pin = TOUCH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TOUCH_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DHT22_DATA_Pin */
  GPIO_InitStruct.Pin = DHT22_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT22_DATA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_CH1_Pin LED_CH2_Pin */
  GPIO_InitStruct.Pin = LED_CH1_Pin|LED_CH2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_MODE_Pin */
  GPIO_InitStruct.Pin = BTN_MODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN_MODE_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint32_t now = HAL_GetTick();

  if (GPIO_Pin == BTN_MODE_Pin)
  {
    static uint32_t s_last_mode_tick = 0;
    if (now - s_last_mode_tick > 200)
    {
      s_last_mode_tick = now;
      g_exti_btn_mode_flag = true;
    }
  }
  else if (GPIO_Pin == BTN_FAN_Pin)
  {
    static uint32_t s_last_fan_tick = 0;
    if (now - s_last_fan_tick > 200)
    {
      s_last_fan_tick = now;
      g_exti_btn_fan_flag = true;
    }
  }
  else if (GPIO_Pin == BTN_LIGHT_Pin)
  {
    static uint32_t s_last_light_tick = 0;
    if (now - s_last_light_tick > 200)
    {
      s_last_light_tick = now;
      g_exti_btn_light_flag = true;
    }
  }
  else if (GPIO_Pin == BTN_DEHUM_Pin)
  {
    static uint32_t s_last_dehum_tick = 0;
    if (now - s_last_dehum_tick > 200)
    {
      s_last_dehum_tick = now;
      g_exti_btn_dehum_flag = true;
    }
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
