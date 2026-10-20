# BÁO CÁO ĐẶC TẢ HỆ THỐNG (SYSTEM SPECIFICATION REPORT)
## DỰ ÁN: HỆ THỐNG ĐIỀU KHIỂN PHÒNG THÔNG MINH (SMART ROOM CONTROL)
**Phân hệ phụ trách:** Giao diện HMI, Màn hình màu TFT LCD, Cảm ứng điện trở, Nút bấm vật lý và Cơ cấu chấp hành Rơ-le  
**Vi điều khiển trung tâm:** STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz)  
**Tiêu chuẩn tài liệu:** Cấu trúc 5 giai đoạn tài liệu đặc tả hệ thống nhúng (*Documents for System Specification* - Bộ môn Kỹ thuật Điện tử, Trường ĐH Bách Khoa TP.HCM):
1. **Product Requirement**
2. **Design Specification**
3. **Hardware Specification**
4. **Software Specification**
5. **Test Specification**

---

# 1. PRODUCT REQUIREMENT (ĐẶC TẢ YÊU CẦU SẢN PHẨM)
*Mô tả sản phẩm sẽ như thế nào từ góc nhìn người dùng và yêu cầu nghiệp vụ (Describe how the product will be).*

### 1.1. Mục đích sản phẩm (Product Purpose)
Phân hệ đóng vai trò là **Bảng điều khiển trung tâm HMI (Human-Machine Interface)** của phòng thông minh, kết nối trực tiếp với người dùng trong phòng. Sản phẩm hiển thị trực quan thông số khí hậu trong phòng (Nhiệt độ, Độ ẩm, Độ sáng, Cảnh báo có người PIR) và cung cấp phương thức điều khiển 4 thiết bị phụ tải (Quạt làm mát, Đèn chiếu sáng 1, Đèn chiếu sáng 2, Máy hút ẩm) qua cả **màn hình cảm ứng đồ họa** và **nút bấm vật lý cơ khí**.

### 1.2. Yêu cầu chức năng (Functional Requirements - FR)
- **FR1 - Giám sát môi trường thời gian thực:** Tiếp nhận số liệu đo đạc định kỳ (float cho Nhiệt độ, Độ ẩm, Độ sáng; bool cho PIR) và hiển thị trực tiếp lên bảng điều khiển với giao diện trực quan, có thanh đo tiến trình (Progress Bar) phần trăm.
- **FR2 - Điều khiển 4 kênh phụ tải độc lập:** Thực thi đóng/ngắt 4 thiết bị phụ tải điện qua các tiếp điểm rơ-le công suất cách ly quang an toàn.
- **FR3 - Chế độ hoạt động kép (Dual-Mode Operation):**
  - *Chế độ Tự động (AUTO):* Hệ thống tự động so sánh số liệu cảm biến với các ngưỡng định trước (Nhiệt độ $\ge 28.5^\circ\text{C}$ bật Quạt; Độ ẩm $\ge 70\%$ bật Hút ẩm; Độ sáng $< 60\%$ hoặc có người chuyển động bật Đèn 1) để tự động kích hoạt thiết bị.
  - *Chế độ Thủ công (MANUAL):* Cho phép người dùng toàn quyền bật/tắt từng thiết bị theo nhu cầu cá nhân.
- **FR4 - Trọng tài ưu tiên phần cứng (Hardware Priority Arbitration):** Nút bấm vật lý luôn có quyền ưu tiên cao hơn màn hình cảm ứng TFT. Khi có thao tác đồng thời (hoặc khi nút cơ đang được nhấn giữ), tín hiệu cảm ứng lập tức bị vô hiệu hóa để tránh xung đột lệnh.
- **FR5 - Khóa an toàn chế độ tự động (AUTO Mode Lockout):** Khi hệ thống đang ở chế độ AUTO, các tác vụ điều khiển rơ-le thủ công (từ cả phím cơ và cảm ứng) bị từ chối thực hiện, giữ nguyên kịch bản điều khiển tự động và phát cảnh báo trên màn hình.

