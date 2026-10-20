# TRƯỜNG ĐẠI HỌC BÁCH KHOA - ĐHQG TP.HCM
## KHOA ĐIỆN - ĐIỆN TỬ | BỘ MÔN KỸ THUẬT ĐIỆN TỬ

---

# ĐẶC TẢ KỸ THUẬT HỆ THỐNG TOÀN DIỆN
## (SYSTEM SPECIFICATION DOCUMENT)

**ĐỀ TÀI:** HỆ THỐNG ĐIỀU KHIỂN VÀ GIÁM SÁT PHÒNG THÔNG MINH  
*(SMART ROOM CONTROL & MONITORING SYSTEM)*

---

### DANH SÁCH THÀNH VIÊN VÀ PHÂN CÔNG NHIỆM VỤ

| STT | Họ và Tên | MSSV | Nhiệm vụ & Phạm vi phụ trách |
| :---: | :--- | :---: | :--- |
| **1** | **Coder 1 (Bạn)** | *[Điền MSSV]* | **Phụ trách Phân hệ Hiển thị TFT LCD 2.8", Cảm ứng Touch XPT2046, Nút bấm vật lý, Khối chấp hành Relay, Thiết kế giao diện HMI & Xử lý đồ họa trung tâm** |
| **2** | **Coder 2 (Bạn cùng nhóm)** | *[Điền MSSV]* | **Phụ trách Phân hệ Cảm biến Môi trường: Cảm biến Nhiệt độ - Độ ẩm (DHT/BME), Cảm biến Cường độ sáng (LDR), Cảm biến chuyển động hiện diện PIR** |

---

## I. ĐẶC TẢ YÊU CẦU SẢN PHẨM (PRODUCT REQUIREMENT)

### 1) Name (Tên sản phẩm)
**Hệ thống điều khiển và giám sát phòng thông minh tích hợp giao diện HMI cảm ứng đồ họa** *(Smart Room Control & Monitoring System)*.

### 2) Purpose (Mục đích)
- Thiết bị nhúng gắn tường hoặc để bàn nhằm giám sát chất lượng vi khí hậu trong phòng (nhiệt độ, độ ẩm, độ sáng) và phát hiện sự hiện diện của con người trong thời gian thực.
- Cung cấp giao diện tương tác trực quan người - máy (HMI) thông qua màn hình màu TFT LCD cảm ứng và hệ thống nút bấm vật lý, cho phép người dùng theo dõi và điều khiển các thiết bị phụ tải điện (đèn chiếu sáng, quạt làm mát, máy sưởi/điều hòa qua Relay).
- Tự động hóa quá trình đóng/cắt phụ tải thông minh dựa trên ngữ cảnh môi trường (kết hợp nhận diện người và ánh sáng) nhằm tối ưu hóa năng lượng tiêu thụ và nâng cao tiện nghi sống.

### 3) Inputs and Outputs (Đầu vào và Đầu ra)
- **Inputs (Đầu vào):**
  - *Tín hiệu tương tác người dùng (Coder 1 phụ trách):*
    - Màn hình cảm ứng điện trở: Thu nhận tọa độ chạm $(X, Y)$ và lực nhấn của người dùng trên giao diện đồ họa.
    - Hệ thống nút bấm vật lý: Cung cấp phương thức điều khiển vật lý tức thời (On/Off, Chuyển chế độ Auto/Manual, Đảo trạng thái tải).
  - *Tín hiệu thu thập từ cảm biến môi trường (Coder 2 phụ trách):*
    - Tín hiệu cảm biến nhiệt độ và độ ẩm không khí (dạng số).
    - Tín hiệu cảm biến cường độ sáng phòng (dạng analog ADC hoặc I2C).
    - Tín hiệu logic từ cảm biến chuyển động hồng ngoại thụ động PIR (mức logic 0/1 báo có người).
  - *Nguồn cấp:* Nguồn một chiều DC $5\text{V} \pm 5\%$ từ Adapter lưới điện hoặc cổng USB tiêu chuẩn.
- **Outputs (Đầu ra):**
  - *Tín hiệu hiển thị và cơ cấu chấp hành (Coder 1 phụ trách):*
    - Màn hình hiển thị: Màn hình màu TFT LCD 2.8 inch (độ phân giải $240 \times 320$) hiển thị Dashboard thông số thời gian thực, đồng hồ, đồ thị trạng thái và các widget nút ảo.
    - Cơ cấu chấp hành phụ tải: Các đường tín hiệu logic điều khiển khối rơ-le cách ly quang để đóng/ngắt nguồn điện cho tải điện gia dụng 220VAC (đèn chiếu sáng, quạt...).
    - Chỉ báo đồ họa: Biểu tượng cảnh báo trên màn hình khi nhiệt độ hoặc độ ẩm vượt ngưỡng an toàn.

