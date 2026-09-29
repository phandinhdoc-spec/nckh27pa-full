# BÁO CÁO KỸ THUẬT: THIẾT BỊ ĐEO ESP32-S3 PHẦN CỨNG THỰC TẾ
## Dự án: Thiết bị đeo hỗ trợ phát hiện té ngã và cảnh báo khẩn cấp (NCKH27PA / FallSafe)

---

## 1. Giới thiệu tổng quan sản phẩm thực tế

Thiết bị là module đeo thắt lưng siêu nhỏ gọn (Wearable Sensor Node), được thiết kế tối giản phần cứng theo hướng đo lường và phát hiện té ngã tự động hoàn toàn. Thiết bị sử dụng vi điều khiển **ESP32-S3 Super Mini**, cụm cảm biến đo chuyển động và khí áp, cấp nguồn qua mạch sạc pin LiPo **IP5306**.

Toàn bộ các tác vụ tương tác như đếm ngược cảnh báo, hủy báo giả, gọi điện thoại cấp cứu và gửi SMS tọa độ GPS được chuyển giao hoàn toàn cho **ứng dụng Android FallSafe** qua kết nối không dây **Bluetooth Low Energy (BLE)**. Nhờ đó, phần cứng thiết bị đeo không cần nút bấm cơ học hay còi hú cồng kềnh, giảm thiểu tối đa kích thước, trọng lượng và nguy cơ hỏng hóc cơ học khi va đập.

```
+-----------------------------------------------------------------------------------+
|                        HỆ THỐNG PHẦN CỨNG THỰC TẾ THIẾT BỊ ĐEO                    |
|                                                                                   |
|  +--------------------+        ĐƯỜNG NGUỒN (5V / GND)         +----------------+  |
|  |  Pin LiPo 3.7V     |-----> [ Mạch Sạc Nguồn IP5306 ] ----->| OUT+ (5V DC)   |  |
|  |  (BAT+ / BAT-)     |       (Không hàn dây tín hiệu)        | OUT- (GND Mass)|  |
|  +--------------------+                                       +-------+--------+  |
|                                                                       |           |
|            +--------------------------+-------------------------------+           |
|            |                          |                               |           |
|            v (5V, GND)                v (5V, GND)                     v (5V, GND) |
|  +-------------------+      +-------------------+      +-----------------------+  |
|  | Cảm biến MPU-6050 |      | Cảm biến MS5611   |      |  ESP32-S3 Super Mini  |  |
|  | (Module GY-521)   |      | (Module GY-63)    |      |                       |  |
|  |                   |      |                   |      | - Dual-core LX7       |  |
|  |   SDA: GPIO 8 <---|------|-------------------|----->| - Bus I2C0: SDA 8,SCL 9| |
|  |   SCL: GPIO 9 <---|------|-------------------|----->| - Bus I2C1: SDA 6,SCL 7| |
|  |                   |      |   SDA: GPIO 6 <---|----->| - FSM tự động 100 Hz  |  |
|  |                   |      |   SCL: GPIO 7 <---|----->| - BLE GATT Server     |  |
|  +-------------------+      +-------------------+      +-----------+-----------+  |
|                                                                    |              |
+--------------------------------------------------------------------|--------------+
                                                                     | BLE 5.0
                                                                     v
                                                      +-----------------------------+
                                                      | ỨNG DỤNG ANDROID (FallSafe) |
                                                      | - Nhận telemetry / cảnh báo |
                                                      | - Đếm ngược 10s hủy báo giả |
                                                      | - Tự động gọi điện SOS      |
                                                      | - Tự động gửi SMS kèm GPS   |
                                                      +-----------------------------+
```

---

## 2. Danh mục linh kiện phần cứng có mặt trong hệ thống thực tế

Toàn bộ hệ thống phần cứng chỉ bao gồm đúng **4 thành phần chính** sau:

