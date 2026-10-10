# BẢNG GIẢI THÍCH THUẬT NGỮ KHOA HỌC & KỸ THUẬT
## Dự án: Thiết bị cảnh báo hỗ trợ người có nguy cơ đột quỵ, tai biến mạch máu não
*(Dành riêng cho học sinh THCS ôn luyện, hiểu sâu bản chất và tự tin trả lời Ban giám khảo)*

---

## 📌 HƯỚNG DẪN DÀNH CHO CÁC EM KHI ĐI THI
> Khi Ban giám khảo (thầy cô) hỏi về bất kỳ từ tiếng Anh hay thuật ngữ kỹ thuật nào trong bài báo cáo, các em **hãy bình tĩnh trả lời theo 3 bước**:
> 1. **Dịch nghĩa tiếng Việt mộc mạc** (từ đó nghĩa là gì).
> 2. **Chức năng trong sản phẩm của nhóm** (nó dùng để làm gì trong máy của mình).
> 3. **Ví dụ thực tế dễ hiểu** (giúp thầy cô thấy các em thật sự hiểu bản chất chứ không học vẹt).

---

## MỤC LỤC CÁC NHÓM THUẬT NGỮ
1. [Linh kiện & Phần cứng thiết bị](#1-linh-kiện--phần-cứng-thiết-bị)
2. [Khái niệm Vật lý & Thuật toán phát hiện ngã](#2-khái-niệm-vật-lý--thuật-toán-phát-hiện-ngã)
3. [Phần mềm, Kết nối & Viễn thông](#3-phần-mềm-kết-nối--viễn-thông)
4. [Máy chủ & Cơ chế an toàn dữ liệu](#4-máy-chủ--cơ-chế-an-toàn-dữ-liệu)
5. [Đo lường, Đánh giá & Kiểm thử](#5-đo-lường-đánh-giá--kiểm-thử)

---

## 1. LINH KIỆN & PHẦN CỨNG THIẾT BỊ

### 1.1. MCU (Microcontroller Unit) / ESP32-S3 Super Mini
- **Nghĩa tiếng Việt:** Vi điều khiển (bộ não điện tử nhỏ gọn).
- **Giải thích gần gũi:** Giống như một chiếc máy tính thu nhỏ bằng đốt ngón tay, có sẵn bộ xử lý, bộ nhớ và các chân cắm điện tử để nhận tín hiệu từ cảm biến và điều khiển các bộ phận khác.
- **Trong đề tài:** Nhóm dùng vi điều khiển **ESP32-S3 Super Mini** làm "bộ não" gắn ở thắt lưng, chuyên đọc dữ liệu chuyển động và tính toán xem người dùng có bị ngã hay không.
- **Trả lời giám khảo:** *"Thưa thầy/cô, MCU là vi điều khiển đóng vai trò bộ não trung tâm của thiết bị đeo, nhận lệnh và xử lý dữ liệu cảm biến."*

### 1.2. IMU (Inertial Measurement Unit)
- **Nghĩa tiếng Việt:** Thiết bị đo quán tính.
- **Giải thích gần gũi:** Là cảm biến chuyên đo xem một vật đang chuyển động nhanh chậm ra sao (gia tốc) và đang quay/nghiêng như thế nào (tốc độ góc).
- **Trong đề tài:** Khối IMU gồm cảm biến MPU6050 giúp thiết bị biết người đeo đang đứng, ngồi, đi lại hay bị vấp ngã.

### 1.3. MPU6050 (GY-521)
- **Nghĩa tiếng Việt:** Cảm biến gia tốc và con quay hồi chuyển 6 trục.
- **Giải thích gần gũi:** Con chip này gồm 2 phần:
  1. *Gia tốc kế (3 trục X, Y, Z):* Cảm nhận lực đẩy, rung lắc hoặc rơi xuống.
  2. *Con quay hồi chuyển (Gyroscope):* Cảm nhận độ xoay tròn của cơ thể (lộn nhào, xoay người).
- **Trong đề tài:** Dùng để phát hiện cú rơi tự do và cú va chạm mạnh với mặt sàn khi ngã.

### 1.4. MS5611 / GY-63 (Barometric Pressure Sensor)
- **Nghĩa tiếng Việt:** Cảm biến áp suất khí quyển (khí áp kế).
- **Giải thích gần gũi:** Trong không khí, càng lên cao áp suất càng giảm, càng xuống thấp áp suất càng tăng. Cảm biến này đo áp suất không khí cực kỳ nhạy, từ đó suy ra sự thay đổi độ cao (chính xác đến vài chục xentimét).
- **Trong đề tài:** Khi ngã, thắt lưng đột ngột hạ thấp xuống đất từ $0,4\text{ m}$ đến $0,8\text{ m}$. Cảm biến khí áp giúp phân biệt giữa **ngã xuống sàn** (độ cao tụt mạnh) với việc **ngồi xuống ghế** (độ cao giảm ít và chậm hơn).

### 1.5. Giao tiếp I2C (I2C Bus: SDA, SCL)
- **Nghĩa tiếng Việt:** Chuẩn giao tiếp 2 dây giữa các vi mạch.
- **Giải thích gần gũi:** Là "con đường giao thông" để vi điều khiển ESP32 trò chuyện với cảm biến chỉ bằng 2 sợi dây: dây **SDA** (truyền dữ liệu) và dây **SCL** (phát nhịp đồng hồ đồng bộ).
- **Trong đề tài:** ESP32 đọc dữ liệu từ cảm biến MPU6050 và MS5611 thông qua đường truyền I2C với tốc độ $400\text{ kHz}$ ($400.000$ nhịp/giây).

### 1.6. Mạch sạc IP5306 & Pin LiPo (Lithium Polymer)
- **Nghĩa tiếng Việt:** Mạch quản lý nguồn và pin sạc dẹp.
- **Giải thích gần gũi:** Pin LiPo là loại pin mỏng, nhẹ thường thấy trong điện thoại di động (dung lượng $700\text{ mAh}$). Mạch IP5306 vừa sạc pin khi cắm cổng Type-C, vừa chống nổ/chống xả cạn kiệt pin, vừa nâng điện áp của pin ($3,7\text{ V}$) lên $5\text{ V}$ ổn định cho vi điều khiển hoạt động.

### 1.7. RAM (Random Access Memory)
- **Nghĩa tiếng Việt:** Bộ nhớ tạm thời của máy tính/vi điều khiển.
- **Giải thích gần gũi:** Giống như một cái "bảng nháp". Dữ liệu ghi trên đó được đọc rất nhanh, nhưng khi tắt điện sẽ bị xóa sạch.
- **Trong đề tài:** ESP32 dùng $40\text{ KB}$ bộ nhớ RAM để lưu tạm 10 giây dữ liệu cảm biến gần nhất mà không cần ghi vào thẻ nhớ.

## 2. KHÁI NIỆM VẬT LÝ & THUẬT TOÁN PHÁT HIỆN NGÃ

> **Lưu ý quan trọng cho học sinh:** Thuật toán phát hiện ngã bằng cảm biến gia tốc theo 3 pha (rơi tự do $\rightarrow$ va đập $\rightarrow$ nằm yên) không phải do nhóm tự nghĩ ra từ con số 0, mà **được kế thừa từ các công trình nghiên cứu kinh điển và sách chuyên khảo quốc tế** (công trình của nhà khoa học A. K. Bourke năm 2007 và sách chuyên khảo *Wearable Sensors* của GS. Edward Sazonov, NXB Elsevier). Đóng góp của nhóm là: áp dụng nguyên lý khoa học đó, tinh chỉnh các ngưỡng phù hợp với cơ thể người cao tuổi Việt Nam và lập trình cài đặt thành công lên vi điều khiển ESP32-S3 giá rẻ.

### 2.1. Gia tốc ($a$) và Gia tốc trọng trường ($g$)
- **Nghĩa tiếng Việt:**
  - *Gia tốc ($a$):* Đại lượng đo sự thay đổi vận tốc nhanh hay chậm (đơn vị: $\text{m/s}^2$).
  - *Gia tốc trọng trường ($g$):* Gia tốc do lực hút của Trái Đất gây ra, giá trị chuẩn khoảng $9,81\text{ m/s}^2$ (thường gọi là $1g$).
- **Giải thích gần gũi:** Khi đứng yên trên mặt đất, cơ thể ta luôn chịu lực ép của trọng trường tương đương $9,81\text{ m/s}^2$. Nếu nhảy từ trên cao xuống, trong lúc rơi ta sẽ thấy nhẹ bẫng (gia tốc tụt giảm).

### 2.2. Rơi tự do (Free Fall)
- **Nghĩa tiếng Việt:** Trạng thái rơi dưới tác dụng của trọng lực mà không có điểm tựa.
- **Giải thích gần gũi:** Khi một người bị trượt chân hoặc hẫng người ngã xuống, cơ thể mất điểm tựa trong tích tắc, cảm biến đo được gia tốc tổng giảm mạnh xuống dưới $4,9\text{ m/s}^2$ (tương đương mất đi hơn một nửa trọng lực).
- **Trong đề tài:** Đây là dấu hiệu đầu tiên nghi ngờ cú ngã bắt đầu.

### 2.3. Đỉnh va đập / Va chạm mạnh (Impact)
- **Nghĩa tiếng Việt:** Cú va chạm đột ngột giữa cơ thể và mặt phẳng cứng.
- **Giải thích gần gũi:** Khi thân người đập xuống sàn nhà, cơ thể dừng lại đột ngột khiến gia tốc tăng vọt lên rất lớn ($a \ge 25\text{ m/s}^2$, gấp hơn 2,5 lần trọng lực bình thường).
- **Trong đề tài:** Đây là bằng chứng thứ hai khẳng định đã xảy ra cú va đập nguy hiểm.

### 2.4. Bất động sau va đập (Inactivity / Stillness)
- **Nghĩa tiếng Việt:** Trạng thái nằm yên, không cử động sau cú ngã.
- **Giải thích gần gũi:** Nếu chỉ là nhảy lò cò hoặc vô tình làm rơi chiếc gối, vật thể sẽ tiếp tục nảy lên hoặc người đó sẽ đứng dậy đi tiếp ngay. Nhưng nếu người bệnh bị đột quỵ/chấn thương ngất đi, cơ thể sẽ nằm bất động trên sàn (gia tốc trở về bình yên quanh mức $9,81\text{ m/s}^2$).
- **Trong đề tài:** Thuật toán bắt buộc phải thấy người nằm yên liên tục ít nhất 1 giây ($1000\text{ ms}$) thì mới kết luận là **ngã thật** để tránh báo động giả.

### 2.5. Tốc độ góc (Angular Velocity - $\omega$)
- **Nghĩa tiếng Việt:** Tốc độ xoay quanh một trục (đo bằng độ/giây).
- **Giải thích gần gũi:** Khi cúi người bình thường, người ta cúi từ từ. Nhưng khi bị ngã, thân trên lộn nhào hoặc xoay rất nhanh (vượt quá 120 độ/giây).
- **Trong đề tài:** Dùng làm bằng chứng phụ để tăng độ chắc chắn của thuật toán.

### 2.6. Tần số lấy mẫu (Sample Rate - $100\text{ Hz}$)
- **Nghĩa tiếng Việt:** Số lần đọc dữ liệu trong một giây.
- **Giải thích gần gũi:** $100\text{ Hz}$ nghĩa là cứ 1 giây, vi điều khiển đọc cảm biến đúng **100 lần** (tức cách nhau $10\text{ ms} = 0,01$ giây một lần).
- **Vì sao cần $100\text{ Hz}$?** Vì khoảnh khắc người chạm đất diễn ra cực kỳ nhanh (chỉ từ 20 đến 50 phần nghìn giây). Nếu đọc chậm (ví dụ 10 lần/giây), máy sẽ bị "chớp mắt" bỏ lỡ khoảnh khắc va đập.

### 2.7. Máy trạng thái (Finite State Machine - FSM)
- **Nghĩa tiếng Việt:** Mô hình điều khiển theo từng nấc trạng thái rõ ràng.
- **Giải thích gần gũi:** Giống như đèn giao thông (chỉ có thể là Đỏ $\rightarrow$ Xanh $\rightarrow$ Vàng) hoặc quá trình đun nước (Nước nguội $\rightarrow$ Đang sôi $\rightarrow$ Đã tắt). Thiết bị không thể nhảy cóc lung tung mà phải lần lượt qua từng bước.
- **Trong đề tài:** Thiết bị có 7 trạng thái:
  1. `BOOT_SELF_TEST`: Bật nguồn, tự kiểm tra xem linh kiện có bị lỏng không.
  2. `CALIBRATING`: Đứng yên vài giây để lấy số 0 chuẩn cho cảm biến.
  3. `MONITORING`: Canh chừng, theo dõi bình thường.
  4. `SUSPECTED`: Nghi ngờ vì thấy rơi tự do.
  5. `VERIFYING`: Đang xác minh (chờ xem có va đập mạnh và nằm yên không).
  6. `LOCAL_ALERTING`: Đã xác nhận ngã, phát cảnh báo khẩn.
  7. `DEGRADED`: Trạng thái an toàn khi một cảm biến bị hỏng (máy vẫn chạy nhưng cảnh báo cho người dùng biết).

### 2.8. Bộ đệm trước sự kiện (Pre-event buffer)
- **Nghĩa tiếng Việt:** Hộp lưu trữ dữ liệu quá khứ.
- **Giải thích gần gũi:** Giống như camera hành trình trên xe ô tô: nó liên tục ghi đè 10 giây gần nhất. Khi tai nạn xảy ra, ta có thể mở lại đúng 10 giây trước đó để xem nạn nhân đi đứng thế nào trước lúc ngã.

---

## 3. PHẦN MỀM, KẾT NỐI & VIỄN THÔNG

### 3.1. Firmware
- **Nghĩa tiếng Việt:** Phần mềm điều khiển phần cứng (phần mềm nhúng).
- **Giải thích gần gũi:** Là đoạn mã lập trình (viết bằng ngôn ngữ C/C++ trên Arduino) được nạp vĩnh viễn vào bộ nhớ của bo mạch ESP32 để dạy cho vi mạch biết phải làm gì khi bật điện.
- **Khác gì với App?** App chạy trên điện thoại hoặc máy tính, còn firmware chạy trực tiếp trong con chip điện tử bé tí.

### 3.2. Telemetry (Dữ liệu đo từ xa)
- **Nghĩa tiếng Việt:** Luồng dữ liệu cảm biến gửi từ xa về máy theo dõi.
- **Giải thích gần gũi:** Cứ mỗi nhịp, chiếc hộp ở thắt lưng lại tự động "bắn" số đo gia tốc, góc nghiêng, độ cao, phần trăm pin qua Bluetooth về điện thoại để hiển thị đồ thị lên màn hình. Toàn bộ dòng số liệu đo từ xa này gọi là *telemetry*.

### 3.3. Bluetooth / BLE (Bluetooth Low Energy)
- **Nghĩa tiếng Việt:** Sóng Bluetooth năng lượng thấp (tiết kiệm pin).
- **Giải thích gần gũi:** Là công nghệ sóng không dây tầm ngắn (khoảng 10 mét) giống tai nghe Bluetooth nhưng tiêu tốn cực kỳ ít điện năng, giúp pin của thiết bị đeo dùng được lâu.

### 3.4. JSON (JavaScript Object Notation)
- **Nghĩa tiếng Việt:** Định dạng đóng gói dữ liệu dạng chữ đơn giản.
- **Giải thích gần gũi:** Là một quy ước chung để các máy tính khác nhau hiểu được nhau. Thay vì gửi một mớ số lộn xộn, máy sẽ đóng gói thành dòng chữ có tên rõ ràng:
  `{"a": 9.8, "pin": 85, "trang_thai": "BINH_THUONG"}`. Điện thoại đọc vào là hiểu ngay $a$ là gia tốc, $pin$ là phần trăm pin.

### 3.5. Profile (Bảng cấu hình hằng số)
- **Nghĩa tiếng Việt:** Hồ sơ cài đặt các ngưỡng phát hiện.
- **Giải thích gần gũi:** Mỗi người có cơ thể và thói quen khác nhau: người già đi lại chậm rãi cần ngưỡng nhạy hơn; thanh niên vận động mạnh cần ngưỡng cao hơn để không báo nhầm. *Profile* là một bảng lưu sẵn các số đo này trên điện thoại để người dùng chọn nhanh mà không cần nạp lại code cho con chip.

### 3.6. Kotlin & Jetpack Compose
- **Nghĩa tiếng Việt:** Ngôn ngữ lập trình và bộ công cụ vẽ giao diện ứng dụng Android hiện đại.
- **Giải thích gần gũi:** Kotlin là ngôn ngữ chính thức để viết ứng dụng điện thoại Android; còn Jetpack Compose là công cụ giúp thiết kế các nút bấm, đồ thị và bảng biểu đẹp mắt, mượt mà.

### 3.7. GPS & Vị trí tổng hợp (Fused Location)
- **Nghĩa tiếng Việt:** Hệ thống định vị vệ tinh toàn cầu và cơ chế dò vị trí thông minh.
- **Giải thích gần gũi:**
  - *GPS:* Bắt sóng trực tiếp từ các vệ tinh bay ngoài vũ trụ (rất chính xác ngoài trời, nhưng trong nhà sóng bị tường cản).
  - *Fused Location:* Cơ chế thông minh của Google, tự động kết hợp cả sóng GPS, sóng Wi-Fi xung quanh và trạm phát sóng điện thoại để tìm ra vị trí người bệnh nhanh nhất (chỉ mất 1–2 giây) ngay cả khi đang ở trong phòng kín.

### 3.8. Cuộc gọi SIM (`ACTION_CALL`) & Tin nhắn SMS
- **Nghĩa tiếng Việt:** Tự động quay số và gửi tin nhắn qua sóng nhà mạng di động (Viettel, Vinaphone, Mobifone).
- **Giải thích gần gũi:** Khi ngã, điện thoại tự động kích hoạt tính năng gọi trực tiếp cho người thân mà không cần người dùng chạm tay vào màn hình, đồng thời gửi tin nhắn SMS đính kèm đường link bản đồ Google Maps chỉ rõ vị trí đang bị ngã.
- **Ưu điểm lớn:** Chỉ cần có sóng di động là gọi và nhắn tin được ngay, **không cần có mạng Internet/4G/Wi-Fi**.

---

## 4. MÁY CHỦ & CƠ CHẾ AN TOÀN DỮ LIỆU

### 4.1. Node.js & Cơ sở dữ liệu SQLite
- **Nghĩa tiếng Việt:** Nền tảng máy chủ điều khiển và kho lưu trữ dữ liệu dạng tệp nhẹ.
- **Giải thích gần gũi:**
  - *Node.js:* Đóng vai trò máy tính trung tâm tiếp nhận thông tin từ các điện thoại gửi về.
  - *SQLite:* Là một cuốn "sổ tay điện tử" gọn nhẹ, lưu trữ lịch sử các lần té ngã, danh bạ người thân mà không cần cài đặt phần mềm máy chủ cồng kềnh.

### 4.2. Hàng đợi Outbox (Outbox Queue)
- **Nghĩa tiếng Việt:** Hòm thư chờ gửi đi.
- **Giải thích gần gũi:** Khi có cảnh báo, tin nhắn được xếp vào một hàng đợi giống như các lá thư bỏ vào hòm thư bưu điện. Nếu lỡ mất mạng đột ngột, lá thư vẫn nằm nguyên trong hòm; khi có mạng lại, hệ thống sẽ tự động gửi tiếp mà không sợ bị mất tin báo khẩn cấp.

### 4.3. Watchdog (Chó canh cổng)
- **Nghĩa tiếng Việt:** Bộ đếm thời gian giám sát chống treo máy.
- **Giải thích gần gũi:** Giống như một chú chó trung thành ngồi đếm nhịp. Cứ mỗi giây, chương trình phải báo cho "chú chó" biết là mình đang sống khỏe mạnh. Nếu chương trình bị đơ hay treo máy, "chú chó" watchdog sẽ lập tức bấm nút khởi động lại máy để hệ thống tiếp tục làm việc, không bị tê liệt.

### 4.4. Best-effort (Nỗ lực tối đa nhưng không phụ thuộc)
- **Nghĩa tiếng Việt:** Cơ chế hỗ trợ phụ, chạy độc lập.
- **Giải thích gần gũi:** Cuộc gọi SOS và tin nhắn SMS được thực hiện trực tiếp từ điện thoại nên người thân nhận được ngay lập tức. Việc gửi dữ liệu lên máy chủ chỉ là để lưu lại lịch sử theo dõi sau này; máy chủ có bị hỏng hay mất điện thì người nhà vẫn nhận được cuộc gọi và tin nhắn báo nạn.

---

## 5. ĐO LƯỜNG, ĐÁNH GIÁ & KIỂM THỬ

### 5.1. Độ nhạy (Sensitivity)
- **Nghĩa tiếng Việt:** Khả năng phát hiện đúng khi có ngã thật.
- **Công thức dễ hiểu:** $\text{Độ nhạy} = \frac{\text{Số lần phát hiện đúng}}{\text{Tổng số lần ngã thật}} \times 100\%$
- **Ví dụ:** Cho người thử ngã thật trên đệm 60 lần, thiết bị bắt đúng 57 lần $\rightarrow$ Độ nhạy đạt $95\%$.

### 5.2. Độ đặc hiệu (Specificity)
- **Nghĩa tiếng Việt:** Khả năng không báo nhầm khi sinh hoạt bình thường.
- **Công thức dễ hiểu:** $\text{Độ đặc hiệu} = \frac{\text{Số lần KHÔNG báo nhầm}}{\text{Tổng số lần sinh hoạt bình thường}} \times 100\%$
- **Ví dụ:** Cho người thử đứng lên, ngồi xuống, cúi nhặt đồ 60 lần, máy chỉ kêu nhầm 4 lần (56 lần chuẩn xác) $\rightarrow$ Độ đặc hiệu đạt $93,3\%$.

### 5.3. Báo giả (False Positive) & Bỏ sót (False Negative)
- **Báo giả:** Người bình thường không ngã nhưng máy lại tưởng là ngã và báo động làm phiền người nhà.
- **Bỏ sót:** Người bệnh bị ngã thật nhưng máy lại không nhận ra (nguy hiểm nhất).

### 5.4. Emulator (Trình giả lập)
- **Nghĩa tiếng Việt:** Điện thoại ảo chạy trên máy vi tính.
- **Giải thích gần gũi:** Là phần mềm trên máy tính mô phỏng y hệt một chiếc điện thoại Android thật, giúp nhóm học sinh thử nghiệm gọi điện thoại, gửi SMS và tạo cú ngã ảo mà không tốn tiền cước viễn thông.

### 5.5. Unit Test (Kiểm thử đơn vị)
- **Nghĩa tiếng Việt:** Các bài kiểm tra tự động từng phần nhỏ của phần mềm.
- **Giải thích gần gũi:** Giống như giáo viên chấm 175 câu hỏi trắc nghiệm tự động. Máy tính chạy qua 175 phép toán thử nghiệm; nếu tất cả đều ra kết quả đúng (đạt 100% PASS) thì chứng tỏ mã nguồn không bị lỗi logic.

---

## 💡 MẸO GHI ĐIỂM KHI TRẢ LỜI CÂU HỎI HỘI ĐỒNG GIÁM KHẢO
1. **Nếu giám khảo hỏi:** *"Tại sao các con không dùng đồng hồ thông minh như Apple Watch mà lại tự làm thiết bị đeo thắt lưng?"*
   - 👉 **Trả lời:** *"Dạ thưa Ban giám khảo, Apple Watch hay đồng hồ hãng có giá từ vài triệu đến mười mấy triệu đồng, quá đắt với phần lớn người cao tuổi ở nông thôn. Hơn nữa, tay người cử động vung vẩy rất nhiều nên đeo ở cổ tay dễ bị báo động giả; thắt lưng là trọng tâm của cơ thể nên đo cú ngã chính xác hơn nhiều. Thiết bị của chúng em tự làm bằng linh kiện phổ thông chỉ khoảng hơn 600.000 đồng, dễ tiếp cận hơn cho mọi gia đình ạ."*

2. **Nếu giám khảo hỏi:** *"Nếu người già lỡ cúi xuống nhặt đồ mà máy báo ngã thì làm sao?"*
   - 👉 **Trả lời:** *"Dạ thưa Ban giám khảo, chúng em có 2 lớp bảo vệ:*
     * *Lớp 1: Thuật toán đòi hỏi phải có cả rơi tự do, va đập mạnh và nằm yên sau đó. Cúi nhặt đồ thì không có rơi tự do và người ta sẽ đứng lên ngay chứ không nằm yên.*
     * *Lớp 2: Kể cả khi có báo nhầm, điện thoại sẽ đếm ngược 10 giây kèm chuông báo. Người dùng chỉ cần bấm nút 'Tôi ổn' trên màn hình là hệ thống hủy báo động ngay, không làm phiền đến người nhà ạ."*

3. **Nếu giám khảo hỏi:** *"Nếu ngã trong nhà không có sóng GPS hoặc nhà không có Wi-Fi thì máy có báo được không?"*
   - 👉 **Trả lời:** *"Dạ hoàn toàn báo được ạ! Hệ thống của chúng em dùng sóng di động thông thường qua thẻ SIM để tự động gọi điện và nhắn tin SMS, không cần mạng Internet/Wi-Fi. Đồng thời hệ thống sử dụng cơ chế định vị tổng hợp (Fused Location) lấy trạm BTS xung quanh nên dù ở trong nhà vẫn xác định được vị trí tương đối ạ."*