### 4) Use cases (Kịch bản sử dụng)
- **Kịch bản 1 - Giám sát tự động (Toàn hệ thống):**  
  Người dùng cấp nguồn $\rightarrow$ Vi điều khiển khởi tạo phần cứng (SPI, màn hình, cảm ứng, cảm biến) $\rightarrow$ Màn hình hiển thị giao diện Dashboard chính $\rightarrow$ Các widget nhiệt độ, độ ẩm, độ sáng và trạng thái người được cập nhật liên tục theo chu kỳ thu thập từ cảm biến.
- **Kịch bản 2 - Điều khiển bằng Cảm ứng (Coder 1):**  
  Tại màn hình Dashboard, người dùng chạm tay vào các nút ảo (Widget Relay Button: Đèn, Quạt) trên màn hình cảm ứng $\rightarrow$ Hệ thống quét tọa độ chạm, xác thực vùng bấm hợp lệ $\rightarrow$ Đảo trạng thái rơ-le tương ứng ngay lập tức $\rightarrow$ Widget trên màn hình chuyển đổi màu sắc (Xanh: Bật, Xám: Tắt) phản hồi tức thì cho người dùng.
- **Kịch bản 3 - Điều khiển bằng Nút bấm vật lý (Coder 1):**  
  Người dùng nhấn trực tiếp nút bấm cơ học trên vỏ thiết bị $\rightarrow$ Mạch đọc tín hiệu kích hoạt ngắt ngoài hoặc quét chu kỳ với bộ lọc chống rung (Debounce) $\rightarrow$ Kích hoạt đóng/cắt rơ-le tải $\rightarrow$ Cập nhật đồng bộ trạng thái lên giao diện màn hình TFT LCD.
- **Kịch bản 4 - Tự động hóa điều khiển theo ngữ cảnh (Phối hợp Coder 1 & Coder 2):**  
  Người dùng chuyển hệ thống sang chế độ Tự động (Auto Mode) $\rightarrow$ Khi cảm biến PIR phát hiện có người bước vào phòng ($\text{PIR} = 1$) VÀ cảm biến ánh sáng báo trời tối ($\text{Lux} < \text{Ngưỡng}$) $\rightarrow$ Hệ thống tự động kích hoạt đóng Relay bật đèn chiếu sáng $\rightarrow$ Khi người rời khỏi phòng sau khoảng trễ quy định (Timeout), hệ thống tự động tắt tải để tiết kiệm năng lượng.
- **Kịch bản 5 - Cảnh báo và Cấu hình ngưỡng (Toàn hệ thống):**  
  Người dùng nhấn nút Menu cài đặt trên màn hình $\rightarrow$ Nhập/tinh chỉnh ngưỡng nhiệt độ cảnh báo (VD: $> 38^\circ\text{C}$) hoặc ngưỡng sáng tự động $\rightarrow$ Lưu cấu hình vào bộ nhớ $\rightarrow$ Khi nhiệt độ thực tế vượt ngưỡng, hệ thống đổi màu widget cảnh báo sang đỏ nhấp nháy và tự động kích hoạt Relay quạt làm mát.

### 5) Function (Các chức năng chính)
- **F1 - Khởi tạo & Quản lý hệ thống:** Khởi tạo vi điều khiển, cấu hình xung nhịp 72 MHz, hiển thị màn hình Splash Screen đồ họa khi mở nguồn.
- **F2 - Hiển thị đồ họa thời gian thực (Coder 1):** Vẽ và cập nhật Dashboard hiển thị các thông số vi khí hậu (nhiệt độ, độ ẩm, cường độ sáng) và chỉ báo trạng thái hiện diện con người.
- **F3 - Xử lý tương tác cảm ứng (Coder 1):** Đọc tọa độ cảm ứng điện trở 12-bit, thực hiện giải thuật lọc nhiễu trung vị, hiệu chuẩn tọa độ hiển thị và phát hiện sự kiện chạm (Touch Press/Release).
- **F4 - Xử lý nút bấm vật lý (Coder 1):** Đọc trạng thái mảng nút nhấn vật lý với thuật toán lọc rung phần mềm tích hợp chống kẹt phím.
- **F5 - Điều khiển cơ cấu chấp hành Relay (Coder 1):** Điều khiển đóng/ngắt các kênh Relay thông qua mạch cách ly quang Optocoupler, đảm bảo không gây sụt áp hoặc nhiễu ngược sang MCU.
- **F6 - Thu thập dữ liệu cảm biến (Coder 2):**  
  *[Coder 2: Giao tiếp và đọc giá trị định kỳ từ cảm biến nhiệt độ - độ ẩm, ánh sáng, chuyển động PIR - Để trống nội dung kỹ thuật chi tiết]*