| STT | Tên linh kiện | Tên module | Vai trò trong hệ thống thực tế |
| :---: | :--- | :--- | :--- |
| 1 | **Vi điều khiển** | **ESP32-S3 Super Mini** | Trung tâm xử lý: đọc dữ liệu từ 2 cảm biến qua 2 bus I2C độc lập, chạy thuật toán phát hiện rơi - va đập - bất động, truyền telemetry và sự kiện qua BLE. |
| 2 | **Cảm biến chuyển động (IMU 6 trục)** | **MPU-6050 (GY-521)** | Đo gia tốc 3 trục ($a_x, a_y, a_z$) và vận tốc góc 3 trục ($\omega_x, \omega_y, \omega_z$) ở tần số lấy mẫu 100 Hz. |
| 3 | **Cảm biến áp suất khí quyển** | **MS5611-01BA03 (GY-63)** | Đo áp suất khí quyển $P$, nhiệt độ $T$ và tính toán biến thiên độ cao tương đối $\Delta h$ phục vụ xác thực cú ngã tiếp đất. |
| 4 | **Mạch quản lý nguồn & sạc** | **IP5306** | Mạch nguồn độc lập: sạc pin LiPo qua cổng sạc, bảo vệ pin, kích áp lên 5V tại 2 chân `OUT+` và `OUT-` cấp toàn bộ hệ thống. |
| 5 | **Nguồn cấp năng lượng** | **Pin LiPo 3.7V (700mAh)** | Nối vào 2 chân `BAT+` và `BAT-` của mạch IP5306. |

> **Lưu ý đặc biệt về ngoại vi:**
> - **KHÔNG CÓ nút bấm SOS phần cứng.**
> - **KHÔNG CÓ nút hủy (Cancel) phần cứng.**
> - **KHÔNG CÓ còi báo động (Buzzer).**
> - **Đèn LED chỉ thị:** Chỉ sử dụng các đèn LED có sẵn tích hợp trên các module (LED nguồn trên bo mạch ESP32-S3 Super Mini, LED nguồn trên module GY-521, LED trên GY-63 và dải 4 LED báo mức pin trên mạch sạc IP5306).

---

## 3. Sơ đồ đấu dây và hàn chết phần cứng thực tế

### 3.1. Sơ đồ cấp nguồn (Power Rail)
Mạch IP5306 đóng vai trò là khối cấp nguồn phần cứng thuần túy, **hoàn toàn không hàn bất kỳ dây tín hiệu nào với ESP32-S3**:
- Pin LiPo: Cực dương `(+)` nối `BAT+`, cực âm `(-)` nối `BAT-` của mạch IP5306.
- Đầu ra 5V từ IP5306 gồm 2 chân `OUT+` (5V) và `OUT-` (GND) được đấu song song cấp cho 3 module:
  - `OUT+` (5V) $\longrightarrow$ Chân `VCC / 5V` của **ESP32-S3 Super Mini**, chân `VCC` của **GY-521** và chân `VCC` của **GY-63** *(Cả 2 module cảm biến đều tích hợp IC hạ áp LDO trên bo để nhận nguồn 5V an toàn)*.
  - `OUT-` (GND) $\longrightarrow$ Chân `GND` của **ESP32-S3 Super Mini**, chân `GND` của **GY-521** và chân `GND` của **GY-63** (Tạo mass chung cho toàn hệ thống).

### 3.2. Sơ đồ hàn chân tín hiệu thực tế (Signal Pinout)
Toàn bộ phần tín hiệu trong hệ thống chỉ gồm **4 dây hàn chết** kết nối giữa 2 cảm biến và ESP32-S3 thông qua 2 bus I2C phần cứng độc lập:

