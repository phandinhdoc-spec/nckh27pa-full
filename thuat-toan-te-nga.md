# TÀI LIỆU HƯỚNG DẪN THUẬT TOÁN PHÁT HIỆN TÉ NGÃ BẰNG CẢM BIẾN GIA TỐC VÀ KHÍ ÁP
## Bản Tiếng Việt Dành Cho Học Sinh THCS & Nhóm Nghiên Cứu
*(Tài liệu đọc hiểu lý thuyết, phục vụ thực hành lập trình và báo cáo trước Hội đồng Giám khảo)*

---

## 📌 VÌ SAO CẦN TÀI LIỆU NÀY?
Khi làm đề tài nghiên cứu khoa học, học sinh THCS thường gặp khó khăn vì:
1. Các sách chuyên khảo quốc tế (như Elsevier, Springer) viết bằng tiếng Anh chuyên ngành sâu và có giá rất đắt (hàng chục USD mỗi chương), không phù hợp với thực tế học sinh.
2. Tài liệu này được biên soạn bằng **tiếng Việt 100%**, tóm lược toàn bộ nguyên lý khoa học chuẩn mực từ các bài báo khoa học mở và giáo trình kỹ thuật, giải thích theo cách gần gũi, dễ hiểu nhất với chương trình môn Khoa học tự nhiên cấp THCS.

---

## 1. NGUYÊN LÝ VẬT LÝ CỦA MỘT CÚ TÉ NGÃ

Một cú ngã bất ngờ của con người diễn ra rất nhanh (trong khoảng 1 đến 2 giây) nhưng luôn trải qua **3 giai đoạn vật lý liên tiếp**:

```
[Bình thường: 1g] 
       ↓ 
[Giai đoạn 1: RƠI TỰ DO] (Trọng lượng hẫng, a < 0,5g)
       ↓ 
[Giai đoạn 2: VA ĐẬP MẶNH] (Chạm sàn, đỉnh a > 2,5g)
       ↓ 
[Giai đoạn 3: BẤT ĐỘNG] (Nằm yên trên sàn, a quanh mức 1g)
```

### Giai đoạn 1: Rơi tự do (Free Fall)
* **Hiện tượng:** Khi bị trượt chân, vấp ngã hoặc khuỵu xuống do đột quỵ, cơ thể mất điểm tựa và rơi xuống đất.
* **Tín hiệu cảm biến:** Khi rơi, cảm biến rơi vào trạng thái "không trọng lượng" thoáng qua. 
* **Thông số nhận biết:** Gia tốc tổng $a$ tụt giảm đột ngột xuống dưới $4,9\text{ m/s}^2$ (tức nhỏ hơn $0,5g$), kéo dài liên tục từ $80\text{ ms}$ ($0,08$ giây) trở lên.

### Giai đoạn 2: Va đập (Impact)
* **Hiện tượng:** Cơ thể va chạm với mặt sàn cứng và đột ngột dừng lại.
* **Tín hiệu cảm biến:** Lực cản của sàn nhà tạo ra một xung lực cực mạnh tác động lên cơ thể.
* **Thông số nhận biết:** Gia tốc tổng $a$ tăng vọt lên rất cao, vượt ngưỡng $a \ge 25\text{ m/s}^2$ (khoảng $2,55g$).
* **Lưu ý:** Đỉnh va đập này xuất hiện cực nhanh (chỉ kéo dài khoảng 20 đến 50 phần nghìn giây). Vì vậy, cảm biến phải đo liên tục ở tần số **100 Hz** (100 lần mỗi giây) thì mới không bị bỏ sót.

### Giai đoạn 3: Bất động sau va đập (Inactivity / Stillness)
* **Hiện tượng:** Sau khi ngã, nếu người bệnh bị chấn thương hoặc đột quỵ bất tỉnh, họ sẽ nằm yên trên sàn nhà.
* **Tín hiệu cảm biến:** Cơ thể không còn cử động mạnh, gia tốc cảm nhận duy nhất chỉ còn là lực hút Trái Đất ($g \approx 9,81\text{ m/s}^2$).
* **Thông số nhận biết:** Độ lệch gia tốc so với trọng trường rất nhỏ: $|a - 9,81| \le 1,0\text{ m/s}^2$ liên tục trong ít nhất 1 giây ($1000\text{ ms}$).
* **Mục đích:** Giúp phân biệt cú ngã thật với hành động nhảy lò cò, chạy nhảy hay ném đồ vật (nhảy xong người ta sẽ cử động ngay chứ không nằm yên).