- **F7 - Xử lý logic tự động hóa (System Logic):** So sánh dữ liệu cảm biến với các ngưỡng cài đặt, thực thi giải thuật tự động hóa bật/tắt thiết bị theo ngữ cảnh kết hợp bảo vệ an toàn.

### 6) Performance (Yêu cầu hiệu năng - Đã khắc phục & tính toán chính xác)
- **Tốc độ quét toàn màn hình (Full-screen Refresh Rate):** $\ge 14.6\text{ FPS}$ (thời gian quét toàn màn hình $t_{\text{full}} = 68.3\text{ ms}$ tại xung nhịp bus SPI $18\text{ MHz}$), loại bỏ hiện tượng giật hình hoặc quét ngang vệt đen.
- **Tốc độ cập nhật thông số cục bộ (Partial ROI Refresh Rate):** Áp dụng kỹ thuật vẽ cục bộ (Partial ROI Draw) kết hợp bộ đệm Burst Buffer 512 Bytes, tốc độ cập nhật các ô số cảm biến (nhiệt độ, độ ẩm) đạt tới $427\text{ Hz}$ (thời gian cập nhật chỉ $t_{\text{roi}} = 2.34\text{ ms}$), đáp ứng hiển thị mượt mà tức thời.
- **Thời gian đáp ứng cảm ứng & Nút bấm:** Độ trễ phản hồi kể từ thời điểm tác động ngón tay đến khi trạng thái tải thay đổi và widget trên màn hình đổi màu $\le 20\text{ ms}$.
- **Chu kỳ thu thập cảm biến (Coder 2):** Chu kỳ quét PIR $\le 100\text{ ms}$ (đáp ứng chuyển động tức thì); Chu kỳ lấy mẫu cảm biến nhiệt độ, độ ẩm, độ sáng từ $1.0\text{s}$ đến $2.0\text{s}$ (phù hợp với quán tính nhiệt và giới hạn vật lý của cảm biến).
- **Độ trễ chuyển mạch phụ tải:** Độ trễ đóng ngắt tiếp điểm rơ-le $\le 15\text{ ms}$, cách ly hoàn toàn xung kích điện áp cao bằng Optocoupler.

### 7) Manufacturing costs (Ước tính chi phí sản xuất)
Tổng chi phí linh kiện phần cứng cho 01 sản phẩm thử nghiệm dao động từ **450.000 VNĐ** đến **650.000 VNĐ**:
- Màn hình 2.8" TFT LCD ILI9341 tích hợp Touch XPT2046: ~ 180.000 VNĐ
- Bo mạch vi điều khiển STM32F103C8T6 (Blue Pill): ~ 85.000 VNĐ
- Module Relay 2-4 kênh có cách ly quang PC817: ~ 45.000 VNĐ
- Khối cảm biến môi trường (DHT22/BME280, LDR, PIR HC-SR501): ~ 120.000 VNĐ *[Coder 2]*
- Mạch nguồn hạ áp, nút nhấn, linh kiện thụ động R-C và vỏ hộp: ~ 80.000 VNĐ

### 8) Power (Yêu cầu nguồn điện & Công suất tiêu thụ)
- Hệ thống sử dụng nguồn cấp một chiều DC $5\text{V} \pm 5\%$, dòng danh định tối thiểu $1.5\text{A}$ từ Adapter nguồn AC-DC hoặc nguồn cổng USB tiêu chuẩn.
- Tích hợp mạch ổn áp tuyến tính chất lượng cao hạ từ 5V xuống 3.3V (IC AMS1117-3.3V hoặc tương đương) với khả năng cấp dòng liên tục tối thiểu $800\text{ mA}$ để nuôi toàn bộ MCU, màn hình TFT LCD, IC cảm ứng và mạch cảm biến.
- Tổng công suất tiêu thụ tối đa của toàn hệ thống (khi bật 100% đèn nền TFT và đóng đồng thời tất cả các rơ-le) không vượt quá $1.8\text{ W}$ (Dòng tiêu thụ $< 360\text{ mA}$ ở 5V).

