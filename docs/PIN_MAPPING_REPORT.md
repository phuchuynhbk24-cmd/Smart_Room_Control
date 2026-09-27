# BÁO CÁO KỸ THUẬT: THIẾT KẾ PHÂN BỔ CHÂN (PIN MAPPING) VÀ CẤU HÌNH HỆ THỐNG
## DỰ ÁN: STM32F103C8T6 SMART ROOM CONTROL

---

* **Đơn vị thực hiện:** Nhóm phát triển Firmware & Hardware Smart Room Control
* **Kỹ sư chịu trách nhiệm:** Coder 1 (HMI & Embedded Architecture)
* **Vi điều khiển mục tiêu:** STM32F103C8T6 (Gói LQFP48, ARM Cortex-M3)
* **Tài liệu cấu hình tham chiếu:** `Smart_Room_Control.ioc`

---

## 1. TỔNG QUAN YÊU CẦU THIẾT KẾ HỆ THỐNG

Hệ thống "Smart Room Control" sử dụng vi điều khiển STM32F103C8T6 làm trung tâm xử lý, tích hợp các khối ngoại vi sau:
1. **Khối giao tiếp HMI:** Màn hình màu TFT ILI9341 (SPI1) và bộ điều khiển cảm ứng điện trở XPT2046 (SPI2).
2. **Khối cảm biến (Sensors):** Cảm biến nhiệt độ/độ ẩm DHT22 (1-Wire), cảm biến ánh sáng LDR (ADC), cảm biến chuyển động PIR (Digital Input/EXTI).
3. **Khối cơ cấu chấp hành (Actuators):** 4 kênh Relay điều khiển Quạt, Đèn 1, Đèn 2, Máy hút ẩm cùng 4 LED hiển thị trạng thái tương ứng.
4. **Khối điều khiển người dùng:** 4 nút nhấn cơ học (MODE, LIGHT, FAN, DEHUM) kích hoạt ngắt ngoài EXTI.
5. **Khối hệ thống & Giám sát:** Cổng nạp/debug chuẩn Serial Wire Debug (SWD), cổng truyền thông nối tiếp USART1 phục vụ in log runtime, và LED chỉ báo hoạt động (Heartbeat).

---

## 2. SECTION 1: CẤU HÌNH XUNG NHỊP HỆ THỐNG (CLOCK TREE) VÀ NẠP/DEBUG (SYS)

### 2.1. Bảng I/O và Chức năng

| Chân MCU | Vị trí LQFP48 | Signal Name | Cấu hình CubeMX | Chức năng kỹ thuật |
| :--- | :---: | :--- | :--- | :--- |
| **PD0-OSC_IN** | 5 | `RCC_OSC_IN` | HSE Oscillator | Ngõ vào thạch anh ngoài 8.0 MHz |
| **PD1-OSC_OUT**| 6 | `RCC_OSC_OUT`| HSE Oscillator | Ngõ ra thạch anh ngoài 8.0 MHz |
| **PA13** | 34 | `SYS_JTMS-SWDIO` | Serial Wire | Chân dữ liệu nạp/debug ST-Link (SWDIO) |
| **PA14** | 37 | `SYS_JTCK-SWCLK` | Serial Wire | Chân xung clock nạp/debug ST-Link (SWCLK) |

### 2.2. Cơ sở Tính toán & Lựa chọn Tần số

1. **Tần số lõi CPU (SYSCLK / HCLK = 72 MHz):**
   * Sử dụng thạch anh ngoài `f_HSE = 8.0 MHz` qua bộ nhân tần số PLL Mul 9:
     ```text
     f_SYSCLK = f_HSE × 9 = 8.0 MHz × 9 = 72.0 MHz
     ```
   * Đây là mức xung nhịp tối đa cho phép của dòng STM32F103, đảm bảo CPU đủ năng lực xử lý giao diện đồ họa ILI9341 mượt mà, tính toán giải thuật điều khiển và đáp ứng ngắt thời gian thực.