| Thiết bị cảm biến | Chân cảm biến | Chân ESP32-S3 hàn thực tế | Tên ngoại vi trên MCU | Cấu hình & Địa chỉ I2C |
| :--- | :---: | :---: | :---: | :--- |
| **MPU-6050 (GY-521)** | **SDA** | **GPIO 8** | I2C0_SDA | Bus I2C0 phần cứng, tần số 100 kHz. |
| **MPU-6050 (GY-521)** | **SCL** | **GPIO 9** | I2C0_SCL | Bus I2C0 phần cứng, tần số 100 kHz. |
| **MS5611 (GY-63)** | **SDA** | **GPIO 6** | I2C1_SDA | Bus I2C1 phần cứng, tần số 100 kHz. |
| **MS5611 (GY-63)** | **SCL** | **GPIO 7** | I2C1_SCL | Bus I2C1 phần cứng, tần số 100 kHz. |

```
               SƠ ĐỒ ĐẤU NỐI CHI TIẾT TRONG THỰC TẾ

     +-----------------------+
     |   PIN LIPO (3.7V)     |
     |   (+)            (-)  |
     +----+--------------+---+
          |              |
          v (BAT+)       v (BAT-)
     +-----------------------------------------+
     |        MẠCH SẠC NGUỒN IP5306            |
     | (Không nối dây dữ liệu I2C với ESP32)   |
     |        OUT+ (5V)       OUT- (GND)       |
     +-----------+----------------+------------+
                 |                |
     +-----------+----------------+-------------------------------+
     |                            |                               |
     v (5V)                       v (5V)                          v (5V)
+----+---------------+       +----+---------------+          +----+---------------+
| Cảm biến MPU-6050  |       | Cảm biến MS5611    |          | ESP32-S3           |
| (Module GY-521)    |       | (Module GY-63)     |          | Super Mini         |
|                    |       |                    |          |                    |
| VCC: Nối OUT+ (5V) |       | VCC: Nối OUT+ (5V) |          | 5V/VBUS: Nối OUT+  |
| GND: Nối OUT- (GND)|       | GND: Nối OUT- (GND)|          | GND    : Nối OUT-  |
|                    |       |                    |          |                    |
| SDA -------------->|-------|--------------------|--------->| GPIO 8 (I2C0 SDA)  |
| SCL -------------->|-------|--------------------|--------->| GPIO 9 (I2C0 SCL)  |
|                    |       | SDA -------------->|--------->| GPIO 6 (I2C1 SDA)  |
|                    |       | SCL -------------->|--------->| GPIO 7 (I2C1 SCL)  |
+--------------------+       +--------------------+          +--------------------+
```

---

## 4. Nhiệm vụ và Chức năng của từng cảm biến

### 4.1. Cảm biến MPU-6050 (GY-521)
- **Tần số trích xuất:** 100 Hz (đọc chu kỳ cố định 10 ms).
- **Giao tiếp:** I2C Bus 0 (SDA: GPIO 8, SCL: GPIO 9).
- **Nhiệm vụ:**
  1. **Đo gia tốc tức thời ($a_x, a_y, a_z$):** Tính độ lớn gia tốc tổng hợp $a = \sqrt{a_x^2 + a_y^2 + a_z^2}$.
  2. **Phát hiện rơi tự do (Free-fall):** Nhận diện pha rơi khi $a < 4.9 \text{ m/s}^2$ liên tục trong khoảng $\ge 80 \text{ ms}$.
  3. **Ghi nhận va đập (Impact):** Bắt xung lực va chạm mặt đất khi $a \ge 25 \text{ m/s}^2$ kết hợp góc xoay cơ thể $\omega > 120^\circ/s$.
  4. **Kiểm tra trạng thái bất động (Inactivity):** Sau va chạm, kiểm tra người bệnh nằm yên với $|a - 9.81| \le 1.0 \text{ m/s}^2$ duy trì liên tục $\ge 1000 \text{ ms}$.

