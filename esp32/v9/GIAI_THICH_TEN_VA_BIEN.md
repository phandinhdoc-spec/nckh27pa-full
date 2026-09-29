# Giải thích tên, biến và hàm trong v9

Tài liệu này đi cùng `v9.ino`, `fall_detector.h`, `fall_profile.h` và thư mục `tests`. Tên trong mã nguồn được viết tiếng Việt không dấu để tương thích tốt với bộ công cụ Arduino; phần mô tả dùng tiếng Việt có dấu.

## Quy ước đặt tên

- Hậu tố `Ms`, `Us`, `Pa`, `M`, `Ms2`, `Dps` lần lượt biểu thị mili giây, micro giây, Pascal, mét, mét/giây² và độ/giây.
- `uint8_t`, `uint16_t`, `uint32_t` được chọn theo miền giá trị và để tiết kiệm bộ nhớ. Phép trừ số không dấu của `millis()` vẫn tính đúng khoảng thời gian khi bộ đếm tràn.
- `std::atomic` bảo vệ các cờ và mã số cùng được callback BLE và `loop()` truy cập, tránh điều kiện tranh chấp.
- Tên API bắt buộc như `setup()`, `loop()`, `onConnect()`, `onWrite()`, `notify()` và tên trường JSON/giao thức BLE phải giữ nguyên vì Android, Arduino và thư viện phụ thuộc vào chúng.

## Biến toàn cục trong `v9.ino`

| Nhóm biến | Vai trò |
|---|---|
| `busMpu`, `busGy63`, `DIA_CHI_MPU`, `DIA_CHI_GY63` | Hai bus I2C độc lập cho MPU-6050 và GY-63/MS5611, cùng địa chỉ phần cứng tương ứng. |
| Các `UUID_*` | Định danh dịch vụ và đặc tính BLE để Android tìm đúng kênh telemetry, SOS, lệnh và xác nhận cấu hình. |
| `bleDaKetNoi`, `daDangKySuKien`, `canGuiLaiSos`, `canQuangBaLai` | Các cờ nguyên tử phản ánh kết nối, đăng ký nhận SOS, yêu cầu gửi lại và quảng bá lại. |
| `canGuiDuLieu`, `maSosDaXacNhan`, `mtuBle` | Lưu yêu cầu telemetry, mã ACK SOS và MTU đã thương lượng với Android. |
| `dacTinhDuLieu`, `dacTinhSuKien`, `dacTinhXacNhan` | Con trỏ đến ba đặc tính BLE được dùng để gửi dữ liệu, SOS và kết quả cập nhật cấu hình. |
| `coCauHinhChoXuLy`, `boDemCauHinh`, `doDaiCauHinh`, `luuTruCauHinh` | Nhận JSON cấu hình trong callback, rồi để `loop()` kiểm tra và lưu vào NVS an toàn. |
| `boDemDuLieu`, `doDaiDuLieu`, `viTriDuLieu`, `soKhungDuLieu`, `tongKhungDuLieu` | Vùng đệm và tiến độ phân mảnh telemetry thành các khung BLE. |
| `boDemSos`, `doDaiSos`, `viTriSos`, `soKhungSos`, `tongKhungSos` | Hàng đợi SOS riêng, có ưu tiên cao hơn telemetry. |
| `maSuKienSos`, `sosLaThuNghiem`, `sosDangChoXacNhan` | Theo dõi định danh, loại sự kiện và trạng thái chờ Android ACK. |
| `lucGuiSosGanNhatMs`, `lucXepHangSosMs`, `THOI_HAN_XAC_NHAN_SOS_MS` | Điều phối việc gửi lại SOS và giới hạn thời gian chờ xác nhận. |
| `mauMpuMoiNhat`, `mauGy63MoiNhat` | Giữ mẫu cảm biến mới nhất để nhận dạng ngã, ghi nhật ký và tạo telemetry. |
| `mpuSanSang`, `gy63SanSang`, `boNhoMpuSanSang`, `boNhoMpuDay` | Trạng thái sẵn sàng của cảm biến và LittleFS, ngăn đọc hoặc ghi khi phần cứng lỗi. |
| `boPhatHienNga` | Lưu cấu hình và trạng thái nhận dạng giữa nhiều lần lấy mẫu. |

## Bản ghi mẫu cảm biến

`MauMpu` lưu thời điểm, ba trục gia tốc, ba trục tốc độ xoay và cờ `hopLe`. Cờ này ngăn chương trình dùng lại dữ liệu cũ sau lỗi I2C.