2. **Phân phối Bus APB1 và APB2:**
   * **Bus APB2 (PCLK2 = 72 MHz):** Bộ chia `HCLK_DIV1` đưa xung nhịp tối đa `72 MHz` vào các ngoại vi tốc độ cao: SPI1 (TFT), ADC1, USART1, và toàn bộ khối GPIO.
   * **Bus APB1 (PCLK1 = 36 MHz):** Giới hạn phần cứng của APB1 trên STM32F1 là `36 MHz`, do đó bộ chia `HCLK_DIV2` được áp dụng (`72 MHz / 2 = 36 MHz`) để cấp xung cho SPI2 (Touch) và Timer TIM2.
3. **Cấu hình Debug SWD và Tắt JTAG:**
   * Mặc định sau Reset, STM32F1 kích hoạt chế độ nạp JTAG 5 chân (chiếm dụng `PA13, PA14, PA15, PB3, PB4`).
   * Hệ thống cấu hình mục Debug là **Serial Wire** và thực thi lệnh sinh mã `__HAL_AFIO_REMAP_SWJ_NOJTAG()`. Lựa chọn này giữ lại 2 chân `PA13` và `PA14` cho nạp nạp ST-Link, đồng thời giải phóng an toàn 3 chân `PA15, PB3, PB4` để làm các nút bấm vật lý.

---

## 3. SECTION 2: PHÂN HỆ MÀN HÌNH ĐỒ HỌA TFT ILI9341 (SPI1 & CONTROL)

### 3.1. Bảng I/O và Chức năng

| Chân MCU | Vị trí LQFP48 | Signal Name | Cấu hình CubeMX | Chức năng kỹ thuật |
| :--- | :---: | :--- | :--- | :--- |
| **PA1** | 11 | `LCD_BL` | S_TIM2_CH2 (PWM) | Điều khiển độ sáng đèn nền màn hình qua băm xung PWM |
| **PA2** | 12 | `LCD_RST` | GPIO_Output | Reset phần cứng màn hình (Active-Low) |
| **PA3** | 13 | `LCD_DC` | GPIO_Output | Lựa chọn Data (HIGH) / Command (LOW) |
| **PA4** | 14 | `LCD_CS` | GPIO_Output | Chip Select màn hình TFT (Active-Low) |
| **PA5** | 15 | `LCD_SCK` | SPI1_SCK | Xung nhịp truyền thông dữ liệu SPI |
| **PA6** | 16 | `LCD_MISO`| SPI1_MISO | Ngõ nhận dữ liệu từ TFT (Full-Duplex) |
| **PA7** | 17 | `LCD_MOSI`| SPI1_MOSI | Ngõ đẩy dữ liệu đồ họa / pixel sang TFT |

### 3.2. Cơ sở Kỹ thuật & Lựa chọn Tần số

1. **Lựa chọn Bus SPI1 trên APB2:**
   * Màn hình TFT ILI9341 có độ phân giải 240 × 320 pixels, mỗi pixel yêu cầu 16-bit màu (RGB565). Một frame đầy đủ đòi hỏi truyền:
     ```text
     240 × 320 × 2 = 153,600 bytes ≈ 1.23 Mbits/frame
     ```
   * Giao tiếp SPI1 được đặt trên bus APB2 (72 MHz). Bộ chia BaudRate Prescaler được chọn ở mức **DIV4**:
     ```text
     f_SPI1 = f_PCLK2 / 4 = 72.0 MHz / 4 = 18.0 MBits/s
     ```
   * Tần số `18 MHz` nằm trong vùng hoạt động an toàn của ILI9341 (chu kỳ xung `55.5 ns`, đáp ứng tốt chuẩn thời gian ghi dữ liệu của IC điều khiển hiển thị), đảm bảo tốc độ refresh khung hình nhanh, không bị xé hình.
2. **Cấu hình PWM Đèn nền (TIM2_CH2):**
   * Tần số PWM được chọn là `1.0 kHz` nhằm tránh hiện tượng nhấp nháy mắt người (flickering) và không gây rít âm tần từ cuộn cảm.
   * Với Timer Clock `f_TIM = 72 MHz`, thiết lập: Prescaler = 71, Period (ARR) = 999:
     ```text
     f_PWM = 72.0 MHz / ((71 + 1) × (999 + 1)) = 1,000 Hz = 1.0 kHz
     ```
   * Cho phép điều chỉnh độ phân giải sáng 1000 mức (0.1%/bước).