### 1.3. Yêu cầu phi chức năng (Non-Functional Requirements - NFR)
- **NFR1 - Thời gian đáp ứng thao tác:** Độ trễ từ khi nhấn nút cơ hoặc chạm màn hình cảm ứng đến khi rơ-le đóng/ngắt $\le 50\,\text{ms}$.
- **NFR2 - Tốc độ hiển thị không nhấp nháy (Zero-Flicker):** Thời gian làm tươi ô dữ liệu số đồ họa $\le 3\,\text{ms}$; không quét lại toàn màn hình khi cập nhật thông số nhằm triệt tiêu hiện tượng chớp nháy.
- **NFR3 - Thiết kế giao diện công thái học:** Giao diện Dark Mode tông màu tối, tương phản cao, dịu mắt trong phòng tối, màu sắc phân biệt rõ ràng từng loại cảm biến.
- **NFR4 - An toàn điện & Chống sốc:** Cách ly điện hoàn toàn giữa khối điều khiển $3.3\,\text{V}$ và tải điện lưới $220\,\text{VAC}$ qua Optocoupler; tích hợp diode dập xung ngược bảo vệ linh kiện bán dẫn.

---

# 2. DESIGN SPECIFICATION (ĐẶC TẢ THIẾT KẾ HỆ THỐNG & PHÂN RÃ)
*Mô tả các khối phần cứng, phân hệ và các chức năng sẽ được sử dụng (Describe hardware modules, subsystem, and functions will be used).*

### 2.1. Phân rã hệ thống thành các phân hệ chức năng (Subsystem Decomposition)
Phân hệ của Coder 1 được phân rã thành 5 khối chức năng chính:
1. **Phân hệ Hiển thị (Display Subsystem):** Module màn hình LCD TFT 2.8" SPI (IC ILI9341), độ phân giải $240 \times 320$, độ sâu màu 16-bit RGB565.
2. **Phân hệ Nhận diện Chạm (Touch Input Subsystem):** Tấm cảm ứng điện trở 4 dây tích hợp trên màn hình cùng IC giải mã chuyên dụng XPT2046 (ADC 12-bit).
3. **Phân hệ Nút bấm Cơ khí (Physical Button Subsystem):** 4 nút bấm tactile switches (SW2, SW4, SW3, SW5) nối chân GPIO cấu hình ngắt/quét với mạch lọc RC.
4. **Phân hệ Chấp hành Công suất (Actuator Relay Subsystem):** 4 kênh rơ-le 5VDC (SRD-05VDC-SL-C) kích mở qua Opto PC817 và transistor đệm.
5. **Phân hệ Điều phối HMI & Trọng tài Đầu vào (HMI & Arbitration Subsystem):** Khối phần mềm trung gian (Middleware) đảm nhiệm việc giải quyết xung đột đầu vào và xây dựng các thành phần đồ họa (Cards, Progress bars, Diagnostic Banner).

### 2.2. Bảng phân định Phần cứng và Phần mềm (Hardware/Software Partitioning)

| Chức năng hệ thống | Phân bổ Phần cứng (HW Implementation) | Phân bổ Phần mềm (SW Implementation) |
| :--- | :--- | :--- |
| **Giao tiếp & Hiển thị LCD** | Module LCD TFT 2.8", Bus SPI1 vi điều khiển | Driver ILI9341, Burst Buffer 512B, ROI redraw |
| **Đọc tọa độ cảm ứng** | Màng điện trở 4 dây, IC chuyển đổi XPT2046, Bus SPI2 | Driver XPT2046, Lọc trung vị 7 mẫu, định chuẩn pixel |
| **Nhận diện nút bấm cơ** | 4 nút nhấn cơ, mạch lọc phần cứng RC ($\tau = 1\,\text{ms}$) | Driver Button, thuật toán Debounce phần mềm $25\,\text{ms}$ |
| **Đóng ngắt phụ tải điện** | 4 Rơ-le 5VDC, Opto PC817, Diode dập xung 1N4148 | Driver Relay, đóng/ngắt logic GPIO mức cao |
| **Trọng tài ưu tiên đầu vào** | Chân ngắt chạm IRQ (PA8), ngõ vào phím cơ | Middleware Input Manager (Khóa cảm ứng khi bấm nút cơ) |
| **Giao diện người dùng HMI** | Panel tinh thể lỏng 65K màu | Middleware UI Dashboard (Render Dark mode, Fonts) |

