# BÁO CÁO PHÂN TÍCH KỸ THUẬT CHI TIẾT: DRIVER TFT LCD ILI9341
**Phân hệ:** Device Driver (TFT LCD ILI9341 2.8 inch)  
**Tác giả:** Coder 1  
**Mục tiêu:** Cung cấp tài liệu kỹ thuật toàn diện về nguyên lý phần cứng, phân tích chi tiết quy trình cấu hình module màn hình ILI9341 2.8 inch (hàm `ili9341_init`) và giải thích từng hàm chức năng trong driver.

---

## 1. TỔNG QUAN PHẦN CỨNG & GIAO THỨC TRUYỀN THÔNG

Màn hình TFT LCD 2.8 inch sử dụng IC điều khiển **ILI9341** với độ phân giải vật lý $240 \times 320$ pixels, hỗ trợ 65.536 màu (chuẩn 16-bit RGB565).

### 1.1. Sơ đồ kết nối chân vi điều khiển STM32F103C8T6

Module giao tiếp qua chuẩn SPI 4 dây kết hợp các chân điều khiển GPIO:

| Chân LCD | Chân MCU | Tên định danh (`main.h`) | Kiểu chân | Chức năng chi tiết |
| :--- | :--- | :--- | :--- | :--- |
| **SCK** | **PA5** | SPI1_SCK | SPI1 Alternate Function | Xung nhịp đồng hồ SPI (Baudrate = **18.0 MBits/s**) |
| **MOSI** | **PA7** | SPI1_MOSI | SPI1 Alternate Function | Master Output / Slave Input (Đường truyền lệnh và pixel) |
| **MISO** | **PA6** | SPI1_MISO | SPI1 Alternate Function | Master Input (Không dùng cho màn hình, nối chuẩn bus) |
| **CS** | **PA4** | `LCD_CS_Pin` | GPIO Output Push-Pull | Chip Select (Tích cực mức THẤP - Active-Low) |
| **DC** | **PA3** | `LCD_DC_Pin` | GPIO Output Push-Pull | Data / Command: **0 = Command**, **1 = Data** |
| **RST** | **PA2** | `LCD_RST_Pin` | GPIO Output Push-Pull | Hardware Reset (Tích cực mức THẤP - Active-Low) |
| **BL / LED**| **PA1** | `LCD_BL_Pin` | GPIO Output Push-Pull | Đèn nền màn hình (Backlight: **1 = Bật sáng**) |

### 1.2. Nguyên lý hoạt động của chân CS và DC
- **Chân CS (Chip Select):** IC ILI9341 chỉ lắng nghe dữ liệu trên đường bus SPI khi chân CS ở mức **LOW (0)**. Khi CS ở mức **HIGH (1)**, IC ngắt kết nối với bus để giải phóng đường truyền.
- **Chân DC (Data / Command):** ILI9341 không có đường dây riêng biệt giữa thanh ghi điều khiển và bộ nhớ ảnh. Trạng thái của chân DC tại thời điểm truyền xung clock sẽ quyết định:
  - Khi $\text{DC} = 0$: Byte truyền đến được ghi vào **Thanh ghi lệnh (Command Register)** để điều khiển cấu hình (ví dụ: xoay hướng, chọn vùng tọa độ).
  - Khi $\text{DC} = 1$: Byte truyền đến được chuyển trực tiếp vào **Bộ nhớ đệm tham số hoặc Bộ nhớ RAM hiển thị (GRAM)** để tạo màu cho điểm ảnh.

### 1.3. Định dạng màu sắc 16-bit RGB565
Mỗi điểm ảnh (pixel) được mã hóa bằng một số nguyên không dấu 16-bit (`uint16_t`):
- **5 bit Red (R):** Bit [15:11] (32 mức độ đỏ)
- **6 bit Green (G):** Bit [10:5] (64 mức độ xanh lá - mắt người nhạy nhất)
- **5 bit Blue (B):** Bit [4:0] (32 mức độ xanh dương)

```text
 Bit:   15 14 13 12 11 | 10  9  8  7  6  5 | 4  3  2  1  0
 Thành phần:    RED (5-bit)   |    GREEN (6-bit)   |   BLUE (5-bit)
```

**Thứ tự truyền dữ liệu qua SPI (Big-Endian):**  
Giao tiếp SPI của STM32F1 truyền theo từng byte (8-bit). Vì vậy, mỗi mã màu 16-bit sẽ được tách làm 2 bytes:
1. **Byte cao (MSB):** `(uint8_t)(color >> 8)` $\rightarrow$ Chứa 5 bit Red và 3 bit cao của Green.
2. **Byte thấp (LSB):** `(uint8_t)(color & 0xFF)` $\rightarrow$ Chứa 3 bit thấp của Green và 5 bit Blue.