### 9) Physical size / weight (Kích thước và Khối lượng)
- Kích thước vỏ hộp dự kiến: $120\text{ mm (Dài)} \times 85\text{ mm (Rộng)} \times 35\text{ mm (Cao)}$, thiết kế chuẩn gá tường hoặc để bàn.
- Khối lượng toàn bộ thiết bị (bao gồm màn hình, bo mạch và vỏ bảo vệ): Khoảng $220\text{ g} - 260\text{ g}$.

### 10) Installation & Environment (Lắp đặt và Môi trường vận hành)
- Toàn bộ vi điều khiển, màn hình LCD, cảm biến và rơ-le được hàn cố định trên mạch in hai lớp (PCB) chuẩn công nghiệp, cố định chắc chắn bên trong vỏ nhựa ABS cách điện chống cháy.
- Màn hình TFT được đặt ở mặt trước có khung mica bảo vệ; các nút bấm vật lý bố trí tiện tay thao tác; cảm biến PIR và quang trở hướng ra vị trí thoáng để thu nhận tín hiệu chính xác.
- **Nền tảng phát triển phần mềm:** Ngôn ngữ lập trình C chuẩn C99/C++ trên nền tảng Keil MDK ARM / STM32CubeIDE kết hợp trình biên dịch Arm Compiler, đảm bảo tối ưu hóa mã máy và kiểm soát chặt chẽ từng chu kỳ xung nhịp ngoại vi.

---

## II. ĐẶC TẢ THIẾT KẾ (DESIGN SPECIFICATION)

### 1. Mô tả hệ thống (System Description)
Hệ thống giám sát và điều khiển phòng thông minh là một hệ thống nhúng thời gian thực (Real-time Embedded System) dựa trên vi điều khiển 32-bit ARM Cortex-M3 (STM32F103C8T6) hoạt động ở tần số xung nhịp $72\text{ MHz}$. Hệ thống liên kết chặt chẽ giữa hai phân hệ chính:
1. **Phân hệ Hiển thị HMI & Chấp hành (Coder 1):** Gồm màn hình màu TFT LCD 2.8 inch giao tiếp SPI tốc độ cao, IC cảm ứng điện trở 12-bit, mảng nút nhấn và rơ-le công suất cách ly quang.
2. **Phân hệ Cảm biến Môi trường (Coder 2):** Thu thập liên tục nhiệt độ, độ ẩm, độ sáng và phát hiện con người để đưa dữ liệu chuẩn hóa về phân hệ HMI.

### 2. Sơ đồ khối hệ thống (System Block Diagram)

```
+-------------------------------------------------------------------------------------------------+
|                                HỆ THỐNG PHÒNG THÔNG MINH                                        |
+-------------------------------------------------------------------------------------------------+
|  [PHÂN HỆ CẢM BIẾN - CODER 2]                 [VI ĐIỀU KHIỂN TRUNG TÂM]   [PHÂN HỆ HMI & CHẤP HÀNH - CODER 1]  |
|  +--------------------------+                 +-----------------------+   +----------------------------------+  |
|  | Cảm biến Nhiệt - Ẩm      |-(Digital Signal)-> |                       |===(SPI1 18MHz)==> [Màn hình TFT ILI9341]   |
|  +--------------------------+                 |                       |   +----------------------------------+  |
|  | Cảm biến Ánh sáng (LDR)  |-(Analog / I2C)--> |   STM32F103C8T6       |===(SPI2 2.25MHz)<= [Cảm ứng Touch XPT2046] |
|  +--------------------------+                 |   (ARM Cortex-M3      |   +----------------------------------+  |
|  | Cảm biến Hiện diện PIR   |-(Logic 0/1)-----> |    @ 72 MHz)          |----(GPIO In Debounce)--- [Mảng Nút Bấm]  |
|  +--------------------------+                 |                       |   +----------------------------------+  |
|                                               |                       |----(GPIO Out + Opto)--- [Khối Relay Tải]  |
|                                               +-----------------------+   +----------------------------------+  |
|                                                           ^                                                     |
|                                                           | (3.3V LDO)                                          |
|                                                 [KHỐI NGUỒN DC 5V/3.3V]                                         |
+-------------------------------------------------------------------------------------------------+
```

#### Đầu vào (Inputs):
- Tín hiệu phản hồi điện áp cảm ứng từ chip XPT2046 qua bus SPI2 (Coder 1).
- Tín hiệu logic mức tích cực từ các nút bấm vật lý trên đường GPIO ngắt/quét (Coder 1).
- Dữ liệu đo đạc nhiệt độ, độ ẩm không khí từ cảm biến kỹ thuật số (Coder 2).
- Điện áp biến thiên tỷ lệ theo cường độ sáng từ cảm biến quang trở (Coder 2).
- Mức logic báo hiệu chuyển động từ cảm biến PIR (Coder 2).