### 2.3. Sơ đồ khối kiến trúc hệ thống (System Block Diagram)
```
┌────────────────────────────────────────────────────────────────────────┐
│                        STM32F103C8T6 (72 MHz)                          │
│                                                                        │
│   [APP LAYER] app_loop() Executive Loop (Kịch bản AUTO / MANUAL)       │
│       ▲                                                │               │
│       │ Input Events                                   ▼ Telemetry/Cmd │
│   [MIDDLEWARE LAYER]                                                   │
│     ├── input_mgr (Trọng tài: Nút cơ SW2..5 > Cảm ứng XPT2046)         │
│     └── ui_dashboard (Render thẻ cảm biến, thanh tiến trình, banner)   │
│       ▲                                                │               │
│       │ Trạng thái/Tọa độ                              ▼ Vẽ pixel/Xung │
│   [DEVICE LAYER]                                                       │
│     ├── button_driver   ├── xpt2046_driver   ├── ili9341_driver        │
│     └── relay_driver    └── sensor_interface (Dữ liệu từ Coder 2)      │
└───────┬────────────────────────┬───────────────────────┬───────────────┘
        │ GPIO (PA3,PB2,PA10,11) │ SPI2 (2.25 MHz)       │ SPI1 (18 MHz)
        ▼                        ▼                       ▼
  [4x Nút bấm cơ]         [IC Touch XPT2046]      [TFT LCD ILI9341]
   (SW2,SW4,SW3,SW5)       (Resistive Touch)       (2.8" 240x320 RGB565)
                                 │                       │
        ┌────────────────────────┘                       ▼
        ▼ GPIO (PB4,PA15,PB6,PA12)                [Giao diện HMI]
  [4x Relay Opto-isolated]                         (Dark Mode Dashboard)
   (FAN, LIGHT1, LIGHT2, DEHUM)
```

---

# 3. HARDWARE SPECIFICATION (ĐẶC TẢ CHI TIẾT PHẦN CỨNG)
*Mô tả bo mạch phần cứng được triển khai như thế nào và nguyên lý hoạt động của từng linh kiện (Describe how the board will be implemented and how it works).*

### 3.1. Tính toán & Lựa chọn Cây Xung nhịp (Clock Tree & Bus Specification)
- **Tần số lõi CPU:** Thạch anh ngoài $\text{HSE} = 8.000\,\text{MHz} \xrightarrow{\text{PLL} \times 9} f_{\text{SYSCLK}} = \mathbf{72.000\,\text{MHz}}$.
- **Bus APB2 & Bộ ngoại vi SPI1 (Màn hình ILI9341):**
  - Tần số bus: $PCLK2 = 72.000\,\text{MHz}$.
  - Ràng buộc Datasheet ILI9341: Chu kỳ ghi nối tiếp $T_{\text{SCYCW}} \ge 66\,\text{ns}$ ($f_{\text{max}} = 15.15\,\text{MHz}$).
  - *Lựa chọn bộ chia:* `SPI_BAUDRATEPRESCALER_4` $\Rightarrow f_{\text{SPI1}} = \frac{72\,\text{MHz}}{4} = \mathbf{18.000\,\text{MHz}}$ (Chu kỳ $55.5\,\text{ns}$).
  - *Biện luận:* Tần số $18\,\text{MHz}$ nằm trọn trong dải dung sai thực tế của silicon ILI9341 ở điện áp $3.3\,\text{V}$, tối ưu hóa tốc độ nạp điểm ảnh ($18\,\text{Mbps}$) mà không bị méo tín hiệu.