---

## 2. PHÂN TÍCH CHI TIẾT FILE HEADER (`ili9341.h`)

File `ili9341.h` đóng vai trò là giao diện lập trình ứng dụng (API Interface) sạch, chỉ công khai những gì tầng trên cần dùng:

### 2.1. Cấu trúc bảo vệ & Kích thước hiển thị
```c
#ifndef ILI9341_H
#define ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define ILI9341_WIDTH           240
#define ILI9341_HEIGHT          320
```
- `#ifndef ILI9341_H`: Tránh nạp trùng lặp file header khi được include ở nhiều nơi.
- `extern "C"`: Đảm bảo tương thích nếu dự án tích hợp mã nguồn C++.
- `#include "main.h"`: Lấy định nghĩa các chân phần cứng (`LCD_CS_Pin`, `LCD_DC_Pin`, v.v.) sinh ra từ STM32CubeMX.
- `ILI9341_WIDTH (240)` và `ILI9341_HEIGHT (320)`: Hằng số kích thước hiển thị vật lý của panel ở hướng đứng mặc định.

### 2.2. Bảng mã màu 16-bit tiền định nghĩa
Định nghĩa sẵn mã hex RGB565 cho 19 màu tiêu chuẩn (`ILI9341_BLACK`, `ILI9341_WHITE`, `ILI9341_RED`, `ILI9341_GREEN`, `ILI9341_BLUE`, v.v.) giúp lập trình viên chỉ cần gọi tên màu mà không phải tự tính toán giá trị bitwise.

### 2.3. Khai báo nguyên mẫu 6 hàm cốt lõi
```c
void ili9341_init(void);
void ili9341_write_command(uint8_t cmd);
void ili9341_write_data(uint8_t *p_data, uint16_t size);
void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ili9341_fill_screen(uint16_t color);
```

---

## 3. PHÂN TÍCH CHUYÊN SÂU CÁCH CẤU HÌNH MODULE ILI9341 (HÀM `ili9341_init`)

Màn hình TFT LCD không thể hoạt động chỉ bằng việc cấp nguồn 3.3V. Bên trong panel là hàng triệu phân tử tinh thể lỏng được điều khiển bởi các transistor màng mỏng (TFT). 

Để điều khiển các phân tử này xoay chính xác nhằm lọc các dải màu, IC ILI9341 phải tích hợp các mạch **Bơm áp (Charge Pump)** để tạo ra các mức điện áp chuyên dụng:
- Điện áp dương rất cao **$V_{GH} \approx +12\text{V}$**: Kích mở các cổng hàng transistor của từng điểm ảnh.
- Điện áp âm **$V_{GL} \approx -10\text{V}$**: Khóa dứt khoát các hàng transistor khi không quét đến.
- Điện áp tấm cực chung **$V_{COM}$**: Đảo chiều liên tục từ $-1.5\text{V} \rightarrow +4.25\text{V}$ sau mỗi khung hình để chống phân cực một chiều vĩnh viễn (chống cháy màn hình).

Hàm `ili9341_init(void)` thực hiện toàn bộ trình tự cấu hình thanh ghi phần cứng (Golden Initialization Sequence) chia thành **6 bước kỹ thuật chi tiết**:

---

### Bước 1: Khởi động Đèn nền & Trình tự Reset (Hardware / Software Reset)

```c
/* 1. Bật đèn nền LED panel */
HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);

/* 2. Đưa chân Chip Select về mức cao (De-assert) trước khi reset */
ili9341_unselect();

/* 3. Chuỗi Reset phần cứng */
ili9341_hardware_reset();

/* 4. Reset mềm và chờ ổn định */
ili9341_write_command(ILI9341_CMD_SWRESET);
HAL_Delay(150);
```

- **`LCD_BL` (PA1):** Kéo lên mức HIGH để cấp nguồn cho dải LED chiếu sáng nền panel. Nếu không có dòng này, màn hình vẫn nhận lệnh nhưng mắt người sẽ thấy màn hình tối đen.
- **`ili9341_hardware_reset()`:** Kéo chân `RST` (PA2) xuống mức LOW trong 10ms rồi đưa lên HIGH (chờ 50ms). Theo Datasheet ILI9341 (§12.2), độ rộng xung reset tối thiểu là $10\,\mu\text{s}$. Việc giữ 10ms đảm bảo tụ lọc xả sạch điện tích và IC khởi động lại tin cậy 100%.
- **`SWRESET` (Mã lệnh `0x01`):** Lệnh Reset phần mềm, ép toàn bộ thanh ghi nội về giá trị mặc định. Bắt buộc trễ **150ms** để mạch dao động nội (Internal RC Oscillator) và mạch ổn áp nội $1.5\text{V}$ của IC ổn định hoàn toàn.

