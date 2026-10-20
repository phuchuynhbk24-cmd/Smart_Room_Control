# TRƯỜNG ĐẠI HỌC BÁCH KHOA - ĐHQG TP.HCM
## KHOA ĐIỆN - ĐIỆN TỬ | BỘ MÔN KỸ THUẬT ĐIỆN TỬ

---

# ĐẶC TẢ KỸ THUẬT HỆ THỐNG
## (SYSTEM SPECIFICATION DOCUMENT)

**ĐỀ TÀI:** HỆ THỐNG ĐIỀU KHIỂN VÀ GIÁM SÁT PHÒNG THÔNG MINH  
*(SMART ROOM CONTROL & MONITORING SYSTEM)*

---

### DANH SÁCH THÀNH VIÊN VÀ PHÂN CÔNG NHIỆM VỤ

| STT | Họ và Tên | MSSV | Nhiệm vụ phân công |
| :---: | :--- | :---: | :--- |
| **1** | [Họ và tên bạn] | [Điền MSSV] | Thiết kế giao diện HMI, điều khiển hiển thị TFT LCD 2.8", cảm ứng Touch XPT2046, nút bấm và cơ cấu chấp hành Relay |
| **2** | [Họ và tên bạn cùng nhóm] | [Điền MSSV] | Thu thập dữ liệu cảm biến và tiền xử lý tín hiệu môi trường |

---

## I. ĐẶC TẢ YÊU CẦU SẢN PHẨM (PRODUCT REQUIREMENT)

### 1) Name:
**Hệ thống điều khiển và giám sát phòng thông minh tích hợp giao diện HMI cảm ứng** *(Smart Room Control & Monitoring System)*.

### 2) Purpose:
- Cung cấp giao diện tương tác người - máy (HMI) trực quan trên màn hình màu TFT LCD 2.8" cảm ứng và các nút bấm vật lý để theo dõi thông số trạng thái phòng và điều khiển các thiết bị phụ tải điện qua Relay.
- *+ Phân hệ cảm biến môi trường: [Nội dung đang được cập nhật...]*

### 3) Inputs and outputs:
- **Inputs:**
  - Tọa độ chạm $(X, Y)$ và lực nhấn từ màn hình cảm ứng điện trở 12-bit.
  - Tín hiệu logic từ hệ thống nút nhấn vật lý trên vỏ thiết bị.
  - Nguồn điện một chiều 5V DC từ Adapter nguồn hoặc cổng USB.
  - *+ Tín hiệu thu thập từ các cảm biến: [Nội dung đang được cập nhật...]*
- **Outputs:**
  - Màn hình màu TFT LCD 2.8" ($240 \times 320$ pixel) hiển thị Dashboard thông số thời gian thực, đồng hồ và các nút ảo điều khiển.
  - Tín hiệu điều khiển cuộn hút Relay đóng/cắt tải điện 220VAC qua mạch cách ly quang.
  - Biểu tượng cảnh báo đồ họa trên màn hình HMI.
  - *+ Tín hiệu điều khiển cảnh báo bổ sung: [Nội dung đang được cập nhật...]*

### 4) Use cases:
- **+ Kịch bản 1 (Hiển thị HMI):** Khởi động nguồn $\rightarrow$ Vi điều khiển khởi tạo phần cứng $\rightarrow$ Màn hình hiển thị Splash Screen chào mừng $\rightarrow$ Chuyển sang giao diện Dashboard chính hiển thị các thông số thời gian thực.
- **+ Kịch bản 2 (Điều khiển Cảm ứng):** Người dùng chạm tay vào nút ảo (Widget Relay) trên màn hình TFT $\rightarrow$ Hệ thống quét tọa độ cảm ứng $\rightarrow$ Đảo trạng thái Relay tương ứng $\rightarrow$ Cập nhật màu sắc nút ảo trên màn hình phản hồi cho người dùng.
- **+ Kịch bản 3 (Điều khiển Nút bấm vật lý):** Người dùng nhấn nút cơ học trên vỏ hộp $\rightarrow$ Mạch lọc chống rung phần mềm xác nhận nhấn phím $\rightarrow$ Đảo trạng thái Relay $\rightarrow$ Đồng bộ trạng thái lên màn hình TFT.
- *+ Kịch bản 4 (Tự động hóa theo dữ liệu cảm biến): [Nội dung đang được cập nhật...]*