---

## 2. CÔNG THỨC TOÁN HỌC TRONG THUẬT TOÁN

### 2.1. Tính độ lớn gia tốc tổng hợp (Vector Magnitude)
Cảm biến MPU6050 gắn ở thắt lưng có thể bị xoay theo bất kỳ hướng nào khi người đeo chuyển động. Để thuật toán không bị phụ thuộc vào góc nghiêng của thiết bị, ta dùng **định lý Pythagore trong không gian 3 chiều** để tính gia tốc tổng hợp:

$$a = \sqrt{a_x^2 + a_y^2 + a_z^2}$$

* $a_x, a_y, a_z$: Gia tốc đo được theo 3 trục không gian của cảm biến (m/s²).
* $a$: Độ lớn gia tốc tổng hợp tác dụng lên cơ thể.
  - Khi đứng yên: $a \approx 9,81\text{ m/s}^2$ (bằng gia tốc trọng trường $g$).
  - Khi rơi tự do: $a \rightarrow 0\text{ m/s}^2$.
  - Khi va đập: $a \ge 25\text{ m/s}^2$.

### 2.2. Kết hợp cảm biến khí áp đo độ cao tương đối ($\Delta h$)
Để tránh báo động giả khi người dùng chỉ **ngồi mạnh xuống ghế sofa** (cũng có thể tạo va đập nhưng thắt lưng không hạ sát mặt sàn):
* Cảm biến khí áp MS5611 đo áp suất khí quyển $P$.
* Càng xuống thấp, áp suất khí quyển càng tăng. Chênh lệch áp suất được quy đổi ra độ thay đổi độ cao $\Delta h$:
  - Ngồi xuống ghế: Thắt lưng chỉ hạ xuống khoảng $0,1\text{ m} - 0,2\text{ m}$.
  - Ngã xuống sàn: Thắt lưng hạ xuống từ $0,4\text{ m}$ đến $0,8\text{ m}$ ($\Delta h \le -0,4\text{ m}$).
* Nếu có va chạm mà độ cao hạ sâu $\rightarrow$ Khẳng định chắc chắn người đó đã ngã xuống sàn.

---

## 3. LƯU ĐỒ THUẬT TOÁN MÁY TRẠNG THÁI (STATE MACHINE)

Thuật toán trong vi điều khiển ESP32 chạy theo quy trình từng bước rõ ràng:

```
[BẬT NGUỒN]
    ↓
(1) BOOT & HIỆU CHUẨN: Tự kiểm tra phần cứng, đo đường cơ sở
    ↓
(2) MONITORING (Giám sát): Đo MPU6050 & MS5611 liên tục 100 lần/giây
    ↓
    ├─ Nếu a < 4,9 m/s² (kéo dài >= 80 ms) ────────┐
    │                                             ↓
    │                                      (3) SUSPECTED (Nghi ngờ rơi)
    │                                             ↓
    │                                      Chờ trong 500 ms:
    │                                      ├─ Có va đập a >= 25 m/s²? ──┐
    │                                      └─ Không có → Quay lại (2)    │
    │                                                                   ↓
    └──────────────────────────────────────────────────────────(4) VERIFYING (Xác minh)
                                                                        ↓
                                                                   Đếm mẫu tĩnh trong 3000 ms:
                                                                   ├─ Nằm yên liên tục >= 1000 ms?
                                                                   │    ↓ (ĐÚNG)
                                                                   │  (5) FALL_CONFIRMED!
                                                                   │    - Gửi tín hiệu Bluetooth sang Android
                                                                   │    - Android đếm ngược 10 giây
                                                                   │    - Tự động gọi điện SOS & gửi SMS định vị
                                                                   │
                                                                   └─ Cử động tiếp (chưa đủ 1000 ms)?
                                                                        ↓ (BÁO GIẢ)
                                                                      Hủy bằng chứng → Quay lại (2)
```

---

## 4. TÀI LIỆU KHOA HỌC THAM KHẢO TIẾNG VIỆT & TÀI LIỆU MỞ

Nhóm học sinh có thể tự tin trích dẫn các tài liệu khoa học chính thống, công khai sau:

1. **Bài báo khoa học tiếng Việt (Trường Đại học Cần Thơ, 2017):**
   * *Tác giả:* Trần Công Án, Đỗ Thanh Đức, Lê Đình Chiến và cộng sự (2017).
   * *Tên bài báo:* *Phát hiện té ngã cho người cao tuổi bằng gia tốc kế*.
   * *Nguồn:* Tạp chí Khoa học Đại học Cần Thơ, Số chuyên đề Công nghệ thông tin, trang 65–71.
   * *Đặc điểm:* Toàn văn tiếng Việt, nghiên cứu thuật toán nhận diện té ngã cho người cao tuổi qua cảm biến gia tốc đặt tại thắt lưng.
   * 🔗 **Link xem bài báo:** [Trang bài báo tại Tạp chí ĐH Cần Thơ](https://ctujsvn.ctu.edu.vn/index.php/ctujsvn/article/view/2939)
   * 📥 **Link tải trực tiếp file PDF:** [Tải PDF trực tiếp (ĐH Cần Thơ)](https://ctujsvn.ctu.edu.vn/index.php/ctujsvn/article/download/2939/880) *(DOI: https://doi.org/10.22144/ctu.jsi.2017.009)*

2. **Bài báo khoa học tiếng Việt (Trường ĐH Bách Khoa Tp. HCM - ĐHQG TP.HCM, 2024):**
   * *Tác giả:* Lê Trường Giang, Nguyễn Khánh Lợi (Bộ môn Viễn thông, Khoa Điện - Điện tử, ĐH Bách Khoa TP.HCM).
   * *Tên bài báo:* *Hệ thống theo dõi sức khỏe tích hợp trên ứng dụng*.
   * *Nguồn:* Tạp chí Khoa học và Công nghệ Giao thông (JSTT), Tập 4, Số 4 (2024), trang 25–32.
   * *Đặc điểm:* Sử dụng cảm biến MPU6050 trên thiết bị đeo, áp dụng công thức Pythagore tính độ lớn vector gia tốc và kết nối ứng dụng điện thoại cảnh báo té ngã.
   * 🔗 **Link xem bài báo:** [Trang bài báo tại Tạp chí JSTT](https://jstt.vn/index.php/vn/article/view/254)
   * 📥 **Link tải trực tiếp file PDF:** [Tải PDF trực tiếp (ĐH Bách Khoa TP.HCM)](https://jstt.vn/index.php/vn/article/download/254/263) *(DOI: https://doi.org/10.58845/jstt.utt.2024.vn.4.4.25-32)*

3. **Giáo trình chuyên ngành tiếng Việt:**
   * *Tác giả:* PGS. TS. Nguyễn Nhật Quang và cộng sự (2023).
   * *Tên sách:* *Giáo trình cảm biến và đo lường*. Nhà xuất bản Khoa học và Kỹ thuật.
   * *Đặc điểm:* Giảng dạy chi tiết nguyên lý cảm biến quán tính IMU, cảm biến áp suất và cách giao tiếp vi điều khiển.

4. **Tài liệu kỹ thuật mở chính hãng (Analog Devices):**
   * *Tên tài liệu:* *Application Note AN-1023: Detecting Human Falls with a 3-Axis Digital Accelerometer*.
   * *Đặc điểm:* Tài liệu mở miễn phí của nhà sản xuất chip, minh họa đồ thị gia tốc 3 pha của cú ngã và hướng dẫn cài đặt ngưỡng rơi tự do, va chạm và nằm yên.
   * 📥 **Link tải trực tiếp file PDF:** [Analog Devices AN-1023 PDF](https://www.analog.com/media/en/technical-documentation/application-notes/AN-1023.pdf)

---

## 5. CÂU HỎI VẤN ĐÁP THỰC TẾ TRƯỚC GIÁM KHẢO

**❓ Câu hỏi:** *"Thuật toán phát hiện té ngã này các con lấy từ đâu, có tự làm được không?"*
* **👉 Học sinh trả lời:**
  > *"Dạ thưa Ban giám khảo, nguyên lý phân tích cú ngã thành 3 pha (rơi tự do $\rightarrow$ va đập $\rightarrow$ nằm yên) là phương pháp khoa học đã được công bố trên các tạp chí khoa học trong nước như Tạp chí Khoa học Đại học Cần Thơ và tài liệu kỹ thuật của hãng sản xuất cảm biến. Nhóm chúng em đã kế thừa nguyên lý này, tự tay lập trình mã nguồn firmware cho vi điều khiển ESP32-S3, tự lắp ráp mạch và thực nghiệm đo đạc để tìm ra bộ ngưỡng thông số phù hợp nhất với người cao tuổi Việt Nam ạ!"*