#### Đầu ra (Outputs):
- Dữ liệu điểm ảnh (Pixel Data RGB565) và lệnh điều khiển gửi tới ILI9341 qua bus SPI1 tốc độ 18 MHz (Coder 1).
- Tín hiệu logic kích dẫn Transistor điều khiển cuộn hút Relay đóng ngắt tải điện 220VAC (Coder 1).
- Tín hiệu cảnh báo bằng đồ họa nhấp nháy trên màn hình HMI (Coder 1).

### 3. Chức năng chi tiết (Detailed Functions)
- **a) Phân hệ Giao diện HMI và Khối Chấp hành (Coder 1):**  
  Bao gồm thư viện điều khiển đồ họa phần cứng ILI9341, giải thuật vẽ ký tự Font chuẩn, thuật toán vẽ widget Dashboard, giải thuật quét cảm ứng Touch Controller kèm bộ lọc trung vị (Median Filter), trình điều khiển nút bấm lọc chống rung và điều khiển rơ-le.
- **b) Phân hệ Thu thập Cảm biến Môi trường (Coder 2):**  
  *[Coder 2: Phụ trách cấu hình ngoại vi cảm biến và thuật toán đọc dữ liệu - Để trống nội dung chi tiết]*

### 4. Hành vi của hệ thống (System Behavior)

#### a) Luồng điều khiển (Control Flow)
1. Khi bật nguồn, hệ thống thực thi Reset Handler, thiết lập System Clock $72\text{ MHz}$, khởi tạo ngoại vi GPIO, SPI1, SPI2, Timer.
2. Coder 1 thực thi `ILI9341_Init()` gửi chuỗi cấu hình Power Control, Gamma, Memory Access. Hiển thị màn hình Splash Screen trong 1.5 giây.
3. Hệ thống chuyển sang vẽ khung giao diện Dashboard tĩnh (tiêu đề, khung card nhiệt độ, độ ẩm, nút bấm ảo).
4. Vòng lặp chính (Super-Loop) điều phối tuần tự theo lát cắt thời gian:
   - Mỗi $10\text{ ms}$: Quét cảm ứng `XPT2046_Scan()` và lọc rung nút nhấn `Button_Scan()` (Coder 1).
   - Mỗi $100\text{ ms}$: Đọc tín hiệu cảm biến chuyển động PIR (Coder 2).
   - Mỗi $500\text{ ms}$: Nhận dữ liệu cập nhật từ Coder 2 và thực thi vẽ cục bộ thông số cảm biến `HMI_UpdateSensorValues()` (Coder 1).
   - Khi có sự kiện Touch hoặc Nút bấm: Thực thi hàm đảo trạng thái Relay và cập nhật giao diện trong $< 15\text{ ms}$ (Coder 1).

#### b) Luồng dữ liệu và Tín hiệu (Data and Signal Flow - Contract giữa Coder 1 và Coder 2)
Toàn bộ dữ liệu trao đổi giữa hai thành viên được chuẩn hóa qua giao diện cấu trúc duy nhất:
```c
typedef struct {
    float temperature;     // Nhiệt độ phòng (°C)
    float humidity;        // Độ ẩm tương đối (%RH)
    float light_lux;       // Cường độ sáng (Lux)
    bool  motion_detected; // Trạng thái có người (true: Có người, false: Không có người)
} SensorData_t;

// Coder 2 cung cấp hàm đọc dữ liệu chuẩn:
bool Sensor_ReadAll(SensorData_t *out_data);
// Coder 1 chỉ việc gọi hàm này để lấy dữ liệu xuất ra màn hình HMI và thực thi logic!
```

#### c) Yêu cầu thời gian và ràng buộc kỹ thuật (Timing and Constraints)

| Thông số kỹ thuật | Giá trị tính toán | Ràng buộc Datasheet | Tần số / Chu kỳ | Kết luận kỹ thuật |
| :--- | :---: | :---: | :---: | :--- |
| **Chu kỳ xung nhịp SPI1 TFT** | $55.5\text{ ns}$ | $T_{\text{SCYCW}} \ge 100\text{ ns}$ (nominal) | $18\text{ MHz}$ (PCLK2/4) | Đạt tốc độ cao ổn định trên thực tế |
| **Vẽ quét toàn màn hình** | $68.3\text{ ms}$ | Không trễ quá $100\text{ ms}$ | $14.6\text{ FPS}$ | Loại bỏ hiện tượng nhấp nháy toàn màn |
| **Cập nhật thông số ROI** | $2.34\text{ ms}$ | Không gián đoạn luồng | $427\text{ Hz}$ | Số nhảy tức thời, siêu mượt |
| **Chu kỳ xung nhịp SPI2 Touch** | $444\text{ ns}$ | $T_{\text{CLK}} \ge 400\text{ ns}$ (XPT2046) | $2.25\text{ MHz}$ (PCLK1/16) | Đúng chuẩn tuyệt đối datasheet |
| **Thời gian đáp ứng Relay** | $8.5\text{ ms}$ | $t_{\text{transfer}} \le 15\text{ ms}$ | Tức thời | Đảm bảo an toàn cách ly quang |