- **Bus APB1 & Bộ ngoại vi SPI2 (IC Cảm ứng XPT2046):**
  - Tần số bus: $PCLK1 = 36.000\,\text{MHz}$ (Giới hạn trần APB1 của STM32F1).
  - Ràng buộc Datasheet XPT2046: Tần số xung nhịp đồng hồ ADC tối đa $f_{\text{DCLK\_max}} \le \mathbf{2.500\,\text{MHz}}$.
  - *Lựa chọn bộ chia:* `SPI_BAUDRATEPRESCALER_16` $\Rightarrow f_{\text{SPI2}} = \frac{36\,\text{MHz}}{16} = \mathbf{2.250\,\text{MHz}} \le 2.500\,\text{MHz}$.
  - *Biện luận:* Đảm bảo các tụ nội SAR ADC nạp xả chính xác, không gây trôi giá trị đo tọa độ.

### 3.2. Bảng phân bổ chân kết nối phần cứng (Pinout Mapping)

| Phân hệ ngoại vi | Chân Module | Chân STM32 | Cấu hình GPIO HAL | Chức năng phần cứng & Tần số |
| :--- | :--- | :--- | :--- | :--- |
| **TFT ILI9341** | `SCK` | `PA5` | `GPIO_MODE_AF_PP` (High) | SPI1 Clock phát dữ liệu ($18.0\,\text{MHz}$) |
| **TFT ILI9341** | `MOSI` | `PA7` | `GPIO_MODE_AF_PP` (High) | SPI1 Master-Out nạp lệnh và pixel màu |
| **TFT ILI9341** | `CS` | `PB0` | `GPIO_MODE_OUTPUT_PP` (High) | Tín hiệu chọn chip hiển thị (Active-LOW) |
| **TFT ILI9341** | `DC` | `PB1` | `GPIO_MODE_OUTPUT_PP` (High) | Phân biệt Lệnh (`0`) và Dữ liệu (`1`) |
| **TFT ILI9341** | `RST` | `PA4` | `GPIO_MODE_OUTPUT_PP` (Low) | Reset phần cứng màn hình (Xung thấp $\ge 10\,\mu\text{s}$) |
| **TFT ILI9341** | `LED` | `+3.3V` | Cấp nguồn qua trở $10\,\Omega$ | Đèn nền Backlight ($60 - 80\,\text{mA}$) |
| **Touch XPT2046**| `T_CLK` | `PB13`| `GPIO_MODE_AF_PP` (High) | SPI2 Clock chuyển đổi ADC ($2.25\,\text{MHz}$) |
| **Touch XPT2046**| `T_DO` | `PB14`| `GPIO_MODE_INPUT` (Floating) | SPI2 MISO nhận 12-bit dữ liệu ADC |
| **Touch XPT2046**| `T_DIN` | `PB15`| `GPIO_MODE_AF_PP` (High) | SPI2 MOSI gửi byte chọn kênh ($X, Y$) |
| **Touch XPT2046**| `T_CS` | `PB12`| `GPIO_MODE_OUTPUT_PP` (High) | Tín hiệu chọn chip cảm ứng (Active-LOW) |
| **Touch XPT2046**| `T_IRQ` | `PA8` | `GPIO_MODE_INPUT` (Pull-Up) | Ngắt báo chạm phần cứng `PENIRQ` (Active-LOW) |
| **Nút bấm SW2** | `MODE` | `PA3` | `GPIO_MODE_INPUT` (Pull-Up) | Đổi chế độ AUTO/MANUAL (Mạch RC chống rung) |
| **Nút bấm SW4** | `FAN` | `PB2` | `GPIO_MODE_INPUT` (Pull-Up) | Đảo trạng thái Quạt (Mạch RC chống rung) |
| **Nút bấm SW3** | `LIGHT1`| `PA10`| `GPIO_MODE_INPUT` (Pull-Up) | Đảo trạng thái Đèn 1 (Mạch RC chống rung) |
| **Nút bấm SW5** | `DEHUM` | `PA11`| `GPIO_MODE_INPUT` (Pull-Up) | Đảo trạng thái Máy hút ẩm (Mạch RC chống rung) |
| **Relay Kênh 1** | `FAN` | `PB4` | `GPIO_MODE_OUTPUT_PP` (Low) | Kích Opto PC817 Quạt làm mát (Active-HIGH) |
| **Relay Kênh 2** | `LIGHT1`| `PA15`| `GPIO_MODE_OUTPUT_PP` (Low) | Kích Opto PC817 Đèn chiếu sáng 1 (Active-HIGH) |
| **Relay Kênh 3** | `LIGHT2`| `PB6` | `GPIO_MODE_OUTPUT_PP` (Low) | Kích Opto PC817 Đèn chiếu sáng 2 (Active-HIGH) |
| **Relay Kênh 4** | `DEHUM` | `PA12`| `GPIO_MODE_OUTPUT_PP` (Low) | Kích Opto PC817 Máy hút ẩm (Active-HIGH) |