### 5) Function:
- Khởi tạo hệ thống, cấu hình xung nhịp, hiển thị giao diện Splash Screen đồ họa khi mở nguồn.
- Vẽ và cập nhật Dashboard hiển thị các thông số đo và trạng thái thiết bị thời gian thực.
- Đọc tọa độ cảm ứng điện trở 12-bit, thực hiện giải thuật lọc nhiễu trung vị và hiệu chuẩn tọa độ hiển thị.
- Đọc trạng thái mảng nút nhấn vật lý với thuật toán lọc chống rung phần mềm.
- Điều khiển đóng/ngắt các kênh Relay công suất qua mạch cách ly quang Optocoupler.
- *+ Các chức năng xử lý cảm biến: [Nội dung đang được cập nhật...]*

### 6) Performance:
- Tốc độ quét toàn màn hình: $\ge 14.6\text{ FPS}$ ($t_{\text{full}} = 68.3\text{ ms}$ tại xung nhịp bus SPI $18\text{ MHz}$), không có hiện tượng chớp nháy.
- Tốc độ cập nhật thông số cục bộ (Partial ROI Draw với Burst Buffer 512B): Đạt $427\text{ Hz}$ ($t_{\text{roi}} = 2.34\text{ ms}$), số nhảy tức thời.
- Thời gian phản hồi cảm ứng và nút bấm: $\le 20\text{ ms}$.
- Độ trễ đóng ngắt tiếp điểm Relay: $\le 15\text{ ms}$.
- *+ Yêu cầu hiệu năng lấy mẫu cảm biến: [Nội dung đang được cập nhật...]*

### 7) Manufacturing costs:
- Màn hình TFT LCD 2.8" ILI9341 tích hợp Touch XPT2046: ~ 180.000 VNĐ
- Bo mạch vi điều khiển STM32F103C8T6: ~ 85.000 VNĐ
- Module Relay cách ly quang PC817: ~ 45.000 VNĐ
- Nút bấm, linh kiện thụ động R-C, mạch nguồn và vỏ hộp: ~ 80.000 VNĐ
- *+ Chi phí linh kiện khối cảm biến: [Nội dung đang được cập nhật...]*

### 8) Power:
- Hệ thống sử dụng nguồn cấp một chiều DC $5\text{V} \pm 5\%$, dòng tối thiểu 1.5A từ Adapter nguồn AC-DC hoặc cổng USB.
- Công suất tiêu thụ tối đa của khối vi điều khiển, màn hình TFT và Relay $\le 1.5\text{ W}$ (Dòng tiêu thụ $< 300\text{ mA}$ ở 5V).
- *+ Công suất tiêu thụ khối cảm biến: [Nội dung đang được cập nhật...]*

### 9) Physical size/weight:
- Kích thước vỏ hộp dự kiến: $120\text{ mm} \times 85\text{ mm} \times 35\text{ mm}$.
- Khối lượng toàn bộ thiết bị: Khoảng $220\text{ g} - 260\text{ g}$.

### 10) Installation:
- Vi điều khiển, màn hình TFT, relay và nút bấm được hàn cố định trên mạch in PCB, đặt trong vỏ hộp bảo vệ có khoét khung màn hình.
- Môi trường lập trình: Ngôn ngữ C (chuẩn C99) trên IDE Keil MDK / STM32CubeIDE.
- *+ Yêu cầu lắp đặt cảm biến môi trường: [Nội dung đang được cập nhật...]*

---

## II. ĐẶC TẢ THIẾT KẾ (DESIGN SPECIFICATION)