### 4.2. Cảm biến khí áp MS5611 (GY-63)
- **Độ phân giải:** 24-bit ADC, đo áp suất từ 10 đến 1200 mbar với độ nhạy độ cao đạt tới 10–20 cm.
- **Giao tiếp:** I2C Bus 1 độc lập (SDA: GPIO 6, SCL: GPIO 7).
- **Nhiệm vụ:**
  1. **Xác định mốc áp suất ban đầu ($P_0$):** Tính trung bình áp suất chuẩn khi khởi động để làm gốc đo độ cao.
  2. **Đo biến thiên độ cao tương đối ($\Delta h$):** Khi ngã từ tư thế đứng xuống sàn, độ cao tại vị trí thắt lưng sụt giảm nhanh ($\Delta h \le -0.40 \text{ m}$).
  3. **Loại bỏ báo động giả (False-alarm filtering):** Là bằng chứng vật lý độc lập kiểm chứng cú ngã; ngăn chặn báo nhầm trong các trường hợp vỗ mạnh vào thiết bị, dậm chân hay nhảy lên rồi đứng thẳng.

### 4.3. Mạch sạc nguồn IP5306
- **Nhiệm vụ:** Cung cấp nguồn điện ổn định 5V DC liên tục cho cả 3 bo mạch từ pin LiPo 3.7V; tự ngắt khi pin yếu để bảo vệ chống xả cạn kiệt; sạc pin qua cổng sạc Type-C/Micro-USB và hiển thị mức pin qua 4 đèn LED có sẵn trên mạch nguồn.
- **Đặc điểm thực tế:** Không tham gia giao tiếp I2C với vi điều khiển, loại bỏ hoàn toàn các lỗi xung đột bus hoặc treo bus liên quan đến quản lý năng lượng.

---

## 5. Nhiệm vụ và Chức năng tổng thể của sản phẩm ESP32

Sản phẩm là một thiết bị đo đạc và xử lý tại biên (Edge Device) hoạt động hoàn toàn tự động với các nhiệm vụ chính:

1. **Khởi động và tự kiểm tra (Self-test):**
   - Tự động kiểm tra tính sẵn sàng của 2 cảm biến MPU-6050 trên Bus 0 và MS5611 trên Bus 1.
   - Hiệu chuẩn điểm 0 áp suất khí quyển $P_0$ làm mốc đo cao độ.

2. **Vận hành máy trạng thái phát hiện té ngã (FSM 100 Hz):**
   - Thu thập liên tục mỗi 10 ms mà không cần thao tác bấm từ người dùng.
   - Tự động chuyển đổi các trạng thái:
     - `MONITORING`: Giám sát thông thường.
     - `SUSPECTED`: Phát hiện rơi tự do.
     - `VERIFYING`: Đã xảy ra va chạm, chờ kiểm tra bất động và sụt giảm độ cao.
     - `ALERT`: Xác nhận ngã thật (`FALL_CONFIRMED`).

3. **Truyền thông không dây Bluetooth Low Energy (BLE):**
   - Đóng vai trò BLE GATT Server phát sóng tên thiết bị `FALLSAFE-xxxx`.
   - Bắn trực tiếp dữ liệu chuyển động (Telemetry stream) và thông điệp cảnh báo sự kiện ngã sang điện thoại Android.
   - Nhận lệnh cấu hình bộ ngưỡng (Profile) từ ứng dụng để điều chỉnh độ nhạy mà không cần phải nạp lại code qua dây cáp.

4. **Phối hợp đầu - cuối với ứng dụng di động (Android FallSafe):**
   - Vì thiết bị không mang nút bấm và còi báo, toàn bộ quy trình phản hồi sau ngã được điện thoại thực hiện:
     - Phát âm thanh cảnh báo lớn và hiển thị **bộ đếm ngược 10 giây**.
     - Nếu người dùng cử động bình thường hoặc báo nhầm, bấm nút **HỦY** trên màn hình điện thoại.
     - Nếu sau 10 giây người dùng bất tỉnh không hủy, ứng dụng tự động thực hiện **cuộc gọi điện thoại khẩn cấp** tới người thân ưu tiên và **gửi tin nhắn SMS** kèm tọa độ định vị GPS cùng đường link Google Maps cứu hộ.
5. Toàn bộ hệ thống phần mềm được xây dựng cách lấy dữ liệu là đọc trực tiếp giá trị trên thanh ghi ở từng địa chỉ, không sử dụng các thư viện có sẵn trên Arduino IDE. 