### 3.3. Nguyên lý Mạch phối ghép Phần cứng (Interface Schematic Details)
- **Mạch lọc chống rung 2 lớp nút bấm:**
  Mỗi nút bấm được kéo lên nguồn $3.3\,\text{V}$ qua điện trở kéo $R = 10\,\text{k}\Omega$ và mắc song song với tụ gốm $C = 100\,\text{nF}$ xuống đất. Hằng số thời gian nạp xả $\tau = R \times C = 1\,\text{ms}$ dập tắt hoàn toàn các xung gai cơ khí tần số cao lúc tiếp điểm va đập.
- **Mạch kích công suất và cách ly rơ-le:**
  Các chân GPIO xuất dòng qua trở $1\,\text{k}\Omega$ kích LED phát quang trong Opto PC817. Phía thứ cấp kéo dòng cực B của transistor NPN (S8050) để đóng rơ-le $5\,\text{VDC}$. Diode 1N4148 mắc ngược cực song song với cuộn hút hấp thụ toàn bộ xung ngược Back-EMF khi transistor ngắt dòng.

---

# 4. SOFTWARE SPECIFICATION (ĐẶC TẢ THIẾT KẾ PHẦN MỀM)
*Mô tả phần mềm và firmware được hiện thực như thế nào (Describe how the software will be implemented).*

### 4.1. Kiến trúc Firmware phân tầng & Nguyên tắc Độc lập (Layer Separation)
Firmware được triển khai theo chuẩn Inversion of Control:
- Tầng `app/` điều phối toàn bộ luồng nghiệp vụ, tuyệt đối không gọi các hàm `HAL_GPIO_*` hay `HAL_SPI_*`.
- Tầng `middleware/` cung cấp dịch vụ giao diện (`ui_dashboard`) và trọng tài đầu vào (`input_mgr`).
- Tầng `device/` đóng gói các thao tác phần cứng cấp thanh ghi vào các hàm API có ý nghĩa nghiệp vụ rõ ràng (`relay_set`, `button_was_pressed`, `ili9341_fill_rect`, `xpt2046_get_xy`).

### 4.2. Trình điều khiển Màn hình ILI9341 (Display Driver Specs)
- **Bộ đệm khối Burst Buffer:** Khai báo cố định `static uint8_t s_burst_buffer[512]` ($256\,\text{pixels}$). Bộ đệm này chỉ chiếm $2.5\%$ dung lượng SRAM của STM32F103 ($20\,\text{KB}$), vừa vặn để DMA/SPI truyền dữ liệu liên tục mà không gây nguy cơ tràn ngăn xếp (Stack Overflow).
- **Thuật toán Làm tươi Cục bộ (ROI Partial Refresh):**
  - Thời gian truyền toàn màn hình $240 \times 320$ ($153.6\,\text{KB}$) ở $18\,\text{MHz}$ là $T_{\text{full}} \approx 68.3\,\text{ms}$ ($14.6\,\text{FPS}$).
  - Khi cập nhật số liệu cảm biến, firmware chỉ mở cửa sổ ghi RAM (`0x2A`, `0x2B`, `0x2C`) đúng kích thước ô số liệu ($94 \times 28$ pixels $\approx 5.2\,\text{KB}$).
  - Thời gian truyền vùng ROI: $T_{\text{ROI}} = \frac{5.264 \times 8}{18 \times 10^6} \approx \mathbf{2.34\,\text{ms}}$ ($> 420\,\text{Hz}$), loại bỏ hoàn toàn hiện tượng chớp nháy (Zero-Flicker).