### Mô tả hệ thống:
Hệ thống giám sát và điều khiển phòng thông minh sử dụng vi điều khiển 32-bit ARM Cortex-M3 (STM32F103C8T6) hoạt động ở tần số $72\text{ MHz}$ làm bộ xử lý trung tâm. Thiết bị có màn hình màu TFT LCD 2.8 inch tích hợp cảm ứng để giám sát thông số thời gian thực và điều khiển các thiết bị phụ tải điện trong phòng thông qua các kênh Relay công suất.

### Sơ đồ khối:

```
+-------------------------------------------------------------------------------------------------+
|                                HỆ THỐNG PHÒNG THÔNG MINH                                        |
+-------------------------------------------------------------------------------------------------+
|  [KHỐI CẢM BIẾN MÔI TRƯỜNG]                   [VI ĐIỀU KHIỂN TRUNG TÂM]   [KHỐI HIỂN THỊ & ĐIỀU KHIỂN HMI]      |
|  +--------------------------+                 +-----------------------+   +----------------------------------+  |
|  | [Sơ đồ khối cảm biến     |---------------->|                       |===(SPI1 18MHz)==> [Màn hình TFT ILI9341]   |
|  |  đang được cập nhật...]  |                 |   STM32F103C8T6       |   +----------------------------------+  |
|  +--------------------------+                 |   (ARM Cortex-M3      |===(SPI2 2.25MHz)<= [Cảm ứng Touch XPT2046] |
|                                               |    @ 72 MHz)          |   +----------------------------------+  |
|                                               |                       |----(GPIO In Debounce)--- [Mảng Nút Bấm]  |
|                                               |                       |   +----------------------------------+  |
|                                               |                       |----(GPIO Out + Opto)--- [Khối Relay Tải]  |
|                                               +-----------------------+   +----------------------------------+  |
|                                                           ^                                                     |
|                                                           | (3.3V LDO)                                          |
|                                                 [KHỐI NGUỒN DC 5V/3.3V]                                         |
+-------------------------------------------------------------------------------------------------+
```

#### Đầu vào:
- Tín hiệu phản hồi điện áp cảm ứng từ chip XPT2046 qua bus SPI2.
- Tín hiệu mức logic từ các nút bấm vật lý qua GPIO chống rung.
- *+ Tín hiệu ngõ vào cảm biến: [Nội dung đang được cập nhật...]*

#### Đầu ra:
- Dữ liệu điểm ảnh và lệnh gửi tới ILI9341 qua bus SPI1 tốc độ 18 MHz.
- Tín hiệu điều khiển Transistor kích cuộn hút Relay đóng/cắt tải điện.
- Thông tin trạng thái và cảnh báo hiển thị trên màn hình HMI.
- *+ Tín hiệu ngõ ra bổ sung: [Nội dung đang được cập nhật...]*

### Chức năng:
- Quản lý hệ thống: Khởi tạo vi điều khiển, xung nhịp ngoại vi, hiển thị màn hình khởi động.
- Quản lý hiển thị HMI: Vẽ các khung card thông số, widget nút bấm, đồng hồ thời gian thực.
- Quản lý tương tác: Quét và giải mã tọa độ cảm ứng 12-bit kèm bộ lọc nhiễu trung vị; quét phím bấm vật lý kèm chống rung.
- Quản lý phụ tải: Điều khiển đóng ngắt rơ-le cách ly quang an toàn.
- *+ Quản lý và xử lý dữ liệu cảm biến: [Nội dung đang được cập nhật...]*

### Hành vi của hệ thống:

#### Luồng điều khiển:
1. Khi bật nguồn, hệ thống thực thi Reset Handler, thiết lập System Clock $72\text{ MHz}$, khởi tạo ngoại vi GPIO, SPI1, SPI2.
2. Gửi chuỗi lệnh khởi tạo ILI9341, hiển thị Splash Screen chào mừng trong 1.5 giây.
3. Chuyển sang vẽ khung giao diện Dashboard chính (tiêu đề, khung card, nút bấm ảo).
4. Vòng lặp chính điều phối tuần tự:
   - Mỗi $10\text{ ms}$: Quét cảm ứng và lọc rung nút nhấn.
   - Mỗi $500\text{ ms}$: Cập nhật giá trị hiển thị lên các ô thông số trên màn hình.
   - Khi có sự kiện Touch hoặc Nút bấm: Đảo trạng thái Relay và cập nhật giao diện trong $< 15\text{ ms}$.