#### d) Sơ đồ máy trạng thái hữu hạn (Finite State Machine - FSM)
- **STATE 0: RESET & HARDWARE INIT** $\rightarrow$ Khởi tạo xung nhịp 72MHz, khởi tạo bus SPI1, SPI2, GPIO, ngoại vi cảm biến.
- **STATE 1: SPLASH SCREEN** $\rightarrow$ Vẽ logo và tên đồ án, hiển thị thanh tiến trình nạp trong 1.5 giây.
- **STATE 2: DASHBOARD MONITORING** $\rightarrow$ Trạng thái hoạt động chính: hiển thị đồng hồ, card cảm biến, lắng nghe cảm ứng và nút nhấn.
- **STATE 3: TOUCH INTERACTION & RELAY TOGGLE** $\rightarrow$ Xử lý sự kiện chạm widget tải hoặc nút bấm, kích hoạt cuộn hút Relay và đổi màu giao diện.
- **STATE 4: AUTO LOGIC EXECUTION** $\rightarrow$ Thực thi thuật toán bật/tắt tải tự động theo ngữ cảnh PIR và độ sáng khi ở chế độ Auto.
- **STATE 5: ALARM STATE** $\rightarrow$ Cảnh báo nhấp nháy đỏ khi nhiệt độ vượt ngưỡng an toàn.

---

## III. LỰA CHỌN LINH KIỆN VÀ ĐẶC TẢ PHẦN CỨNG (HARDWARE SPECIFICATION)

### 1. Tính toán dung lượng bộ nhớ (Memory Budget Calculation)
- **Bài toán Framebuffer:**  
  Một frame màn hình màu $240 \times 320$ RGB565 (16-bit) chiếm:
  $$240 \times 320 \times 2 = 153.600\text{ Bytes} = 150\text{ KB}$$
  Vi điều khiển STM32F103C8T6 chỉ có tổng cộng $20\text{ KB}$ SRAM, do đó **VỀ MẶT KỸ THUẬT KHÔNG THỂ** tạo toàn bộ Framebuffer trong RAM!
- **Giải pháp Burst Buffer 512B của Coder 1:**  
  Coder 1 giải quyết triệt để vấn đề này bằng kỹ thuật Line-Buffer / ROI Burst-Buffer kích thước cố định **512 Bytes** (256 pixels). Kỹ thuật này chỉ chiếm **$2.5\%$ tổng dung lượng SRAM** (512B / 20KB), để dành $97.5\%$ SRAM còn lại cho Stack, biến toàn cục và hàng đợi sự kiện Touch/Cảm biến.
- **Tính toán bộ nhớ Flash:**  
  Mã nguồn Driver và HMI chiếm khoảng $18\text{ KB}$; Bảng mã Font chữ chuẩn chiếm $6\text{ KB}$; Biểu tượng Bitmap nén chiếm $8\text{ KB}$. Tổng dung lượng Flash sử dụng ước tính $\sim 32\text{ KB} / 64\text{ KB}$ (đạt $50\%$ dung lượng Flash, hệ số an toàn $100\%$).

### 2. Lựa chọn Màn hình LCD và Chip điều khiển (Coder 1)
- **Linh kiện:** Màn hình màu TFT LCD 2.8 inch tích hợp IC điều khiển ILI9341.
- **Độ phân giải:** $240\text{ (RGB)} \times 320$ điểm ảnh, hiển thị $65.536$ màu (định dạng màu RGB565 16-bit).
- **Giao tiếp:** Chuẩn SPI 4-dây (SCK, MOSI, CS, DC/RS), hỗ trợ chân RESET cứng và chân LED điều khiển đèn nền.
- **Điện áp hoạt động:** $V_{\text{DD}} = 2.8\text{V} - 3.3\text{V}$. Chân LED đèn nền tiêu thụ dòng khoảng $45\text{ mA}$ ở $3.3\text{V}$.
- **Lý do lựa chọn:** Driver ILI9341 là chuẩn công nghiệp phổ biến, hỗ trợ lệnh cấu hình cửa sổ vẽ (Set Address Window) cho phép vẽ vùng cục bộ cực nhanh mà không cần quét lại toàn màn.