3. **Quy hoạch đường mạch PCB:** Dải chân từ `PA1` đến `PA7` gồm 7 chân liên tiếp trên cùng một cạnh của vi điều khiển, cho phép kéo bus cáp màn hình thẳng hàng mà không phải bắt chéo dây.

---

## 4. SECTION 3: PHÂN HỆ CẢM ỨNG ĐIỆN TRỞ XPT2046 (SPI2 & TOUCH CONTROL)

### 4.1. Bảng I/O và Chức năng

| Chân MCU | Vị trí LQFP48 | Signal Name | Cấu hình CubeMX | Chức năng kỹ thuật |
| :--- | :---: | :--- | :--- | :--- |
| **PB11** | 22 | `TOUCH_IRQ` | GPXTI11 (EXTI) | Ngắt báo chạm màn hình (Active-Low) |
| **PB12** | 25 | `TOUCH_CS` | GPIO_Output | Chip Select cảm ứng XPT2046 (Active-Low) |
| **PB13** | 26 | `TOUCH_SCK` | SPI2_SCK | Xung nhịp truyền thông cảm ứng SPI |
| **PB14** | 27 | `TOUCH_MISO`| SPI2_MISO | Nhận dữ liệu tọa độ 12-bit từ XPT2046 |
| **PB15** | 28 | `TOUCH_MOSI`| SPI2_MOSI | Gửi lệnh lấy mẫu trục X/Y sang XPT2046 |

### 4.2. Cơ sở Kỹ thuật & Lựa chọn Tần số

1. **Lựa chọn Bus SPI2 trên APB1 và Giới hạn Tần số Tối đa:**
   * SPI2 nằm trên bus APB1 (`f_PCLK1 = 36 MHz`).
   * **Nguyên tắc bắt buộc:** Datasheet của IC XPT2046 quy định tần số DCLK tối đa là **2.5 MHz** (chu kỳ xung tối thiểu `400 ns` để bộ ADC 12-bit lấy mẫu ổn định).
   * Do đó, bộ chia BaudRate Prescaler được chọn là **DIV16**:
     ```text
     f_SPI2 = f_PCLK1 / 16 = 36.0 MHz / 16 = 2.25 MBits/s (< 2.5 MHz)
     ```
   * Tần số `2.25 MBits/s` đạt tốc độ đọc tọa độ cao nhất trong phạm vi an toàn, loại bỏ hoàn toàn nguy cơ đọc về giá trị rác (`0x000` hoặc `0xFFF`) do vi phạm thời gian trích xuất mẫu.
2. **Cấu hình Ngắt Chạm (EXTI Line 11):**
   * Chân `PB11` (`TOUCH_IRQ`) được cấu hình ngắt cạnh rơi (**Falling Edge Trigger**) kèm điện trở kéo lên nội (**Pull-Up**).
   * Khi người dùng chạm vào bề mặt màn hình, ngõ ra `PENIRQ` của XPT2046 bị kéo xuống mức 0V, kích hoạt ngắt để MCU đọc tọa độ ngay lập tức mà không cần polling liên tục, tiết kiệm tài nguyên CPU.
3. **Quy hoạch đường mạch PCB:** Cụm chân `PB11 - PB15` gồm 5 chân thẳng hàng nằm liền kề nhau trên cạnh dưới và cạnh phải của chip, hỗ trợ kết nối cáp cảm ứng cực kỳ gọn gàng.

---

## 5. SECTION 4: PHÂN HỆ NÚT NHẤN CƠ HỌC (4 PUSH BUTTONS)

### 5.1. Bảng I/O và Chức năng

| Chân MCU | Vị trí LQFP48 | Signal Name | Cấu hình CubeMX | Chức năng kỹ thuật |
| :--- | :---: | :--- | :--- | :--- |
| **PA15** | 38 | `BTN_MODE` | GPXTI15 (EXTI) | Nút 1: Chuyển chế độ AUTO ↔ MANUAL |
| **PB3** | 39 | `BTN_LIGHT` | GPXTI3 (EXTI) | Nút 2: Bật/Tắt Đèn (Light On/Off) |
| **PB4** | 40 | `BTN_FAN` | GPXTI4 (EXTI) | Nút 3: Bật/Tắt Quạt (Fan On/Off) |
| **PB9** | 46 | `BTN_DEHUM` | GPXTI9 (EXTI) | Nút 4: Bật/Tắt Máy hút ẩm (Dehum On/Off) |