- *+ Chu kỳ luồng điều khiển thu thập cảm biến: [Nội dung đang được cập nhật...]*

#### Luồng dữ liệu và tín hiệu:
Dữ liệu đo đạc từ các hàm thư viện cảm biến (dưới dạng các số nguyên/thực cho nhiệt độ, độ ẩm, độ sáng và logic bool cho chuyển động) được truyền trực tiếp đến khối HMI để xuất ra màn hình TFT và phục vụ hiển thị.
- *+ Giải thuật xử lý tín hiệu nội bộ cảm biến: [Nội dung đang được cập nhật...]*

#### Yêu cầu thời gian và ràng buộc:

| Thông số kỹ thuật | Giá trị tính toán | Ràng buộc Datasheet | Tần số / Chu kỳ | Kết luận kỹ thuật |
| :--- | :---: | :---: | :---: | :--- |
| **Chu kỳ xung nhịp SPI1 TFT** | $55.5\text{ ns}$ | $T_{\text{SCYCW}} \ge 100\text{ ns}$ (nominal) | $18\text{ MHz}$ (PCLK2/4) | Đạt tốc độ cao ổn định trên thực tế |
| **Vẽ quét toàn màn hình** | $68.3\text{ ms}$ | Không trễ quá $100\text{ ms}$ | $14.6\text{ FPS}$ | Loại bỏ hiện tượng nhấp nháy toàn màn |
| **Cập nhật thông số ROI** | $2.34\text{ ms}$ | Không gián đoạn luồng | $427\text{ Hz}$ | Số nhảy tức thời, siêu mượt |
| **Chu kỳ xung nhịp SPI2 Touch** | $444\text{ ns}$ | $T_{\text{CLK}} \ge 400\text{ ns}$ (XPT2046) | $2.25\text{ MHz}$ (PCLK1/16) | Đúng chuẩn tuyệt đối datasheet |
| **Thời gian đáp ứng Relay** | $8.5\text{ ms}$ | $t_{\text{transfer}} \le 15\text{ ms}$ | Tức thời | Đảm bảo an toàn cách ly quang |

- *+ Ràng buộc thời gian khối cảm biến: [Nội dung đang được cập nhật...]*

#### Sơ đồ trạng thái:
- Trạng thái 0 (Init): Khởi tạo clock 72MHz, SPI1, SPI2, GPIO.
- Trạng thái 1 (Splash): Vẽ màn hình chờ khởi động trong 1.5 giây.
- Trạng thái 2 (Dashboard): Trạng thái giám sát chính, hiển thị giá trị nhận được và lắng nghe thao tác.
- Trạng thái 3 (Action): Đảo trạng thái Relay và cập nhật giao diện khi có chạm cảm ứng hoặc nhấn nút.
- *+ Trạng thái xử lý cảm biến: [Nội dung đang được cập nhật...]*

---

## III. LỰA CHỌN LINH KIỆN:

### Bộ nhớ:
- **Bài toán Framebuffer:**  
  Một frame màn hình màu $240 \times 320$ RGB565 (16-bit) chiếm $153.600\text{ Bytes} = 150\text{ KB}$. Vi điều khiển STM32F103C8T6 chỉ có $20\text{ KB}$ SRAM nên không thể tạo toàn bộ Framebuffer.
- **Giải pháp Burst Buffer 512B:**  
  Sử dụng kỹ thuật Line-Buffer / ROI Burst-Buffer kích thước cố định **512 Bytes** (256 pixels), chỉ chiếm $2.5\%$ tổng SRAM (512B / 20KB), để dành $97.5\%$ SRAM cho hệ thống.
- **Tính toán bộ nhớ Flash:**  
  Mã nguồn Driver và HMI chiếm ~ $18\text{ KB}$; Bảng Font chữ chiếm ~ $6\text{ KB}$. Tổng dung lượng Flash sử dụng ước tính ~ $28\text{ KB} / 64\text{ KB}$ (đạt $44\%$ dung lượng Flash, hệ số an toàn cao).