- **Chuỗi lệnh khởi tạo:** Gửi Hardware Reset ($10\,\text{ms}$) $\rightarrow$ Software Reset `0x01` (chờ $120\,\text{ms}$) $\rightarrow$ Cấu hình mạch nguồn `0xCB`, `0xCF`, `0xC0`, `0xC1`, `0xC5`, `0xC7` $\rightarrow$ Cấu hình quét màn hình `0x36 = 0x08` (Portrait, BGR) $\rightarrow$ Cấu hình 16-bit RGB565 `0x3A = 0x55` $\rightarrow$ Thoát Sleep `0x11` (chờ $120\,\text{ms}$) $\rightarrow$ Bật hiển thị `0x29`.

### 4.3. Trình điều khiển Cảm ứng XPT2046 (Touch Driver Specs)
- **Chu kỳ quét:** $50\,\text{Hz}$ ($20\,\text{ms}$/lần kiểm tra trong `app_loop`).
- **Thuật toán Lọc trung vị 7 mẫu (Median Filter):** Thực hiện 7 lần chuyển đổi ADC liên tiếp trên cùng một trục tọa độ, sắp xếp mảng theo thứ tự tăng dần và lấy mẫu trung vị $S[3]$, triệt tiêu hoàn toàn các xung gai nhiễu điện áp ngẫu nhiên.
- **Công thức định chuẩn Pixel ($240 \times 320$):**
  $$X_{\text{screen}} = \text{Clamp}\left(\frac{(X_{\text{raw}} - 200) \times 240}{3800 - 200}, 0, 239\right)$$
  $$Y_{\text{screen}} = \text{Clamp}\left(320 - \frac{(Y_{\text{raw}} - 200) \times 320}{3800 - 200}, 0, 319\right)$$
- **Thời gian khóa chống dội cảm ứng:** Khóa tiếp nhận chạm mới trong vòng $180\,\text{ms}$ giữa 2 lần chạm liên tiếp.

### 4.4. Dịch vụ Trọng tài Đầu vào & Giao diện HMI (Input Arbitration & UI Service)
- **Luật Trọng tài Ưu tiên:** Trong `input_mgr_poll()`, nếu có bất kỳ phím bấm cơ nào đang được đè giữ (`button_is_down == true`) hoặc vừa có thao tác phím cơ trong vòng $400\,\text{ms}$, toàn bộ tác vụ cảm ứng màn hình đều bị hủy bỏ và xuất cảnh báo `[HW PRIORITY] BTN OVERRIDE TOUCH`.
- **Khóa AUTO Mode:** Khi hệ thống ở chế độ `AUTO` (`s_mode == 1`), các tác vụ bật/tắt rơ-le thủ công đều bị chặn và xuất thông báo `[AUTO-LOCK]`.
- **Bảng màu RGB565:** Nền đen sâu `0x0821`, Thẻ card `0x2124`, Tiêu đề `0x18E3`, Nhiệt độ `0xFA40` (Cam), Độ ẩm `0x067F` (Xanh ngọc), Độ sáng `0xFFE0` (Vàng), PIR `0xF800` (Đỏ khi có người) / `0x2945` (Xám khi yên tĩnh).

---

# 5. TEST SPECIFICATION (ĐẶC TẢ KIỂM THỬ HỆ THỐNG)
*Mô tả cách thức hệ thống sẽ được kiểm thử và tiêu chuẩn nghiệm thu (Describe how the system will be tested).*