### 5.2. Cơ sở Kỹ thuật & Kiến trúc Ngắt EXTI

1. **Phân bổ Line Ngắt Độc lập 100%:**
   * Trong kiến trúc STM32F1, thanh ghi điều khiển ngắt `AFIO_EXTICR` chia sẻ chung đường ngắt theo số thứ tự của chân (ví dụ: `PAx` và `PBx` sẽ dùng chung `EXTIx` và chỉ được chọn một trong hai).
   * Bốn nút nhấn được thiết kế nằm trên 4 Line ngắt hoàn toàn độc lập:
     * Nút 1: Chân `PA15` → EXTI Line 15 (Vector `EXTI15_10_IRQn`)
     * Nút 2: Chân `PB3`  → EXTI Line 3  (Vector `EXTI3_IRQn`)
     * Nút 3: Chân `PB4`  → EXTI Line 4  (Vector `EXTI4_IRQn`)
     * Nút 4: Chân `PB9`  → EXTI Line 9  (Vector `EXTI9_5_IRQn`)
   * Không có bất kỳ sự trùng lặp số chân nào, đảm bảo cả 4 nút bấm đều bắt được ngắt độc lập tại mọi thời điểm.
2. **Cấu hình Điện môi & Cạnh Ngắt:**
   * Cả 4 chân đều cấu hình **Input with Pull-Up**, ngắt cạnh rơi (**Falling Edge**).
   * Phần cứng chỉ cần nối 1 đầu nút nhấn vào chân GPIO và đầu còn lại nối GND, loại bỏ nhu cầu hàn thêm điện trở kéo ngoài.

---

## 6. SECTION 5: PHÂN HỆ CẢM BIẾN MÔI TRƯỜNG (SENSORS)

### 6.1. Bảng I/O và Chức năng

| Chân MCU | Vị trí LQFP48 | Signal Name | Cấu hình CubeMX | Chức năng kỹ thuật |
| :--- | :---: | :--- | :--- | :--- |
| **PA0-WKUP** | 10 | `LDR_ADC` | ADC1_IN0 | Đo điện áp phân áp từ quang trở LDR (12-bit) |
| **PA8** | 29 | `DHT22_DATA` | GPIO_Output (OD) | Giao tiếp 1-Wire đọc nhiệt độ/độ ẩm từ DHT22 |
| **PB10** | 21 | `PIR_INPUT` | GPXTI10 (EXTI) | Phát hiện chuyển động cơ thể người từ cảm biến PIR |

### 6.2. Cơ sở Kỹ thuật & Bảo vệ Phần cứng

1. **Cảm biến Ánh sáng LDR (ADC1_IN0):**
   * Sử dụng kênh `ADC1_IN0` trên chân `PA0`.
   * **Tần số xung ADC Clock:**
     ```text
     f_ADCCLK = f_PCLK2 / 6 = 72.0 MHz / 6 = 12.0 MHz (< 14.0 MHz)
     ```
     Đạt độ chính xác chuyển đổi cao nhất theo khuyến nghị của nhà sản xuất STM32.
   * **Thời gian lấy mẫu:** `55.5 cycles` giúp tụ lấy mẫu nạp đủ điện tích từ mạch phân áp quang trở có trở kháng tương đối cao, triệt tiêu sai số đọc.
2. **Cảm biến DHT22 (PA8 - 5V-Tolerant Protection):**
   * Chân `PA8` được cấu hình **GPIO_Output Open-Drain** kèm **Pull-Up**.
   * **Bảo vệ phần cứng:** Chân `PA8` là chân chịu được mức điện áp 5V (**FT - Five-volt Tolerant**). Do cảm biến DHT22 hoạt động chính xác nhất khi cấp nguồn 5V và có điện trở kéo lên 5V, việc sử dụng chân FT bảo vệ cổng GPIO của STM32 không bị hỏng do quá áp (> 3.6V).