`MauGy63` lưu thời điểm, áp suất đã hiệu chỉnh, nhiệt độ, độ cao tương đối và cờ `hopLe`. `apSuatMocPa`, `apSuatLocPa`, `tongApSuatKhoiDongPa` và `soMauKhoiDongGy63` tạo mốc áp suất ổn định trước khi suy ra thay đổi độ cao.

## Cấu hình và bộ phát hiện ngã

`CauHinhNga` chứa 14 ngưỡng có thể nhận từ Android: va đập, nằm yên, rơi tự do, xoay, áp suất, thay đổi độ cao và thời hạn mẫu. `hopLe()` từ chối cấu hình sai kiểu hoặc nằm ngoài miền an toàn.

`BoPhatHienNga` duy trì chuỗi bằng chứng theo thứ tự: rơi tự do, xoay hoặc va đập, nằm yên, rồi thay đổi độ cao hay áp suất. Các mốc thời gian, bộ đếm mẫu và cờ trạng thái giúp không kết luận từ một mẫu đơn lẻ.

## Các hàm chính

| Hàm hoặc nhóm hàm | Vai trò |
|---|---|
| `khoiPhucBusMpu()`, `khoiTaoMpu()`, `kiemTraMpu()` | Khôi phục bus I2C, khởi tạo MPU-6050 và chẩn đoán kết nối. |
| `khoiTaoBoNhoMpu()`, `inNhatKyMpu()`, `docVaLuuMpu()` | Mở LittleFS, in CSV qua Serial và lưu mẫu MPU. Không tự định dạng LittleFS để tránh mất dữ liệu. |
| `layMauMpuPhatHienNga()` | Đọc nhanh 14 byte thanh ghi MPU, đổi sang đơn vị vật lý và trả độ lớn gia tốc, tốc độ xoay. |
| `khoiTaoGy63()`, `kiemTraGy63()`, `guiLenhGy63()`, `docByteGy63()`, `docAdcGy63()` | Đặt lại MS5611, kiểm tra PROM/CRC và thực hiện giao dịch I2C. |
| `docGy63()` | Máy trạng thái không chặn: bắt đầu chuyển đổi nhiệt độ hoặc áp suất, sau đó đọc kết quả khi đủ thời gian. |
| `XuLyMayChuBle`, `XuLyMoTaSuKien`, `XuLyDacTinhSuKien`, `XuLyYeuCauBle` | Callback BLE chỉ cập nhật cờ hoặc sao chép dữ liệu nhỏ; các xử lý nặng chạy trong `loop()`. |
| `khoiTaoBle()` | Tạo dịch vụ, đặc tính, callback và quảng bá BLE. |
| `xepSuKienSos()`, `guiTungBuocSos()` | Đóng gói SOS, chia thành khung, gửi lại khi cần và chờ ACK đúng mã sự kiện. |
| `phanTichCauHinhNga()`, `napCauHinhNga()`, `xuLyYeuCauCauHinh()` | Kiểm tra JSON đủ 14 trường, nạp/lưu NVS và áp dụng cấu hình hợp lệ. |
| `chuanBiDuLieu()`, `guiTungBuocDuLieu()` | Tạo JSON telemetry từ mẫu còn mới và gửi từng khung theo giới hạn MTU. |
| `capNhatPhatHienNga()` | Ghép mẫu MPU/GY-63, kiểm tra tuổi mẫu và xếp SOS khi vừa xác nhận ngã. |
| `setup()`, `loop()` | Khởi tạo phần cứng và liên tục điều phối lấy mẫu, BLE, SOS, telemetry cùng nhật ký. |

## Biến cục bộ đáng chú ý

- `lucThuLaiMs` giới hạn tần suất khởi tạo lại MPU khi cảm biến mất kết nối.
- `soLoiLienTiep` chỉ đánh dấu MPU lỗi sau nhiều lần đọc liên tiếp thất bại, tránh phản ứng quá mức với lỗi thoáng qua.
- Các biến 64-bit trong `docGy63()` giữ phép bù nhiệt độ MS5611 không bị tràn số.
- `kichThuocMtu`, `doDai` và `khung` giới hạn kích thước từng thông báo BLE; dữ liệu lớn được chia thành nhiều khung.
- `gocJson`, `ketThuc`, `tenTruong`, `cacGiaTri` và `soLuong` kiểm tra JSON hoàn chỉnh trước khi thay cấu hình đang chạy.

## Bài kiểm thử

`tests/fall_detector_test.cpp` tạo các chuỗi mẫu giả bằng `lapQuaKhoangMau()` để kiểm tra các trường hợp: ngã hợp lệ, rơi tự do quá ngắn, thiếu xoay, thiếu thay đổi độ cao, mất mẫu, cấu hình tùy chỉnh và bằng chứng áp suất. `main()` là điểm vào bắt buộc của chương trình kiểm thử C++.