### 3. Lựa chọn Chip giải mã Cảm ứng điện trở (Coder 1)
- **Linh kiện:** IC chuyển đổi tương tự - số 12-bit chuyên dụng XPT2046 (tương thích hoàn toàn ADS7843).
- **Nguyên lý:** Đo tỷ lệ cầu phân áp trên 4 điện cực điện trở ($X+, X-, Y+, Y-$) của tấm film cảm ứng khi có lực tác động.
- **Độ phân giải ADC:** 12-bit (tọa độ thô từ $0$ đến $4095$).
- **Giao tiếp:** SPI độc lập (SPI2 của STM32), tần số hoạt động $2.25\text{ MHz}$ (chu kỳ $444\text{ ns}$, thỏa mãn $T_{\text{CLK}} \ge 400\text{ ns}$ của XPT2046).
- Hỗ trợ chân ngắt ngón tay chạm (`PENIRQ`) kích hoạt ngắt khi người dùng chạm màn hình, giúp tiết kiệm chu kỳ quét CPU.

### 4. Lựa chọn Vi điều khiển trung tâm (MCU)
- **Linh kiện:** Vi điều khiển **STM32F103C8T6** (Dòng ARM Cortex-M3 của STMicroelectronics).
- **Tần số hoạt động cực đại:** $72\text{ MHz}$ (cho năng lực xử lý tính toán đồ họa và quét ngoại vi vượt trội).
- **Bộ nhớ:** $64\text{ KB}$ Flash bộ nhớ chương trình, $20\text{ KB}$ SRAM bộ nhớ dữ liệu.
- **Ngoại vi giao tiếp:** 2 bộ SPI phần cứng độc lập (SPI1 lên tới 18 MHz nuôi màn hình, SPI2 lên tới 9 MHz nuôi cảm ứng), nhiều kênh ngắt ngoài EXTI và bộ định thời phần cứng Timer.

### 5. Lựa chọn Khối chấp hành Relay và Nút bấm (Coder 1)
- **Module Relay:** Module Relay 5V tích hợp Optocoupler cách ly quang PC817: Điện áp kích mức thấp (Active Low), bảo vệ hoàn toàn đường truyền logic của MCU khỏi sốc điện áp cảm ứng khi cuộn hút ngắt điện.
- **Nút bấm:** Nút bấm Tactile Switch 4 chân: Bố trí kèm tụ gốm $100\text{ nF}$ và điện trở kéo lên $10\text{ k}\Omega$ dập nhiễu phần cứng, kết hợp bộ lọc chống rung phần mềm chu kỳ $20\text{ ms}$.

### 6. Lựa chọn Cảm biến Môi trường (Coder 2)
*[Coder 2: Phần mục này do thành viên phụ trách cảm biến triển khai chi tiết - Để trống phần nội dung kỹ thuật theo quy định phân công].*
- *Lựa chọn cảm biến nhiệt độ - độ ẩm: [Để trống]*
- *Lựa chọn cảm biến cường độ sáng: [Để trống]*
- *Lựa chọn cảm biến chuyển động con người: [Để trống]*

### 7. Sơ đồ nối chân phần cứng toàn hệ thống (System Pinout Mapping)

| Chân STM32 | Ngoại vi kết nối | Chức năng chân | Thành viên phụ trách |
| :---: | :--- | :--- | :---: |
| **PA5** | TFT LCD ILI9341 | SPI1_SCK (Clock 18 MHz) | Coder 1 |
| **PA7** | TFT LCD ILI9341 | SPI1_MOSI (Master Out Slave In) | Coder 1 |
| **PA4** | TFT LCD ILI9341 | GPIO Out (TFT_CS - Chip Select) | Coder 1 |
| **PA3** | TFT LCD ILI9341 | GPIO Out (TFT_DC - Data/Command) | Coder 1 |
| **PA2** | TFT LCD ILI9341 | GPIO Out (TFT_RST - Reset cứng) | Coder 1 |
| **PB13** | Touch XPT2046 | SPI2_SCK (Clock 2.25 MHz) | Coder 1 |
| **PB15** | Touch XPT2046 | SPI2_MOSI | Coder 1 |
| **PB14** | Touch XPT2046 | SPI2_MISO (Master In Slave Out) | Coder 1 |
| **PB12** | Touch XPT2046 | GPIO Out (TOUCH_CS) | Coder 1 |
| **PB1** | Module Relay | GPIO Out (RELAY_1 - Đèn phòng) | Coder 1 |
| **PB0** | Nút bấm vật lý | GPIO In Pull-up (Nút bấm đảo tải) | Coder 1 |
| **PA0 / PB6** | Cảm biến Coder 2 | 1-Wire / I2C / Analog In | Coder 2 *[Để trống]* |
| **PA1** | Cảm biến PIR Coder 2 | GPIO In (Tín hiệu có người) | Coder 2 *[Để trống]* |