3. **Cảm biến Chuyển động PIR (PB10):**
   * Cấu hình **GPIO_EXTI10**, ngắt cạnh lên (**Rising Edge Trigger**), điện trở nội **Pull-Down**.
   * Khi cảm biến phát hiện chuyển động, ngõ ra kích mức logic HIGH (3.3V), tạo sườn lên đánh thức tác vụ bật đèn trong thuật toán điều khiển tự động.

---

## 7. SECTION 6: PHÂN HỆ CƠ CẤU CHẤP HÀNH & CHỈ BÁO (RELAYS & LEDS)

### 7.1. Bảng I/O và Chức năng

| Chân MCU | Vị trí LQFP48 | Signal Name | Cấu hình CubeMX | Chức năng kỹ thuật |
| :--- | :---: | :--- | :--- | :--- |
| **PB5** | 41 | `RELAY_FAN` | GPIO_Output | Kích đóng/ngắt Relay Quạt (Kênh 1) |
| **PB6** | 42 | `RELAY_LIGHT1`| GPIO_Output | Kích đóng/ngắt Relay Đèn 1 (Kênh 2) |
| **PB7** | 43 | `RELAY_LIGHT2`| GPIO_Output | Kích đóng/ngắt Relay Đèn 2 (Kênh 3) |
| **PB8** | 44 | `RELAY_DEHUM` | GPIO_Output | Kích đóng/ngắt Relay Máy hút ẩm (Kênh 4) |
| **PA11**| 32 | `LED_CH1` | GPIO_Output | Đèn LED hiển thị trạng thái Kênh Quạt |
| **PA12**| 33 | `LED_CH2` | GPIO_Output | Đèn LED hiển thị trạng thái Kênh Đèn 1 |
| **PB0** | 18 | `LED_CH3` | GPIO_Output | Đèn LED hiển thị trạng thái Kênh Đèn 2 |
| **PB1** | 19 | `LED_CH4` | GPIO_Output | Đèn LED hiển thị trạng thái Kênh Hút ẩm |
| **PC13**| 2 | `SYS_HEARTBEAT`| GPIO_Output (OD) | LED chẩn đoán nhịp tim vi điều khiển (Alive Indicator) |

### 7.2. Cơ sở Kỹ thuật & Layout Mạch

1. **Cụm 4 Relay liên tiếp trên Port B (`PB5, PB6, PB7, PB8`):**
   * Bốn chân output relay nằm liền một khối trên cạnh trên của vi điều khiển, giúp Member 3 kéo đường mạch bus song song nối thẳng vào module 4 Relay (hoặc IC đệm cách ly quang Optocoupler) mà không cắt ngang các đường tín hiệu analog hay SPI.
2. **Cụm LED trạng thái:** Được điều khiển đồng bộ thông qua tầng `output_manager` của Coder 2 (khi Relay đóng thì LED kênh sáng, Relay ngắt thì LED kênh tắt).
3. **Chân chẩn đoán `PC13` trên IC rời:**
   * Chân `PC13` thuộc khối nguồn Backup Domain, có dòng tải giới hạn `3 mA`.
   * Chân này cấu hình **Open-Drain, Low Speed** để điều khiển một LED báo nhịp tim hệ thống (nhấp nháy chu kỳ `500 ms`). Nếu thiết kế LED ngoài trên PCB, điện trở hạn dòng bắt buộc phải ≥ 1 kΩ để bảo vệ công tắc nguồn nội của chip.

---

## 8. SECTION 7: PHÂN HỆ TRUYỀN THÔNG DEBUG (USART1)

### 8.1. Bảng I/O và Chức năng

| Chân MCU | Vị trí LQFP48 | Signal Name | Cấu hình CubeMX | Chức năng kỹ thuật |
| :--- | :---: | :--- | :--- | :--- |
| **PA9** | 30 | `DEBUG_TX` | USART1_TX | Truyền dữ liệu log UART printf lên máy tính |
| **PA10**| 31 | `DEBUG_RX` | USART1_RX | Nhận lệnh điều khiển / cấu hình CLI từ máy tính |

### 8.2. Cơ sở Kỹ thuật & Thông số Truyền thông

