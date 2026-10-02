# STM32F103C8T6 Smart Room Control

Hệ thống điều khiển phòng thông minh sử dụng vi điều khiển **STM32F103C8T6** (Cortex-M3).

## 1. Kiến trúc hệ thống
Hệ thống được thiết kế theo kiến trúc 3 tầng:
```
APP
 ↓
MIDDLEWARE
 ↓
DEVICE
 ↓
STM32 HAL
 ↓
HARDWARE
```

## 2. Phần cứng ngoại vi
* **MCU:** STM32F103C8T6 (LQFP48) - 72MHz HSE
* **Hiển thị:** TFT ILI9341 (SPI1 - 18MBits/s)
* **Cảm ứng:** Touch XPT2046 (SPI2 - 2.25MBits/s)
* **Cảm biến:** DHT22 (PA8 FT pin), LDR (ADC1_IN0), PIR (EXTI10)
* **Chấp hành:** 4 Relay (Quạt, Đèn 1, Đèn 2, Hút ẩm) & 4 LED trạng thái
* **Nút bấm:** 4 Buttons vật lý (MODE, LIGHT, FAN, DEHUM)

## 3. Tài liệu kỹ thuật & Báo cáo tiến độ
* **Báo cáo phân bổ chân và cấu hình hệ thống:** [docs/PIN_MAPPING_REPORT.md](docs/PIN_MAPPING_REPORT.md)
* **Báo cáo Tuần 1 (Coder 1 - Driver TFT LCD ILI9341):** [docs/CODER1_WEEK1_ILI9341_REPORT.md](docs/CODER1_WEEK1_ILI9341_REPORT.md)