---

### Bước 2: Cấu hình Khối nguồn & Mạch dao động (Power & Timing Control)

Đây là khối thanh ghi quyết định trực tiếp đến độ ổn định phần cứng của panel 2.8 inch:

```c
/* Power Control A (0xCB) */
uint8_t pwctra[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
ili9341_write_command(ILI9341_CMD_PWCTRA);
ili9341_write_data(pwctra, sizeof(pwctra));

/* Power Control B (0xCF) */
uint8_t pwctrb[] = { 0x00, 0xC1, 0x30 };
ili9341_write_command(ILI9341_CMD_PWCTRB);
ili9341_write_data(pwctrb, sizeof(pwctrb));

/* Driver Timing Control A (0xE8) & B (0xEA) */
uint8_t dtca[] = { 0x85, 0x00, 0x78 };
ili9341_write_command(ILI9341_CMD_DTCA);
ili9341_write_data(dtca, sizeof(dtca));

uint8_t dtcb[] = { 0x00, 0x00 };
ili9341_write_command(ILI9341_CMD_DTCB);
ili9341_write_data(dtcb, sizeof(dtcb));

/* Power on Sequence Control (0xED) */
uint8_t posc[] = { 0x64, 0x03, 0x12, 0x81 };
ili9341_write_command(ILI9341_CMD_POSC);
ili9341_write_data(posc, sizeof(posc));

/* Pump Ratio Control (0xF7) */
uint8_t pumpratio[] = { 0x20 };
ili9341_write_command(ILI9341_CMD_PUMPRATIO);
ili9341_write_data(pumpratio, sizeof(pumpratio));
```

#### Phân tích chuyên sâu từng thanh ghi:
1. **`PWCTRA` (`0xCB`):** Nạp `{ 0x39, 0x2C, 0x00, 0x34, 0x02 }`. Cấu hình mạch nguồn $V_{CORE} = 1.5\text{V}$ cho nhân xử lý của IC. Tham số `0x34, 0x02` kích hoạt mạch bảo vệ chống sụt áp nguồn nội (Under-Voltage Lockout).
2. **`PWCTRB` (`0xCF`):** Nạp `{ 0x00, 0xC1, 0x30 }`. Giá trị `0xC1` bật tính năng **bảo vệ chống tĩnh điện (ESD Protection)** và chống xả ngược cho các tụ bơm áp bên trong IC.
3. **`DTCA` (`0xE8`):** Nạp `{ 0x85, 0x00, 0x78 }`. Kiểm soát **thời gian chống trùng dẫn (Non-overlap time)** giữa các hàng Gate Driver. Nếu không có thời gian này, các transistor hàng liền kề có thể dẫn cùng lúc, tạo ra hiện tượng **bóng ma (crosstalk / ghosting)** và vệt mờ sọc ngang trên màn hình.
4. **`POSC` (`0xED`):** Nạp `{ 0x64, 0x03, 0x12, 0x81 }`. Định nghĩa **trình tự cấp nguồn tăng dần từng bước** khi IC thức dậy ($V_{DD} \rightarrow V_{GH} \rightarrow V_{GL} \rightarrow V_{COM}$). Điều này triệt tiêu hoàn toàn hiện tượng **đột biến dòng điện (Inrush Current)** làm sụt nguồn 3.3V của cả bo STM32.
5. **`PUMPRATIO` (`0xF7`):** Nạp `{ 0x20 }`. Thiết lập **tỷ số nhân áp $4\times$** cho bộ bơm áp nội, nhân điện áp từ 3.3V lên mức xấp xỉ $\approx 12\text{V}$ cấp cho mạch kích cổng $V_{GH}$.

---

### Bước 3: Cấu hình Điện áp tham chiếu & Triệt tiêu nhấp nháy ($V_{COM}$)