* **Tốc độ truyền (Baudrate):** `115200 bps`.
* **Khung truyền:** 8 Data bits, No parity, 1 Stop bit (8N1).
* **Ứng dụng thực tế:** Phục vụ override hàm `__io_putchar` để sử dụng lệnh `printf()` chuẩn trong C. Toàn bộ thông số cảm biến (`temperature`, `humidity`, `light`, `motion`) và trạng thái chuyển mode (`AUTO/MANUAL`) được đẩy liên tục lên Serial Monitor của máy tính, hỗ trợ tối đa cho việc kiểm thử thuật toán của Coder 2 và kiểm tra giao diện HMI của Coder 1.

---

## 9. SECTION 8: KIỂM TRA ĐỐI CHIẾU XÁC THỰC VỚI FILE `.IOC`

Để đảm bảo toàn bộ thiết kế trên được thực thi chính xác và không có bất kỳ sai lệch nào, cấu hình đã được đối soát trực tiếp với file `Smart_Room_Control.ioc`:

| Hạng mục kiểm tra | Thông số thiết kế | Trạng thái trong file `.ioc` | Kết quả đối chứng |
| :--- | :--- | :--- | :---: |
| **Xung nhịp CPU (SYSCLK)** | 72.0 MHz (HSE 8MHz × PLL 9) | `RCC.SYSCLKFreq_VALUE=72000000` | **ĐẠT** |
| **Xung nhịp Bus APB1 / APB2** | APB1 = 36 MHz, APB2 = 72 MHz | `RCC.APB1Freq_Value=36000000`, `APB2=72000000` | **ĐẠT** |
| **Chuẩn nạp Debug** | Serial Wire (Tắt JTAG) | `SYS.Debug=Serial Wire`, `PA13/PA14=Serial_Wire` | **ĐẠT** |
| **Tốc độ SPI1 (ILI9341)** | Prescaler DIV4 (18 MBits/s) | `SPI1.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_4` | **ĐẠT** |
| **Tốc độ SPI2 (XPT2046)** | Prescaler DIV16 (2.25 MBits/s) | `SPI2.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16`| **ĐẠT** |
| **PWM Đèn nền (PA1)** | TIM2 CH2, Tần số 1 kHz | `SH.S_TIM2_CH2.0=TIM2_CH2,PWM Generation2 CH2` | **ĐẠT** |
| **ADC1 Cảm biến LDR (PA0)** | ADC1_IN0, lấy mẫu 55.5 cycles | `ADC1.SamplingTime...=ADC_SAMPLETIME_55CYCLES_5` | **ĐẠT** |
| **1-Wire DHT22 (PA8)** | Output Open-Drain, 5V-Tolerant | `PA8.GPIO_ModeDefaultOD=GPIO_MODE_OUTPUT_OD` | **ĐẠT** |
| **Ngắt Nút 1 (PA15)** | Line 15, Falling Edge, Pull-Up | `PA15.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING` | **ĐẠT** |
| **Ngắt Nút 2 (PB3)** | Line 3, Falling Edge, Pull-Up | `PB3.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING` | **ĐẠT** |
| **Ngắt Nút 3 (PB4)** | Line 4, Falling Edge, Pull-Up | `PB4.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING` | **ĐẠT** |
| **Ngắt Nút 4 (PB9)** | Line 9, Falling Edge, Pull-Up | `PB9.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING` | **ĐẠT** |
| **Ngắt Cảm ứng (PB11)** | Line 11, Falling Edge, Pull-Up | `PB11.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING` | **ĐẠT** |
| **Ngắt PIR (PB10)** | Line 10, Rising Edge, Pull-Down | `PB10.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_RISING` | **ĐẠT** |
| **Trạng thái chân CubeMX** | 100% chân gán màu xanh lá (Green) | `Mcu.PinsNb=35`, Signal Handlers đầy đủ | **ĐẠT (100% Khớp)** |

### Kết luận:
Toàn bộ 35 chân chức năng trên vi điều khiển STM32F103C8T6 đã được quy hoạch tối ưu, phân chia module rõ ràng, tính toán tần số đáp ứng chuẩn kỹ thuật và đã được cấu hình hoàn hảo trong file `Smart_Room_Control.ioc`.