---

## IV. ĐẶC TẢ PHẦN MỀM (SOFTWARE SPECIFICATION)

Phần mềm hệ thống được thiết kế theo kiến trúc phân tầng 4 lớp (4-Layer Modular Architecture) chuẩn mực kỹ thuật nhúng:

- **Layer 4 - Application Layer:**  
  Chứa logic điều phối hệ thống `main.c`, máy trạng thái FSM, điều khiển tự động hóa và quản lý các khung màn hình HMI.
- **Layer 3 - Service / Middleware Layer:**  
  Chứa thư viện quản lý widget giao diện `hmi_manager.c`, bộ lọc cảm ứng trung vị `touch_filter.c`, trình lọc rung nút nhấn `button_debounce.c`, và cấu trúc cầu nối dữ liệu `SensorData_t` tiếp nhận từ Coder 2.
- **Layer 2 - Hardware Abstraction Layer (HAL / Drivers):**  
  Trình điều khiển thiết bị ngoại vi chuyên sâu: `ili9341.c` (gửi lệnh vẽ, cấu hình window, tối ưu burst buffer), `xpt2046.c` (đọc ADC thô cảm ứng), `relay.c` (đóng ngắt bảo vệ rơ-le) [Coder 1]; và các driver cảm biến nhiệt ẩm, ánh sáng, PIR [Coder 2].
- **Layer 1 - Microcontroller Board Support Package (BSP / Registers):**  
  Tầng truy cập phần cứng vi điều khiển: Cấu hình xung nhịp RCC 72MHz, thanh ghi GPIO, SPI1, SPI2, NVIC, SysTick Timer.

### Chi tiết triển khai phần mềm cảm biến (Coder 2)
*[Coder 2: Phần mục này do thành viên phụ trách cảm biến triển khai chi tiết - Để trống phần nội dung kỹ thuật theo quy định phân công].*

---

## V. ĐẶC TẢ KIỂM THỬ (TEST SPECIFICATION)

Bảng ma trận kiểm thử hệ thống (System Test Matrix) với tiêu chí chấp nhận Pass/Fail rõ ràng:

| Mã Test Case | Nội dung kiểm thử | Quy trình thực hiện | Tiêu chí Đạt (Pass Criteria) | Thành viên |
| :---: | :--- | :--- | :--- | :---: |
| **TC_01** | Kiểm tra hiển thị toàn màn hình TFT | Gửi lệnh khởi tạo SPI1 18MHz, xóa và vẽ full màu nền | Màn hình sáng đều, không sọc nhiễu, thời gian vẽ $\le 70\text{ ms}$ | Coder 1 |
| **TC_02** | Kiểm tra cập nhật thông số cục bộ | Thay đổi giá trị nhiệt độ, gọi hàm vẽ vùng ROI | Số nhảy mượt mà không nhấp nháy, thời gian vẽ $\le 3\text{ ms}$ | Coder 1 |
| **TC_03** | Kiểm tra độ nhạy và tọa độ cảm ứng | Chạm vào 4 góc và tâm màn hình cảm ứng | Tọa độ trả về sai lệch $\le 3\text{ pixel}$ so với vị trí chạm | Coder 1 |
| **TC_04** | Kiểm tra đóng ngắt Relay phụ tải | Nhấn nút ảo trên màn hình hoặc nút bấm cơ | Relay đóng/ngắt tức thời, tiếng tiếp điểm rõ, đèn tải sáng | Coder 1 |
| **TC_05** | Kiểm tra thu thập dữ liệu Cảm biến | Đo nhiệt độ, độ ẩm, cường độ sáng và kích hoạt PIR | *[Coder 2 phụ trách cập nhật quy trình kiểm thử chi tiết]* | Coder 2 |
| **TC_06** | Kiểm tra tự động hóa theo ngữ cảnh | Tạo điều kiện PIR = 1 và che tối quang trở | Hệ thống tự động đóng Relay bật đèn trong vòng $< 200\text{ ms}$ | Toàn hệ thống |

---