```c
/* Power Control 1 (0xC0) */
uint8_t pwctr1[] = { 0x23 };
ili9341_write_command(ILI9341_CMD_PWCTR1);
ili9341_write_data(pwctr1, sizeof(pwctr1));

/* Power Control 2 (0xC1) */
uint8_t pwctr2[] = { 0x10 };
ili9341_write_command(ILI9341_CMD_PWCTR2);
ili9341_write_data(pwctr2, sizeof(pwctr2));

/* VCOM Control 1 (0xC5) & VCOM Control 2 (0xC7) */
uint8_t vmctr1[] = { 0x3E, 0x28 };
ili9341_write_command(ILI9341_CMD_VMCTR1);
ili9341_write_data(vmctr1, sizeof(vmctr1));

uint8_t vmctr2[] = { 0x86 };
ili9341_write_command(ILI9341_CMD_VMCTR2);
ili9341_write_data(vmctr2, sizeof(vmctr2));
```

#### Phân tích chuyên sâu:
1. **`PWCTR1` (`0xC0`):** Nạp `0x23` (35 thập phân) thiết lập điện áp tham chiếu nội $V_{REG1OUT} = 4.60\text{V}$. Đây là mức trần điện áp cấp cho bộ chuyển đổi DAC để tạo ra các sắc độ màu sắc.
2. **`PWCTR2` (`0xC1`):** Nạp `0x10` thiết lập hệ số bậc thang: $V_{GH} = 6 \times V_{REG1OUT}$ và $V_{GL} = -3 \times V_{REG1OUT}$.
3. **`VMCTR1` (`0xC5`) & `VMCTR2` (`0xC7`):**  
   - $V_{COM}$ là điện áp cấp cho tấm điện cực kính đối diện.
   - `0x3E` tương đương mức áp đỉnh $V_{COMH} = +4.25\text{V}$.
   - `0x28` tương đương mức áp đáy $V_{COML} = -1.50\text{V}$.
   - `VMCTR2` nạp `0x86` là giá trị bù dịch áp (VCOM offset).
   - **Tác dụng thực tế:** Cân bằng tuyệt đối giữa bán kỳ dương và bán kỳ âm của điện áp xoay chiều điều khiển tinh thể lỏng. Nếu thiếu 2 lệnh này, màn hình sẽ bị **gợn sóng lăn tăn nhấp nháy (flicker)** gây mỏi mắt và panel nhanh bị lão hóa (burn-in).

---

### Bước 4: Cấu hình Định dạng màu, Tốc độ quét khung & Quét hiển thị

```c
/* Pixel format: 16-bit / pixel (RGB565) */
uint8_t pixfmt[] = { 0x55 };
ili9341_write_command(ILI9341_CMD_PIXFMT);
ili9341_write_data(pixfmt, sizeof(pixfmt));

/* Frame rate control: 79 Hz */
uint8_t frmctr1[] = { 0x00, 0x18 };
ili9341_write_command(ILI9341_CMD_FRMCTR1);
ili9341_write_data(frmctr1, sizeof(frmctr1));

/* Display Function Control (0xB6) */
uint8_t dfunctr[] = { 0x08, 0x82, 0x27 };
ili9341_write_command(ILI9341_CMD_DFUNCTR);
ili9341_write_data(dfunctr, sizeof(dfunctr));

/* Enable 3G & Gamma Curve */
uint8_t enable3g[] = { 0x00 };
ili9341_write_command(ILI9341_CMD_ENABLE_3G);
ili9341_write_data(enable3g, sizeof(enable3g));

uint8_t gammaset[] = { 0x01 };
ili9341_write_command(ILI9341_CMD_GAMMASET);
ili9341_write_data(gammaset, sizeof(gammaset));
```

- **`PIXFMT` (`0x3A`):** Nạp `0x55` chọn chuẩn 16 bits/pixel (RGB565: 5-bit Red, 6-bit Green, 5-bit Blue).
- **`FRMCTR1` (`0xB1`):** Nạp `{0x00, 0x18}` thiết lập tần số làm tươi khung hình ở mức **79 Hz** (tối ưu cho chuyển động mượt, không sọc quét).
- **`DFUNCTR` (`0xB6`):** Nạp `{ 0x08, 0x82, 0x27 }` cấu hình hoạt động quét 320 dòng hiển thị vật lý (`0x27` = 39, tức $(39 + 1) \times 8 = 320$ dòng).

---

### Bước 5: Cân chỉnh đường cong màu sắc (Gamma Correction)

```c
/* Positive Gamma correction (0xE0) */
uint8_t pgamma[] = {
    0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
    0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00
};
ili9341_write_command(ILI9341_CMD_PGAMMA);
ili9341_write_data(pgamma, sizeof(pgamma));

/* Negative Gamma correction (0xE1) */
uint8_t ngamma[] = {
    0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
    0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F
};
ili9341_write_command(ILI9341_CMD_NGAMMA);
ili9341_write_data(ngamma, sizeof(ngamma));
```