Hệ thống được kiểm thử nghiệm thu theo 7 ca kiểm thử (Test Cases) chuẩn:

### Bảng Kế hoạch & Tiêu chuẩn Kiểm thử (Test Cases Matrix)

| Test ID | Mục tiêu kiểm thử | Quy trình thực hiện & Thiết bị kiểm tra | Tiêu chuẩn Đạt (Pass Criteria) |
| :---: | :--- | :--- | :--- |
| **TC01** | **Xung nhịp SPI1 & Băng thông ILI9341** | Kẹp que đo máy hiện sóng (Oscilloscope) vào chân PA5 (SCK) và PA7 (MOSI). Cho cập nhật dữ liệu liên tục. | Tần số đo được trên PA5 đạt $18.0\,\text{MHz} \pm 0.5\%$. Dạng sóng vuông rõ ràng. Thời gian cập nhật ô số liệu $\le 3\,\text{ms}$. Không chớp nháy màn hình. |
| **TC02** | **Xung nhịp SPI2 & Độ chính xác Touch** | Kẹp que đo vào chân PB13 (SPI2_SCK). Dùng bút chấm vào 4 góc màn hình và tâm. | Tần số SCK2 đạt $2.25\,\text{MHz} \le 2.5\,\text{MHz}$. Tọa độ điểm chạm hiển thị trên banner sai lệch so với vị trí thực tế $\le \pm 3\,\text{pixels}$. |
| **TC03** | **Khả năng Chống rung Nút bấm cơ** | Kẹp kênh 1 Oscilloscope vào trước mạch RC, kênh 2 vào sau mạch RC chân PA3. Nhấn nhả liên tục 50 lần. | Xung dội cơ học bị san phẳng qua mạch RC. Driver phần mềm (Debounce $25\,\text{ms}$) ghi nhận duy nhất 1 xung trigger trên mỗi lần nhấn. Không bị kích đúp. |
| **TC04** | **Luật Trọng tài Ưu tiên Phần cứng** | Ấn giữ nút cơ SW4 (Bật Quạt), đồng thời tay kia chạm liên tục vào nút ảo trên màn hình cảm ứng để bật đèn. | Màn hình cảm ứng bị vô hiệu hóa hoàn toàn, không có lệnh rơ-le nào từ cảm ứng được kích hoạt. Thanh Banner hiển thị dòng chữ `[HW PRIORITY] BTN OVERRIDE TOUCH`. |
| **TC05** | **Thời gian đóng ngắt Rơ-le & Dập xung** | Kẹp kênh 1 vào chân kích GPIO (PB4), kênh 2 vào tiếp điểm ra của rơ-le. Cho đóng ngắt chu kỳ liên tục. | Thời gian trễ từ khi chân PB4 lên mức 1 đến khi tiếp điểm rơ-le đóng lại $\le 10\,\text{ms}$. Diode 1N4148 dập tắt hoàn toàn xung ngược Back-EMF ($\le 0.7\,\text{V}$). |
| **TC06** | **Khóa an toàn Chế độ AUTO** | Bật hệ thống sang chế độ AUTO (`s_mode = 1`, badge xanh). Thao tác nút cơ hoặc cảm ứng để cố bật/tắt rơ-le. | Trạng thái 4 rơ-le hoàn toàn không thay đổi. Màn hình hiển thị nhãn cảnh báo `[AUTO-LOCK]` màu vàng trên nền đỏ chẩn đoán. |
| **TC07** | **Độ ổn định & Bộ nhớ dài hạn** | Cho hệ thống chạy liên tục 24 giờ trong điều kiện phòng. Giả lập tín hiệu cảm biến biến thiên liên tục. | Hệ thống không bị treo hoặc rơi vào HardFault, dung lượng RAM giữ nguyên $3.01\,\text{KB}$ (không rò rỉ bộ nhớ), màn hình hoạt động ổn định liên tục. |