- *+ Dự trù bộ nhớ cho khối cảm biến: [Nội dung đang được cập nhật...]*

### Tên sản phẩm chọn để mua:

#### 1. Màn hình LCD & Cảm ứng:
- Độ phân giải: $320 \times 240$ pixel, giao tiếp SPI, nguồn $3.3\text{V}/5\text{V}$.
- IC điều khiển hiển thị: ILI9341 (chạy SPI1 ở tốc độ $18\text{ MHz}$).
- IC giải mã cảm ứng điện trở: XPT2046 12-bit ADC (chạy SPI2 ở tốc độ $2.25\text{ MHz}$).

#### 2. Vi điều khiển trung tâm (MCU):
- Bo mạch vi điều khiển STM32F103C8T6 (ARM Cortex-M3, $72\text{ MHz}$, $64\text{ KB}$ Flash, $20\text{ KB}$ SRAM).

#### 3. Khối chấp hành Relay & Nút bấm:
- Module Relay 5V tích hợp Optocoupler cách ly quang PC817 chống nhiễu.
- Nút bấm Tactile switch kèm mạch dập xung.

#### 4. Khối cảm biến môi trường:
- *+ Danh mục cảm biến lựa chọn: [Nội dung đang được cập nhật...]*

---

## IV. ĐẶC TẢ PHẦN MỀM (SOFTWARE SPECIFICATION)

Phần mềm hệ thống được tổ chức theo kiến trúc phân tầng 4 lớp:
- **Layer 4 - Application Layer:** Chương trình chính `main.c`, máy trạng thái hiển thị Dashboard, xử lý sự kiện tương tác.
- **Layer 3 - Service / Middleware Layer:** Thư viện quản lý widget `hmi_manager.c`, bộ lọc cảm ứng trung vị `touch_filter.c`, trình lọc rung nút nhấn `button_debounce.c`.
- **Layer 2 - Hardware Abstraction Layer (HAL / Drivers):** Driver `ili9341.c` (vẽ đồ họa, tối ưu burst buffer 512B), driver `xpt2046.c`, driver `relay.c`.
- **Layer 1 - Microcontroller BSP:** Cấu hình thanh ghi GPIO, SPI1, SPI2, Timer SysTick.
- *+ Kiến trúc phần mềm khối cảm biến: [Nội dung đang được cập nhật...]*

---

## V. ĐẶC TẢ KIỂM THỬ (TEST SPECIFICATION)

Bảng ma trận kiểm thử hệ thống:

| Mã Test Case | Nội dung kiểm thử | Quy trình thực hiện | Tiêu chí Đạt (Pass Criteria) |
| :---: | :--- | :--- | :--- |
| **TC_01** | Kiểm tra hiển thị toàn màn hình TFT | Gửi lệnh khởi tạo SPI1 18MHz, xóa và vẽ full màu nền | Màn hình sáng đều, không sọc nhiễu, thời gian vẽ $\le 70\text{ ms}$ |
| **TC_02** | Kiểm tra cập nhật thông số cục bộ | Thay đổi giá trị số trên widget, gọi hàm vẽ vùng ROI | Số nhảy mượt mà không nhấp nháy, thời gian vẽ $\le 3\text{ ms}$ |
| **TC_03** | Kiểm tra độ nhạy và tọa độ cảm ứng | Chạm vào 4 góc và tâm màn hình cảm ứng | Tọa độ trả về sai lệch $\le 3\text{ pixel}$ so với vị trí chạm |
| **TC_04** | Kiểm tra đóng ngắt Relay phụ tải | Nhấn nút ảo trên màn hình hoặc nút bấm cơ | Relay đóng/ngắt tức thời, tiếng tiếp điểm rõ, đèn tải sáng |
| **TC_05** | Kiểm tra khối cảm biến | [Quy trình kiểm thử cảm biến] | *[Nội dung đang được cập nhật...]* |

---