- **Nguyên lý:** Phân tử tinh thể lỏng không xoay tuyến tính theo điện áp nạp vào.
- 15 tham số Positive Gamma (`0xE0`) và 15 tham số Negative Gamma (`0xE1`) uốn nắn đường cong điện áp tương ứng với mắt người.
- Kết quả: Màu đen sâu hoàn toàn, màu trắng tinh khiết, các gam màu trung tính rõ nét, không bị lóa hoặc ám xám.

---

### Bước 6: Cấu hình Hướng hiển thị, Bật màn hình & Xóa sạch GRAM

```c
/* Memory Access Control: Dọc chuẩn + lọc màu BGR */
uint8_t madctl[] = { ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR };
ili9341_write_command(ILI9341_CMD_MADCTL);
ili9341_write_data(madctl, sizeof(madctl));

/* Thoát Sleep Mode */
ili9341_write_command(ILI9341_CMD_SLPOUT);
HAL_Delay(120);

/* Bật hiển thị (Display ON) */
ili9341_write_command(ILI9341_CMD_DISPON);
HAL_Delay(20);

/* Xóa sạch màn hình về màu đen */
ili9341_fill_screen(ILI9341_BLACK);
```

- **`MADCTL` (`0x36`):**
  - `ILI9341_MADCTL_MX (0x40)`: Đảo gương trục X để thứ tự cột quét từ trái qua phải chuẩn ($0 \rightarrow 239$).
  - `ILI9341_MADCTL_BGR (0x08)`: Phần lớn panel TFT 2.8 inch trên thị trường xếp kính lọc màu theo thứ tự **Blue-Green-Red (BGR)**. Bit này đảm bảo màu đỏ và xanh dương không bị tráo đổi cho nhau.
- **`SLPOUT` (`0x11`):** Đánh thức toàn bộ IC. Trễ **120ms** bắt buộc theo Datasheet §8.2.12 để các bộ sạc áp nạp đủ năng lượng.
- **`DISPON` (`0x29`):** Lệnh kích hoạt toàn bộ cổng xuất tín hiệu từ bộ nhớ GRAM ra panel.
- **`ili9341_fill_screen(ILI9341_BLACK)`:** Xóa sạch dữ liệu rác ngẫu nhiên khi bật nguồn, đưa toàn bộ $240 \times 320$ pixels về màu đen sạch sẽ.

---

## 4. PHÂN TÍCH CÁC HÀM CÒN LẠI TRONG THƯ VIỆN

### 4.1. `ili9341_write_command` & `ili9341_write_data`
- `write_command(cmd)`: Kéo `DC = LOW`, `CS = LOW`, truyền 1 byte lệnh qua SPI1, kéo `CS = HIGH`.
- `write_data(p_data, size)`: Kéo `DC = HIGH`, `CS = LOW`, truyền khối mảng byte qua SPI1, kéo `CS = HIGH`.

### 4.2. `ili9341_draw_pixel(x, y, color)`
- Kiểm tra biên $x < 240$ và $y < 320$.
- Cài đặt cửa sổ vẽ $1 \times 1$ tại $(x, y)$ bằng lệnh `CASET (0x2A)` và `PASET (0x2B)`.
- Gửi lệnh `RAMWR (0x2C)` rồi truyền 2 bytes màu theo thứ tự Big-Endian.

### 4.3. `ili9341_fill_rect(x, y, width, height, color)`
Áp dụng giải thuật **Burst Block Transfer** tối ưu hóa:
1. **Kiểm tra biên & Cắt xén (Clipping):** Tự động co kích thước `width` và `height` nếu hình chồm ra ngoài viền màn hình, chống lỗi tràn bộ nhớ.
2. **Cài đặt Address Window 1 lần duy nhất:** Cài `CASET` và `PASET` cho toàn bộ khối $W \times H$.
3. **Nạp sẵn bộ đệm tĩnh `s_burst_buffer[512]`:** Điền sẵn 256 pixel mang màu cần tô.
4. **Bắn liên tục qua SPI1:** Giữ chân `CS = LOW` và dùng `HAL_SPI_Transmit` đẩy từng khối 512 bytes qua SPI1 ở tốc độ 18MHz mà không có thời gian chết.

### 4.4. `ili9341_fill_screen(color)`
- Gọi `ili9341_fill_rect(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);` để quét sạch toàn bộ $240 \times 320 = 76.800$ điểm ảnh với tốc độ tối ưu **~68 mili-giây** ($\approx 14.6$ FPS).
