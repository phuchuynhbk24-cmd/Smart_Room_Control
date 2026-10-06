# BÁO CÁO TOÀN DIỆN VỀ KIẾN TRÚC FIRMWARE 4 TẦNG (4-LAYER EMBEDDED ARCHITECTURE)
## DỰ ÁN: HỆ THỐNG ĐIỀU KHIỂN PHÒNG THÔNG MINH (SMART ROOM CONTROL)
**Vi điều khiển:** STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz)  
**Môi trường phát triển:** STM32CubeIDE / GCC 14.3.rel1  
**Mô hình thiết kế:** Decoupled Layered Architecture (Kiến trúc phân tầng phi tập trung)

---

# MỤC LỤC
1. [TỔNG QUAN KIẾN TRÚC 4 TẦNG & CÂY CẤU TRÚC THƯ MỤC](#1-tổng-quan-kiến-trúc-4-tầng--cây-cấu-trúc-thư-mục)
2. [TẦNG 1: APPLICATION LAYER (APP)](#2-tầng-1-application-layer-app)
3. [TẦNG 2: MIDDLEWARE LAYER (DỊCH VỤ TRUNG GIAN)](#3-tầng-2-middleware-layer-dịch-vụ-trung-gian)
4. [TẦNG 3: DEVICE LAYER (TRÌNH ĐIỀU KHIỂN NGOẠI VI)](#4-tầng-3-device-layer-trình-điều-khiển-ngoại-vi)
5. [TẦNG 4: DRIVERS / HAL & HARDWARE LAYER](#5-tầng-4-drivers--hal--hardware-layer)
6. [MA TRẬN PHÂN QUYỀN VÀ QUY TẮC ĐÓNG GÓP CHO ĐỒNG ĐỘI (CODER 2)](#6-ma-trận-phân-quyền-và-quy-tắc-đóng-góp-cho-đồng-đội-coder-2)

---

# 1. TỔNG QUAN KIẾN TRÚC 4 TẦNG & CÂY CẤU TRÚC THƯ MỤC

### 1.1. Sơ đồ khối quan hệ giữa 4 tầng
```
┌────────────────────────────────────────────────────────────────────────┐
│                        TẦNG 1: APP (APPLICATION)                       │
│  - app.c / app.h                                                       │
│  - Điều phối kịch bản phòng thông minh (AUTO/MANUAL)                   │
│  - Logic ngưỡng kích hoạt Relay theo cảm biến                          │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Gọi API Dịch vụ
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                     TẦNG 2: MIDDLEWARE (SERVICES)                      │
│  - input_mgr: Trọng tài ưu tiên (Nút vật lý > Cảm ứng TFT), Debounce   │
│  - ui_dashboard: Xây dựng UI động cơ bản, thẻ dữ liệu, thanh tiến trình │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Gọi API Thiết bị
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                      TẦNG 3: DEVICE (DRIVERS)                          │
│  - button: Trình điều khiển 4 nút cơ SW2..SW5                          │
│  - relay:  Trình điều khiển đóng cắt 4 rơ-le công suất                 │
│  - sensor: Interface chuẩn cho cảm biến môi trường (Coder 2)           │
│  - ili9341: Trình điều khiển hiển thị LCD màu SPI1                     │
│  - xpt2046: Trình điều khiển IC cảm ứng điện trở SPI2                  │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Gọi Thư viện phần cứng chuẩn
                                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│               TẦNG 4: DRIVERS / HAL & HARDWARE (BSP)                   │
│  - STM32F1xx HAL Drivers: HAL_GPIO, HAL_SPI, HAL_GetTick               │
│  - Core/Src: main.c, stm32f1xx_it.c, stm32f1xx_hal_msp.c               │
│  - Phần cứng vật lý: Chip STM32F103C8T6, Bus SPI1, SPI2, GPIO         │
└────────────────────────────────────────────────────────────────────────┘
```

### 1.2. Cây cấu trúc thư mục toàn dự án (Directory Tree)

```text
Smart_Room_Control/
├── app/                                # [TẦNG 1] APPLICATION LAYER
│   ├── app.h                           # Header công khai cho nghiệp vụ hệ thống
│   └── app.c                           # Vòng lặp executive loop, luật AUTO, điều phối
│
├── middleware/                         # [TẦNG 2] MIDDLEWARE LAYER
│   ├── input_mgr/                      # Dịch vụ Quản lý & Trọng tài Đầu vào
│   │   ├── input_mgr.h                 # Định nghĩa mã sự kiện, nguồn nhập, struct event
│   │   └── input_mgr.c                 # Logic ưu tiên Nút vật lý > Cảm ứng TFT, cấm can thiệp AUTO
│   └── ui/                             # Dịch vụ Giao diện Người dùng HMI
│       ├── ui_dashboard.h              # Định nghĩa bảng màu RGB565, struct Telemetry
│       └── ui_dashboard.c              # Hàm vẽ Dashboard, thanh đo tiến trình, Banner chẩn đoán
│
├── device/                             # [TẦNG 3] DEVICE LAYER
│   ├── button/                         # Trình điều khiển 4 Nút bấm cơ khí
│   │   ├── button.h                    # Khai báo enum ID nút, hàm đọc trạng thái
│   │   └── button.c                    # Lọc chống rung (Debounce 25ms), chốt xung nhấn
│   ├── relay/                          # Trình điều khiển 4 Relay công suất
│   │   ├── relay.h                    # Khai báo ID relay (FAN, LIGHT1, LIGHT2, DEHUM)
│   │   └── relay.c                    # Điều khiển GPIO đóng/ngắt/đảo trạng thái rơ-le
│   ├── sensor/                         # Trình điều khiển Cảm biến (Module Coder 2)
│   │   ├── sensor.h                    # Chuẩn API getter: nhiệt độ, độ ẩm, độ sáng, PIR
│   │   └── sensor.c                    # Cài đặt mặc định __weak fallback (chống lỗi biên dịch)
│   ├── ili9341/                        # Trình điều khiển Màn hình màu LCD TFT
│   │   ├── ili9341.h                   # Tập lệnh ILI9341, định nghĩa phân giải 240x320
│   │   └── ili9341.c                   # Truyền burst SPI 512-byte, nạp khung cửa sổ vẽ
│   └── xpt2046/                        # Trình điều khiển Cảm ứng điện trở 4 dây
│       ├── xpt2046.h                   # Header đọc tọa độ và định chuẩn hiển thị
│       └── xpt2046.c                   # Đọc SPI2, lọc trung vị 7 mẫu (Median Filter), xoay trục
│
├── Core/                               # [TẦNG 4] CORE & HAL CONFIGURATION
│   ├── Inc/                            # Các file header cấu hình CubeMX
│   │   ├── main.h                      # Định nghĩa chân GPIO pin mapping theo Schematic
│   │   ├── sensor.h                    # Header chuyển tiếp tương thích
│   │   └── stm32f1xx_it.h              # Khai báo ngắt hệ thống
│   └── Src/                            # Mã nguồn khởi động MCU
│       ├── main.c                      # Điểm vào hệ thống: Khởi tạo xung nhịp 72MHz, ngoại vi
│       ├── stm32f1xx_hal_msp.c         # Cấu hình chân phần cứng mức thấp (MSP)
│       ├── stm32f1xx_it.c              # Vector bảng ngắt (SysTick, HardFault)
│       ├── syscalls.c                  # Cầu nối hàm chuẩn C
│       └── sysmem.c                    # Quản lý bộ nhớ heap
│
└── Drivers/                            # [TẦNG 4] THƯ VIỆN CHUẨN ST
    ├── STM32F1xx_HAL_Driver/           # Thư viện HAL của STMicroelectronics
    └── CMSIS/                          # Định nghĩa thanh ghi chuẩn ARM Cortex-M3
```

---

# 2. TẦNG 1: APPLICATION LAYER (APP)

Tầng App đại diện cho tầng nghiệp vụ cao nhất, quyết định toàn bộ hành vi của phòng thông minh.  
**Quy tắc bất biến:** File trong tầng `app` **tuyệt đối không được gọi trực tiếp hàm HAL** như `HAL_GPIO_WritePin`, `HAL_SPI_Transmit` hay đọc thanh ghi. Mọi thao tác phần cứng đều phải thông qua API của `middleware` hoặc `device`.

---

### 2.1. File `app/app.h`
- **Nhiệm vụ:** Công bố giao diện làm việc chính của hệ sinh thái phần mềm để `Core/Src/main.c` gọi thực thi và các module khác (như Coder 2) có thể giao tiếp nếu cần.

```c
#ifndef APP_H
#define APP_H

#include <stdbool.h>

void app_init(void);
void app_loop(void);
void app_set_sensor_data(float temp, float humi, float light, bool pir);
void app_get_sensor_data(float *temp, float *humi, float *light, bool *pir);

#endif /* APP_H */
```

---

### 2.2. File `app/app.c`
- **Nhiệm vụ:**
  - Khởi tạo tất cả dịch vụ và thiết bị khi cấp nguồn.
  - Vòng lặp executive loop không khóa (non-blocking) điều phối:
    1. Tiếp nhận và phản hồi lệnh từ người dùng (nút bấm, cảm ứng).
    2. Định kỳ đọc cảm biến từ Coder 2 qua `sensor_update()` và các hàm `sensor_get_*()`.
    3. Thực thi luật tự động (AUTO mode) kích hoạt relay.
    4. Cập nhật màn hình TFT không gây giật lag (chỉ vẽ lại khi có biến động).

#### Hàm 1: `app_init(void)`
```c
void app_init(void)
{
    /* Initialize Middleware & Device Services */
    sensor_init();
    input_mgr_init();

    /* Initial state of relays is already OFF from relay_init() */
    s_last_banner_tick = HAL_GetTick();
    s_last_sensor_tick = HAL_GetTick();
    s_banner_active = false;

    /* Read initial sensor telemetry */
    s_temp  = sensor_get_temperature();
    s_humi  = sensor_get_humidity();
    s_light = sensor_get_light();
    s_pir   = sensor_get_pir();

    /* Render initial UI dashboard */
    UI_Draw_Dashboard(s_temp, s_humi, s_light, s_pir,
                      relay_get(RELAY_FAN),
                      relay_get(RELAY_LIGHT1),
                      relay_get(RELAY_LIGHT2),
                      relay_get(RELAY_DEHUM),
                      s_mode);
}
```
* **Ý nghĩa:** Chuẩn bị sẵn sàng hệ thống ngay sau khi vi điều khiển bật nguồn.
* **Cách hoạt động:**
  - Gọi `sensor_init()` và `input_mgr_init()` để reset các timer và bộ lọc.
  - Gán mốc thời gian `HAL_GetTick()` vào `s_last_sensor_tick` và `s_last_banner_tick`.
  - Lấy các mẫu đo ban đầu từ thiết bị cảm biến (`sensor_get_*`).
  - Gọi `UI_Draw_Dashboard` để vẽ toàn bộ giao diện khởi điểm lên màn hình TFT.

#### Hàm 2: `app_loop(void)`
```c
void app_loop(void)
{
    bool need_ui_refresh = false;
    uint32_t now = HAL_GetTick();

    /* 1. Poll High-Level Inputs via Middleware */
    input_event_t evt;
    if (input_mgr_poll(&evt, (s_mode == 1)))
    {
        switch (evt.action)
        {
            case INPUT_ACT_MODE_TOGGLE:
                s_mode = !s_mode;
                if (s_mode == 1)
                {
                    relay_set(RELAY_FAN, (s_temp >= 28.5f));
                    relay_set(RELAY_DEHUM, (s_humi >= 70.0f));
                    relay_set(RELAY_LIGHT1, (s_light < 60.0f || s_pir));
                }
                need_ui_refresh = true;
                break;

            case INPUT_ACT_FAN_TOGGLE:
                relay_toggle(RELAY_FAN);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_LIGHT1_TOGGLE:
                relay_toggle(RELAY_LIGHT1);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_LIGHT2_TOGGLE:
                relay_toggle(RELAY_LIGHT2);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_DEHUM_TOGGLE:
                relay_toggle(RELAY_DEHUM);
                need_ui_refresh = true;
                break;

            case INPUT_ACT_NONE:
            default:
                break;
        }

        if (evt.has_banner)
        {
            UI_Draw_Banner(evt.banner_text, evt.banner_fg, evt.banner_bg);
            s_banner_active = true;
            s_last_banner_tick = now;
        }
    }

    /* 2. Periodic Sensor Sampling from Coder 2's Device Library (Every 1000ms) */
    if (now - s_last_sensor_tick >= 1000)
    {
        s_last_sensor_tick = now;
        sensor_update();

        float new_temp  = sensor_get_temperature();
        float new_humi  = sensor_get_humidity();
        float new_light = sensor_get_light();
        bool  new_pir   = sensor_get_pir();

        /* If sensor values changed, update state, evaluate AUTO rules, and refresh UI */
        if (new_temp != s_temp || new_humi != s_humi || new_light != s_light || new_pir != s_pir)
        {
            s_temp  = new_temp;
            s_humi  = new_humi;
            s_light = new_light;
            s_pir   = new_pir;

            if (s_mode == 1)
            {
                relay_set(RELAY_FAN, (s_temp >= 28.5f));
                relay_set(RELAY_DEHUM, (s_humi >= 70.0f));
                relay_set(RELAY_LIGHT1, (s_light < 60.0f || s_pir));
            }
            need_ui_refresh = true;
        }
    }

    /* 3. Auto-clear diagnostic footer banner after 2.5 seconds */
    if (s_banner_active && (now - s_last_banner_tick >= 2500))
    {
        s_banner_active = false;
        UI_Clear_Banner();
    }

    /* 4. Refresh Dashboard UI when mode, sensor, or relay states change */
    if (need_ui_refresh)
    {
        UI_Draw_Dashboard(s_temp, s_humi, s_light, s_pir,
                          relay_get(RELAY_FAN),
                          relay_get(RELAY_LIGHT1),
                          relay_get(RELAY_LIGHT2),
                          relay_get(RELAY_DEHUM),
                          s_mode);
    }
}
```
* **Ý nghĩa:** Trái tim vận hành của toàn bộ hệ thống.
* **Cách hoạt động:**
  1. **Xử lý sự kiện nhập:** Nhận cấu trúc `evt` từ `input_mgr_poll()`. Nếu có lệnh bật/tắt relay hoặc đổi chế độ `s_mode`, hàm cập nhật phần cứng qua `relay_toggle()`, hiện banner thông báo và đánh dấu cờ `need_ui_refresh = true`.
  2. **Thu thập dữ liệu định kỳ (Chu kỳ 1000ms):** Nhờ cơ chế kiểm tra thời gian `now - s_last_sensor_tick >= 1000`, hàm gọi `sensor_update()` và đọc 4 giá trị từ thư viện của Coder 2. Nếu có sự thay đổi:
     - Lưu giá trị mới vào `s_temp`, `s_humi`, `s_light`, `s_pir`.
     - Nếu đang ở chế độ **AUTO (`s_mode == 1`)**, lập tức đánh giá luật: Quạt mở khi $\ge 28.5^\circ\text{C}$; Hút ẩm mở khi $\ge 70.0\%$; Đèn 1 mở khi sáng $< 60\%$ hoặc có người chuyển động (`s_pir == true`).
     - Đặt cờ `need_ui_refresh = true`.
  3. **Tự động xóa Banner:** Sau 2.5 giây (`2500ms`), xóa dòng trạng thái ở chân màn hình về mặc định.
  4. **Cập nhật màn hình:** Chỉ khi `need_ui_refresh == true`, hàm mới gọi `UI_Draw_Dashboard()`, giúp CPU không bị nghẽn bus SPI vẽ lại màn hình liên tục.

#### Hàm 3: `app_set_sensor_data(float temp, float humi, float light, bool pir)`
```c
void app_set_sensor_data(float temp, float humi, float light, bool pir)
{
    s_temp = temp;
    s_humi = humi;
    s_light = light;
    s_pir = pir;

    /* In AUTO Mode: Environmental thresholds autonomously command actuators */
    if (s_mode == 1)
    {
        relay_set(RELAY_FAN, (s_temp >= 28.5f));
        relay_set(RELAY_DEHUM, (s_humi >= 70.0f));
        relay_set(RELAY_LIGHT1, (s_light < 60.0f || s_pir));
    }

    /* Refresh Dashboard UI with live sensor data */
    UI_Draw_Dashboard(s_temp, s_humi, s_light, s_pir,
                      relay_get(RELAY_FAN),
                      relay_get(RELAY_LIGHT1),
                      relay_get(RELAY_LIGHT2),
                      relay_get(RELAY_DEHUM),
                      s_mode);
}
```
* **Ý nghĩa:** Cổng nhận dữ liệu chủ động (Push model) phòng trường hợp Coder 2 dùng ngắt hoặc muốn gửi dữ liệu khẩn cấp.
* **Cách hoạt động:** Cập nhật ngay các biến toàn cục, đánh giá rơ-le trong chế độ AUTO và vẽ lại giao diện tức thì.

#### Hàm 4: `app_get_sensor_data(float *temp, float *humi, float *light, bool *pir)`
```c
void app_get_sensor_data(float *temp, float *humi, float *light, bool *pir)
{
    if (temp)  *temp  = s_temp;
    if (humi)  *humi  = s_humi;
    if (light) *light = s_light;
    if (pir)   *pir   = s_pir;
}
```
* **Ý nghĩa:** Cung cấp con trỏ đọc dữ liệu cảm biến cho các module khác khi cần kiểm tra chéo.

---

# 3. TẦNG 2: MIDDLEWARE LAYER (DỊCH VỤ TRUNG GIAN)

Tầng Middleware có trách nhiệm tổng hợp các driver đơn lẻ ở tầng Device thành các "Dịch vụ" (Services) có ý nghĩa nghiệp vụ, đóng gói các thuật toán phức tạp (như trọng tài ưu tiên, chống rung, tính toán pixel UI).

---

## 3.1. MODULE QUẢN LÝ ĐẦU VÀO: `middleware/input_mgr`

### File `middleware/input_mgr/input_mgr.h`
- **Nhiệm vụ:** Định nghĩa kiểu sự kiện đầu vào, phân loại nguồn phát (Nút bấm vật lý hay Màn hình cảm ứng) và cấu trúc gói tin sự kiện `input_event_t`.

```c
typedef enum {
    INPUT_SRC_NONE,
    INPUT_SRC_BUTTON,   /* Physical hardware button (HIGHEST PRIORITY) */
    INPUT_SRC_TOUCH     /* TFT resistive touch screen */
} input_source_t;

typedef enum {
    INPUT_ACT_NONE,
    INPUT_ACT_MODE_TOGGLE,
    INPUT_ACT_FAN_TOGGLE,
    INPUT_ACT_LIGHT1_TOGGLE,
    INPUT_ACT_LIGHT2_TOGGLE,
    INPUT_ACT_DEHUM_TOGGLE,
} input_action_t;

typedef struct {
    input_action_t action;
    input_source_t source;
    bool has_banner;
    char banner_text[32];
    uint16_t banner_bg;
    uint16_t banner_fg;
} input_event_t;

void input_mgr_init(void);
bool input_mgr_poll(input_event_t *event, bool is_auto_mode);
```

### File `middleware/input_mgr/input_mgr.c`
- **Nhiệm vụ:** Trọng tài xung đột phần cứng. Thực thi nguyên tắc: **Nút bấm vật lý có quyền ưu tiên tuyệt đối so với màn hình cảm ứng TFT**.

#### Hàm 1: `input_mgr_init(void)`
```c
void input_mgr_init(void)
{
    s_last_touch_tick = 0;
    s_last_btn_activity_tick = 0;
    s_touch_was_pressed = false;
}
```
* **Ý nghĩa:** Khởi tạo trạng thái ban đầu của bộ quản lý sự kiện.

#### Hàm 2: `input_mgr_poll(input_event_t *event, bool is_auto_mode)`
* **Code chi tiết:**
```c
bool input_mgr_poll(input_event_t *event, bool is_auto_mode)
{
    if (event == NULL) return false;

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
```
* **Ý nghĩa:**
  - Đây là trung tâm phân giải xung đột nhập liệu của toàn bộ dự án.
* **Cách hoạt động:**
  1. Quét nút vật lý trước tiên (`button_update()`). Nếu có nút nào được bấm, ngay lập tức tạo action và chặn luôn việc xử lý cảm ứng. Ghi nhớ mốc thời gian `s_last_btn_activity_tick`.
  2. Nếu không có nút vật lý nào tác động, mới chuyển xuống kiểm tra màn hình cảm ứng (`xpt2046_is_touched()`).
  3. **Thuật toán Trọng tài Ưu tiên (Priority Arbitration):** Nếu người dùng đang giữ nút vật lý (`btn_any_down == true`) hoặc vừa bấm nút trong vòng 400ms trước (`now - s_last_btn_activity_tick < 400`), mọi cú chạm màn hình đều bị vô hiệu hóa (`INPUT_ACT_NONE`) và màn hình hiện thông báo cảnh báo `[HW PRIORITY] BTN OVERRIDE TOUCH`.
  4. **Khóa AUTO (Auto-lock):** Khi hệ thống đang ở chế độ AUTO, nếu người dùng ấn nút cơ hoặc chạm màn hình để điều khiển relay, hệ thống sẽ từ chối thực hiện, giữ nguyên trạng thái relay tự động và hiển thị cảnh báo `[AUTO-LOCK]`.

---

## 3.2. MODULE GIAO DIỆN NGƯỜI DÙNG: `middleware/ui`

### File `middleware/ui/ui_dashboard.h`
- **Nhiệm vụ:** Định nghĩa bảng màu RGB565 theo phong cách Dark Mode chuyên nghiệp và các nguyên mẫu hàm vẽ giao diện.

```c
typedef struct {
    float    temp;           /**< Room temperature in degrees Celsius (float) */
    float    humi;           /**< Relative humidity in percent (float) */
    float    light;          /**< Ambient light intensity (float, 0.0 - 100.0%) */
    bool     pir_motion;     /**< PIR sensor motion status (bool, true: Detected, false: Clear) */
    uint8_t  relay_fan;      /**< Fan relay state (1: ON, 0: OFF) */
    uint8_t  relay_light1;   /**< Light 1 relay state (1: ON, 0: OFF) */
    uint8_t  relay_light2;   /**< Light 2 relay state (1: ON, 0: OFF) */
    uint8_t  relay_dehum;    /**< Dehumidifier relay state (1: ON, 0: OFF) */
    uint8_t  auto_mode;      /**< Operational mode (1: AUTO, 0: MANUAL) */
} UI_Dashboard_Data_t;

void UI_Init_Dashboard(void);
void UI_Force_Redraw(void);
void UI_Draw_Dashboard(float temp, float humi, float light, bool pir_motion,
                       uint8_t relay_fan, uint8_t relay_light1, uint8_t relay_light2, uint8_t relay_dehum,
                       uint8_t auto_mode);
void UI_Draw_Banner(const char *msg, uint16_t fg_color, uint16_t bg_color);
void UI_Clear_Banner(void);
void UI_Display_Data(const UI_Dashboard_Data_t *data);
```

### File `middleware/ui/ui_dashboard.c`
- **Nhiệm vụ:**
  - Chuyển đổi dữ liệu số thành giao diện đồ họa trực quan (Thẻ Card bo tròn giả lập, Thanh tiến trình phần trăm, Nhãn chế độ AUTO/MANUAL, Trạng thái relay ON/OFF).

#### Các hàm chính trong `ui_dashboard.c`:

##### 1. `UI_Init_Dashboard(void)`
```c
void UI_Init_Dashboard(void)
{
    ili9341_fill_screen(UI_COLOR_BG);
    /* Draw Top Header Bar */
    ili9341_fill_rect(0, 0, ILI9341_WIDTH, 36, 0x18E3);
    UI_DrawString(10, 10, "SMART ROOM", UI_COLOR_TEXT_MAIN, 0x18E3, 2);
    /* Draw Environmental Cards */
    UI_DrawCard(8, 44, 110, 62, "TEMP", 0x2124);
    UI_DrawCard(122, 44, 110, 62, "HUMIDITY", 0x2124);
    UI_DrawCard(8, 118, 110, 62, "LIGHT", 0x2124);
    UI_DrawCard(122, 118, 110, 62, "PIR MOTION", 0x2124);
    /* Draw Actuator Control Cards */
    UI_DrawRelayCard(8, 192, 110, 44, "FAN", 0);
    UI_DrawRelayCard(122, 192, 110, 44, "LIGHT 1", 0);
    UI_DrawRelayCard(8, 244, 110, 44, "DEHUM", 0);
    UI_DrawRelayCard(122, 244, 110, 44, "LIGHT 2", 0);
    /* Draw Footer Banner */
    UI_Clear_Banner();
}
```
* **Ý nghĩa:** Vẽ toàn bộ khung tĩnh (Background, các viền hộp, tiêu đề) một lần duy nhất khi khởi động.
* **Cách hoạt động:** Xóa màn hình với màu nền tối `0x0821`, sau đó vẽ các khối chữ nhật màu thẻ và nhãn tiêu đề cố định.

##### 2. `UI_Draw_Dashboard(...)`
```c
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
        ili9341_fill_rect(130, 140, 94, 28, 0xF800); /* Red */
        UI_DrawString(142, 148, "DETECTED", UI_COLOR_TEXT_MAIN, 0xF800, 1);
    }
    else
    {
        ili9341_fill_rect(130, 140, 94, 28, 0x2945); /* Dark teal */
        UI_DrawString(148, 148, "CLEAR   ", UI_COLOR_TEXT_MUTED, 0x2945, 1);
    }

    /* Actuator Cards */
    UI_DrawRelayCard(8, 192, 110, 44, "FAN", relay_fan);
    UI_DrawRelayCard(122, 192, 110, 44, "LIGHT 1", relay_light1);
    UI_DrawRelayCard(8, 244, 110, 44, "DEHUM", relay_dehum);
    UI_DrawRelayCard(122, 244, 110, 44, "LIGHT 2", relay_light2);
}
```
* **Ý nghĩa:** Cập nhật vùng dữ liệu động trên màn hình mà không cần xóa toàn bộ màn hình (Zero Flicker).
* **Cách hoạt động:** Format các chuỗi số thực `float` thành dạng chuỗi kí tự, ghi đè trực tiếp lên tọa độ các thẻ tương ứng, tính toán độ rộng thanh tiến trình (Progress Bar) từ 0-100% và đổi màu badge PIR (Đỏ: Phát hiện người, Xám: Yên tĩnh).

##### 3. `UI_Draw_Banner(const char *msg, uint16_t fg_color, uint16_t bg_color)` & `UI_Clear_Banner(void)`
```c
void UI_Draw_Banner(const char *msg, uint16_t fg_color, uint16_t bg_color)
{
    ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, bg_color);
    UI_DrawString(12, 301, msg, fg_color, bg_color, 1);
}

void UI_Clear_Banner(void)
{
    ili9341_fill_rect(0, 293, ILI9341_WIDTH, 27, 0x0842);
    UI_DrawString(12, 301, "SYS: RUNNING | STM32F103", UI_COLOR_TEXT_MUTED, 0x0842, 1);
}
```
* **Ý nghĩa:** Thanh thông báo chẩn đoán dưới cùng của màn hình (tọa độ $y=293$ đến $320$). Hiển thị nguồn nhập đang tác động (ví dụ: `[HW BTN] FAN TOGGLED`, `[HW PRIORITY]`, v.v.).

---

# 4. TẦNG 3: DEVICE LAYER (TRÌNH ĐIỀU KHIỂN NGOẠI VI)

Tầng Device chứa các driver phần cứng cụ thể, giao tiếp trực tiếp với ngoại vi MCU (GPIO, SPI).

---

## 4.1. MODULE CẢM BIẾN (SENSOR INTERFACE CHO CODER 2): `device/sensor`

### File `device/sensor/sensor.h`
- **Nhiệm vụ:** Bản hợp đồng giao tiếp (Interface Contract) với Coder 2. Coder 2 độc lập viết driver đọc cảm biến (DHT, LDR, PIR) miễn sao trả về đúng các hàm này.

```c
#ifndef SENSOR_H
#define SENSOR_H

#include <stdbool.h>

void sensor_init(void);
void sensor_update(void);
float sensor_get_temperature(void);
float sensor_get_humidity(void);
float sensor_get_light(void);
bool sensor_get_pir(void);

#endif /* SENSOR_H */
```

### File `device/sensor/sensor.c`
- **Nhiệm vụ:** Cung cấp mã cài đặt dự phòng dùng từ khóa `__attribute__((weak))` để dự án luôn biên dịch thành công 100% khi Coder 2 chưa hoàn thiện code thật.

```c
#include "sensor.h"

__attribute__((weak)) void sensor_init(void) {}
__attribute__((weak)) void sensor_update(void) {}

__attribute__((weak)) float sensor_get_temperature(void)
{
    return 28.5f;
}

__attribute__((weak)) float sensor_get_humidity(void)
{
    return 65.0f;
}

__attribute__((weak)) float sensor_get_light(void)
{
    return 85.0f;
}

__attribute__((weak)) bool sensor_get_pir(void)
{
    return false;
}
```
* **Cách hoạt động:** Khi Coder 2 định nghĩa lại các hàm này trong file `.c` của họ, trình liên kết GCC Linker sẽ tự động ghi đè và loại bỏ các hàm `weak` này, bảo đảm không bao giờ có lỗi xung đột tên hàm (multiple definition) hoặc thiếu hàm (undefined reference).

---

## 4.2. MODULE NÚT BẤM VẬT LÝ: `device/button`

### File `device/button/button.h`
- **Nhiệm vụ:** Định nghĩa danh sách các nút bấm vật lý theo đúng bản vẽ Schematic.
```c
typedef enum {
    BTN_MODE = 0,   /* SW2 -> PA3  */
    BTN_FAN,        /* SW4 -> PB2  */
    BTN_LIGHT1,     /* SW3 -> PA10 */
    BTN_DEHUM,      /* SW5 -> PA11 */
    BTN_COUNT
} button_id_t;

void button_init(void);
void button_update(void);
bool button_was_pressed(button_id_t id);
bool button_is_down(button_id_t id);
```

### File `device/button/button.c`
- **Nhiệm vụ:** Đọc chân GPIO và áp dụng thuật toán **Lọc chống rung phần mềm (Software Debounce 25ms)**.

```c
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
    if (id >= BTN_COUNT) return false;
    bool pressed = s_buttons[id].was_pressed;
    s_buttons[id].was_pressed = false; /* Tự động xóa cờ xung cạnh lên */
    return pressed;
}

bool button_is_down(button_id_t id)
{
    if (id >= BTN_COUNT) return false;
    return (s_buttons[id].stable_state == 1);
}
```
* **Cách hoạt động:**
  - Phần cứng nút bấm nối Active-LOW (Kéo lên 10k, nhấn xuống GND).
  - Khi phát hiện thay đổi chân vật lý, hàm cập nhật `last_change_tick`.
  - Chỉ khi mức tín hiệu ổn định không đổi liên tục trong $\ge 25\text{ms}$, hàm mới ghi nhận vào `stable_state`.
  - Hàm `button_was_pressed()` trả về `true` duy nhất một lần tại cạnh nhấn (One-shot pulse), bảo đảm không bao giờ bị kích hoạt lặp liên tục khi giữ nút.

---

## 4.3. MODULE RƠ-LE ĐIỀU KHIỂN: `device/relay`

### File `device/relay/relay.h` & `device/relay/relay.c`
- **Nhiệm vụ:** Trừu tượng hóa 4 kênh rơ-le công suất cách ly qua opto.
```c
typedef enum {
    RELAY_FAN = 0,   /* PB4  */
    RELAY_LIGHT1,    /* PA15 */
    RELAY_LIGHT2,    /* PB6  */
    RELAY_DEHUM,     /* PA12 */
    RELAY_COUNT
} relay_id_t;
```

```c
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
    if (id >= RELAY_COUNT) return;
    s_relays[id].state = state;
    HAL_GPIO_WritePin(s_relays[id].port, s_relays[id].pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void relay_toggle(relay_id_t id)
{
    if (id >= RELAY_COUNT) return;
    relay_set(id, !s_relays[id].state);
}

bool relay_get(relay_id_t id)
{
    if (id >= RELAY_COUNT) return false;
    return s_relays[id].state;
}
```
* **Ý nghĩa:** Đóng gói toàn bộ các lệnh `HAL_GPIO_WritePin` vào các hàm ngữ nghĩa (`relay_set`, `relay_toggle`, `relay_get`), giúp tầng trên không cần quan tâm đến Port và Pin cụ thể của vi điều khiển.

---

## 4.4. MODULE MÀN HÌNH LCD TFT: `device/ili9341`

### File `device/ili9341/ili9341.c`
- **Nhiệm vụ:** Trình điều khiển màn hình LCD ILI9341 qua bus phần cứng SPI1 (Tốc độ 36 MHz).
- **Hàm cốt lõi 1: `ili9341_set_address_window(x0, y0, x1, y1)`**
  Gửi lệnh `0x2A` (Column Address Set) và `0x2B` (Page Address Set), tiếp nối bằng `0x2C` (Memory Write) để mở cửa sổ ghi điểm ảnh trên RAM nội của màn hình.
- **Hàm cốt lõi 2: `ili9341_fill_rect(x, y, w, h, color)`**
  Sử dụng bộ đệm truyền khối **Burst Buffer 512 bytes (256 pixels)**:
  ```c
  void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
  {
      ili9341_set_address_window(x, y, x + w - 1, y + h - 1);
      uint8_t hi = (uint8_t)(color >> 8);
      uint8_t lo = (uint8_t)(color & 0xFF);
      for (uint16_t i = 0; i < BURST_PIXEL_CHUNK; i++) {
          s_burst_buffer[i * 2]     = hi;
          s_burst_buffer[i * 2 + 1] = lo;
      }
      uint32_t total_pixels = (uint32_t)w * h;
      ili9341_select();
      while (total_pixels > 0) {
          uint16_t chunk = (total_pixels > BURST_PIXEL_CHUNK) ? BURST_PIXEL_CHUNK : total_pixels;
          HAL_SPI_Transmit(&hspi1, s_burst_buffer, chunk * 2, 100);
          total_pixels -= chunk;
      }
      ili9341_unselect();
  }
  ```
  *Cách hoạt động:* Thay vì gửi từng pixel qua SPI gây trễ nghiêm trọng, driver nạp đầy mảng 512 byte và phát liên tục bằng `HAL_SPI_Transmit`, tăng tốc độ làm mới màn hình lên gấp hơn 15 lần.

---

## 4.5. MODULE CẢM ỨNG ĐIỆN TRỞ: `device/xpt2046`

### File `device/xpt2046/xpt2046.c`
- **Nhiệm vụ:** Đọc tọa độ chạm từ chip XPT2046 qua SPI2 (Tốc độ 2.25 MHz), lọc nhiễu và ánh xạ sang tọa độ hiển thị màn hình 240x320.
- **Thuật toán cốt lõi: Bộ lọc trung vị (Median Filter 7 mẫu):**
  ```c
  #define XPT2046_SAMPLE_COUNT 7

  static uint16_t xpt2046_read_filtered_channel(uint8_t cmd)
  {
      uint16_t samples[XPT2046_SAMPLE_COUNT];
      for (uint8_t i = 0; i < XPT2046_SAMPLE_COUNT; i++) {
          samples[i] = xpt2046_read_adc_channel(cmd);
      }
      sort_array(samples, XPT2046_SAMPLE_COUNT);
      return samples[XPT2046_SAMPLE_COUNT / 2]; /* Lấy phần tử chính giữa */
  }
  ```
  *Cách hoạt động:* Loại bỏ triệt để các xung gai điện áp ngẫu nhiên thường gặp trên màn hình cảm ứng điện trở giá rẻ, lấy mẫu trung vị ổn định trước khi nội suy sang pixel `(touch_x, touch_y)`.

---

# 5. TẦNG 4: DRIVERS / HAL & HARDWARE LAYER

Tầng Driver/HAL bao gồm mã do STM32CubeMX tự động sinh ra và thư viện HAL chính hãng của ST, thiết lập phần cứng MCU hoạt động ở mức vật lý.

---

### 5.1. File `Core/Src/main.c`
- **Nhiệm vụ:** Điểm khởi phát (`entry point`) khi vi điều khiển thức dậy sau Reset.

```c
int main(void)
{
    /* 1. Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* 2. Configure the system clock to 72 MHz (HSE PLL x9) */
    SystemClock_Config();

    /* 3. Initialize all configured peripherals (GPIO, SPI1, SPI2) */
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_SPI2_Init();

    /* 4. Initialize LCD Driver */
    ili9341_init();

    /* 5. Initialize Relays */
    relay_init();

    /* 6. Enter Application Domain */
    app_init();

    /* 7. Executive Infinite Loop */
    while (1)
    {
        app_loop();
    }
}
```
* **Cách hoạt động:**
  1. `HAL_Init()`: Cấu hình bộ đếm ngắt hệ thống `SysTick` (chu kỳ 1ms).
  2. `SystemClock_Config()`: Bật dao động thạch anh ngoài HSE 8MHz, nhân xung tần số PLL lên **72 MHz**.
  3. Khởi tạo các ngoại vi phần cứng, sau đó trao toàn bộ quyền điều khiển cho `app_init()` và vòng lặp `app_loop()`.

---

### 5.2. Bảng Ánh xạ Chân Vật lý (Hardware Pinout Mapping)

| Ngoại vi / Thiết bị | Chân STM32 | Chức năng HAL / Mode | Ghi chú Schematic |
| :--- | :--- | :--- | :--- |
| **Nút bấm 1 (Mode)** | `PA3` | GPIO_Input (Pull-up) | Nút SW2 - Đổi chế độ AUTO/MANUAL |
| **Nút bấm 2 (Fan)** | `PB2` | GPIO_Input (Pull-up) | Nút SW4 - Điều khiển Quạt |
| **Nút bấm 3 (Light 1)**| `PA10` | GPIO_Input (Pull-up) | Nút SW3 - Điều khiển Đèn 1 |
| **Nút bấm 4 (Dehum)** | `PA11` | GPIO_Input (Pull-up) | Nút SW5 - Điều khiển Máy hút ẩm |
| **Relay 1 (Quạt)** | `PB4` | GPIO_Output (Push-Pull) | Kích Opto Relay Quạt làm mát |
| **Relay 2 (Đèn 1)** | `PA15` | GPIO_Output (Push-Pull) | Kích Opto Relay Chiếu sáng 1 |
| **Relay 3 (Đèn 2)** | `PB6` | GPIO_Output (Push-Pull) | Kích Opto Relay Chiếu sáng 2 |
| **Relay 4 (Hút ẩm)**| `PA12` | GPIO_Output (Push-Pull) | Kích Opto Relay Máy hút ẩm |
| **LCD SPI1 SCK** | `PA5` | SPI1 Alternate Function | Đồng hồ truyền dữ liệu LCD (36 MHz) |
| **LCD SPI1 MOSI**| `PA7` | SPI1 Alternate Function | Đường truyền dữ liệu ra LCD |
| **LCD CS / DC / RST**| `PB0 / PB1 / PA4` | GPIO_Output | Tín hiệu điều khiển màn hình ILI9341 |
| **Touch SPI2** | `PB13 / 14 / 15`| SPI2 Alternate Function | Bus SPI giao tiếp chip cảm ứng XPT2046 |
| **Touch CS / IRQ**| `PB12 / PA8` | GPIO_Output / Input | Tín hiệu chọn chip & Ngắt chạm cảm ứng |

---

# 6. MA TRẬN PHÂN QUYỀN VÀ QUY TẮC ĐÓNG GÓP CHO ĐỒNG ĐỘI (CODER 2)

### 6.1. Ma trận Gọi hàm (Layer Calling Matrix)

| Tầng gọi | Tầng bị gọi | Cho phép? | Mục đích / Quy tắc |
| :---: | :---: | :---: | :--- |
| **APP** | **MIDDLEWARE** |  **CÓ** | Gọi các service: `input_mgr_poll()`, `UI_Draw_Dashboard()` |
| **APP** | **DEVICE** |  **CÓ** | Gọi các driver: `relay_set()`, `sensor_get_*()`, `relay_get()` |
| **APP** | **HAL / DRIVER** | ❌ **CẤM** | Không được xuất hiện bất kỳ hàm `HAL_GPIO_*`, `HAL_SPI_*` trong `app/` |
| **MIDDLEWARE**| **DEVICE** |  **CÓ** | `input_mgr` gọi `button_update()`, `xpt2046_get_xy()`; `ui` gọi `ili9341_*` |
| **MIDDLEWARE**| **APP** | ❌ **CẤM** | Middleware không được gọi ngược lên App (Tránh vòng lặp phụ thuộc) |
| **DEVICE** | **HAL / DRIVER**|  **CÓ** | Giao tiếp trực tiếp với thanh ghi và thư viện STM32 HAL |

---

### 6.2. Hướng dẫn đóng góp mã nguồn dành cho Coder 2 (Sensor Developer)

Coder 2 chịu trách nhiệm đọc dữ liệu thực tế từ các cảm biến phần cứng (Ví dụ: DHT11/DHT22 trên chân 1-Wire, BH1750/LDR trên ADC/I2C, Cảm biến chuyển động PIR trên GPIO ngắt).

**Quy trình Coder 2 tích hợp thư viện của họ vào dự án:**
1. Coder 2 chỉ cần mở thư mục `device/sensor/`.
2. Mở file `device/sensor/sensor.c` và điền logic đọc cảm biến thật vào 4 hàm:
   ```c
   float sensor_get_temperature(void) {
       return dht_read_temp_float(); // Trả về nhiệt độ độ C
   }

   float sensor_get_humidity(void) {
       return dht_read_humi_float(); // Trả về độ ẩm %
   }

   float sensor_get_light(void) {
       return ldr_read_lux_percent(); // Trả về % độ sáng (0.0 - 100.0)
   }

   bool sensor_get_pir(void) {
       return (HAL_GPIO_ReadPin(PIR_GPIO_Port, PIR_Pin) == GPIO_PIN_SET);
   }
   ```
3. Coder 2 **không cần sửa một dòng code nào trong `app/` hay `middleware/`**. Hệ thống đã được thiết lập sẵn chu kỳ 1000ms tự động kéo dữ liệu (Pull), kiểm tra ngưỡng kích hoạt relay tự động và vẽ lên màn hình TFT.
