#include <Arduino.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_MPU6050.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <cJSON.h>
#include <atomic>
#include <math.h>
#include "fall_detector.h"

// Sơ đồ chân ESP32-S3 Super Mini theo esp.md. Dùng hai bus I2C riêng để các cảm biến không phải tranh quyền giao tiếp.
TwoWire &busMpu = Wire;   // Bus I2C của MPU-6050: SDA nối GPIO8, SCL nối GPIO9.
TwoWire &busGy63 = Wire1;   // Bus I2C của GY-63/MS5611: SDA nối GPIO6, SCL nối GPIO7.
constexpr uint8_t DIA_CHI_MPU = 0x68; // Địa chỉ I2C mặc định của MPU-6050; khai báo một lần để các hàm dùng thống nhất.
constexpr uint8_t DIA_CHI_GY63 = 0x77; // Địa chỉ I2C của MS5611 trên mô-đun GY-63.
// Android dùng UUID này để lọc thiết bị quảng bá và tìm dịch vụ GATT Bluetooth.
constexpr const char *UUID_DICH_VU_BLE = "7d2a0001-6f45-4c2b-9a1e-38a8f5c10001"; // Mã định danh dịch vụ để ứng dụng tìm FallSafe.
constexpr const char *UUID_DU_LIEU_BLE = "7d2a0002-6f45-4c2b-9a1e-38a8f5c10001"; // Kênh gửi thông báo dữ liệu cảm biến theo yêu cầu.
constexpr const char *UUID_SU_KIEN_BLE = "7d2a0003-6f45-4c2b-9a1e-38a8f5c10001"; // Kênh thông báo SOS và sự kiện ngã được ưu tiên.
constexpr const char *UUID_YEU_CAU_BLE = "7d2a0005-6f45-4c2b-9a1e-38a8f5c10001"; // Kênh nhận yêu cầu lấy dữ liệu, xác nhận và bộ thông số từ Android.
constexpr const char *UUID_XAC_NHAN_BLE = "7d2a0006-6f45-4c2b-9a1e-38a8f5c10001"; // Kênh gửi thông báo kết quả cấu hình.
// Các cờ nguyên tử được hàm gọi lại BLE ghi và loop() đọc, tránh tranh chấp khi hai luồng cùng truy cập.
std::atomic<bool> bleDaKetNoi{false}; // Trạng thái kết nối điện thoại, dùng để quyết định có thể gửi hoặc nhận dữ liệu.
std::atomic<bool> daDangKySuKien{false}; // Cho biết Android đã bật nhận thông báo SOS hay chưa.
std::atomic<bool> canGuiLaiSos{false}; // Yêu cầu đặt lại tiến độ SOS khi kết nối BLE bị ngắt rồi nối lại.
std::atomic<bool> guiThongBaoSosLoi{false}; // Cờ báo gửi thông báo SOS thất bại; vòng lặp chính giữ khung dữ liệu để thử lại.
std::atomic<bool> canQuangBaLai{false}; // Yêu cầu loop() phát quảng bá lại sau khi điện thoại ngắt kết nối.
std::atomic<bool> canGuiDuLieu{false}; // Android bật cờ này khi gửi GET_TELEMETRY; loop() xử lý yêu cầu sau đó.
std::atomic<uint16_t> maSosDaXacNhan{0}; // Mã xác nhận nhận từ Android; chỉ xóa SOS khớp với mã này.
std::atomic<uint16_t> mtuBle{23}; // Kích thước gói dữ liệu hai bên đã thống nhất; mặc định BLE dùng 23 byte.
BLECharacteristic *dacTinhDuLieu = nullptr; // Con trỏ tới kênh thông báo dữ liệu cảm biến, tạo một lần trong khoiTaoBle().
BLECharacteristic *dacTinhSuKien = nullptr; // Con trỏ tới kênh thông báo SOS và các sự kiện ngã.
BLECharacteristic *dacTinhXacNhan = nullptr; // Con trỏ tới kênh thông báo kết quả cấu hình bộ thông số.
std::atomic<bool> coCauHinhChoXuLy{false}; // Báo có dữ liệu JSON trong vùng đệm đang chờ loop() phân tích.
char boDemCauHinh[512] = {}; // Vùng đệm cố định nhận JSON cấu hình, tránh cấp phát động trong hàm gọi lại BLE.
size_t doDaiCauHinh = 0; // Số byte JSON thực nhận, không tính phần trống còn lại của vùng đệm.
Preferences luuTruCauHinh; // Đối tượng truy cập NVS để giữ bộ thông số sau khi ESP khởi động lại.
uint32_t soThuTuDuLieu = 0; // Số thứ tự dữ liệu cảm biến tăng dần để Android nhận biết gói mới hoặc gói bị thiếu.
char boDemDuLieu[512] = {}; // Vùng đệm JSON cảm biến đang chuẩn bị gửi, có kích thước giới hạn để tiết kiệm bộ nhớ.
size_t doDaiDuLieu = 0; // Số byte dữ liệu cảm biến hợp lệ trong vùng đệm.
size_t viTriDuLieu = 0; // Vị trí byte tiếp theo khi dữ liệu cảm biến cần chia thành nhiều khung BLE.
uint16_t soKhungDuLieu = 0; // Chỉ số khung dữ liệu cảm biến đang gửi.
uint16_t tongKhungDuLieu = 0; // Tổng số khung dữ liệu; bằng 0 nếu JSON vừa một thông báo.
char boDemSos[96] = {}; // Vùng đệm JSON SOS riêng để SOS được ưu tiên hơn dữ liệu cảm biến.
size_t doDaiSos = 0; // Số byte thực có của JSON SOS.
size_t viTriSos = 0; // Vị trí byte tiếp theo trong JSON SOS khi gửi thành nhiều khung.
uint16_t soKhungSos = 0; // Chỉ số khung SOS tiếp theo cần gửi.
uint16_t tongKhungSos = 0; // Tổng số khung SOS, tính từ độ dài gói và kích thước gói tối đa.
uint16_t maSuKienSos = 0; // Mã sự kiện đang chờ Android nhận và xác nhận.
bool sosLaThuNghiem = false; // Đánh dấu SOS thử nghiệm để điện thoại xử lý riêng, không xem là cảnh báo thật.
bool sosDangChoXacNhan = false; // Cho biết đã gửi xong các khung và ESP đang chờ Android xác nhận.
uint32_t lucGuiSosGanNhatMs = 0; // Mốc gửi gần nhất, dùng để quyết định lúc thử gửi lại.
uint32_t lucXepHangSosMs = 0; // Mốc bắt đầu tính thời hạn, tránh giữ SOS vô hạn khi Android mất kết nối.
constexpr uint32_t THOI_GIAN_NGHI_SAU_SOS_MS = 5UL * 60UL * 1000UL; // Ngừng đọc cảm biến 5 phút từ lúc bật SOS.
constexpr uint32_t THOI_HAN_XAC_NHAN_SOS_MS = THOI_GIAN_NGHI_SAU_SOS_MS; // Giữ SOS để thử gửi lại trong suốt thời gian nghỉ.
constexpr float NGUONG_VA_DAP_MS2 = 13.0f; // Ngưỡng va đập áp dụng cho v9, kể cả khi NVS lưu 20 hoặc mặc định là 25.
bool dangNghiCamBienSauSos = false; // Vẫn xử lý BLE và ACK, nhưng tạm ngừng mọi lượt đọc cảm biến.
uint32_t lucBatDauNghiCamBienMs = 0; // Mốc bắt đầu khoảng nghỉ để tính thời hạn bằng phép trừ chống tràn millis().

// Gom các giá trị GY-63 thành một bản ghi để các hàm khác dùng cùng thời điểm lấy mẫu và trạng thái hợp lệ.
struct MauGy63 {
  uint32_t thoiDiemMs; // Thời điểm lấy mẫu, dùng để phát hiện dữ liệu đã cũ.
  float apSuatPa; // Áp suất đã hiệu chỉnh, tính bằng Pa.
  float nhietDoC; // Nhiệt độ đã hiệu chỉnh, tính bằng độ C.
  float thayDoiDoCaoM; // Độ cao tương đối so với áp suất ban đầu, dùng làm bằng chứng đổi tư thế.
  bool hopLe; // Cho biết cảm biến đọc thành công và các giá trị nằm trong miền hợp lý.
};
MauGy63 mauGy63MoiNhat = {}; // Bản ghi áp suất, nhiệt độ và độ cao mới nhất dùng chung trong chương trình.
uint16_t heSoHieuChuanGy63[8] = {}; // Hệ số nhà sản xuất trong PROM MS5611, dùng đổi số đo thô thành giá trị vật lý.
float apSuatMocPa = 0; // Áp suất trung bình ban đầu, làm mốc tính độ cao tương đối.
bool gy63SanSang = false; // Chỉ đọc bộ chuyển đổi sau khi PROM đã được đọc và kiểm tra.
uint32_t soMauGy63 = 0; // Đếm mẫu hợp lệ để thống kê tần suất lấy mẫu.
// Nhóm trạng thái này ghi nhớ cảm biến đang đo gì, tránh dùng delay để chờ bộ chuyển đổi hoàn tất.
enum class GiaiDoanGy63 : uint8_t { RANH, CHO_NHIET_DO, CHO_AP_SUAT };
GiaiDoanGy63 giaiDoanGy63 = GiaiDoanGy63::RANH; // Lưu bước đo hiện tại để bộ xử lý vẫn làm việc trong lúc cảm biến chuyển đổi.
uint32_t lucBatDauChuyenDoiUs = 0; // Mốc micros() dùng kiểm tra MS5611 đã chuyển đổi xong chưa.
uint32_t lucDocGy63TiepTheoMs = 0; // Lịch lấy mẫu tiếp theo và giới hạn tần suất thử khởi tạo lại.
uint32_t nhietDoThoGy63 = 0; // Số đo nhiệt độ thô được giữ lại trong lúc bắt đầu phép đo áp suất.
float apSuatLocPa = 0; // Áp suất đã lọc để giảm nhiễu khi tính độ cao.
double tongApSuatKhoiDongPa = 0; // Tổng 25 mẫu đầu để tính mốc áp suất ổn định hơn một mẫu riêng lẻ.
uint8_t soMauKhoiDongGy63 = 0; // Đếm số mẫu khởi động để biết khi nào có thể công bố mốc áp suất.

// Gom sáu trục và thời điểm lấy mẫu; tệp CSV lưu lịch sử, còn bản ghi này giữ mẫu hiện tại.
struct MauMpu {
  uint32_t thoiDiemMs; // Thời điểm lấy mẫu, dùng để kiểm tra tuổi dữ liệu.
  float giaTocX, giaTocY, giaTocZ; // Ba trục gia tốc m/s² dùng tính hướng và độ lớn va đập.
  float tocDoXoayX, tocDoXoayY, tocDoXoayZ; // Ba trục tốc độ xoay rad/s dùng nhận biết xoay hoặc lật quanh từng trục.
  bool hopLe; // Ngăn dùng lại mẫu cũ còn trong bộ nhớ tạm sau lỗi I2C.
};

Adafruit_MPU6050 camBienMpu; // Đối tượng thư viện quản lý cấu hình và giao tiếp với MPU-6050.
MauMpu mauMpuMoiNhat = {}; // Bản ghi MPU dùng nhận dạng ngã, tạo dữ liệu cảm biến và ghi tệp CSV.
bool mpuSanSang = false; // Cho biết có thể đọc MPU; chuyển về sai khi I2C lỗi liên tiếp.
uint32_t soMauMpu = 0; // Đếm mẫu MPU thành công để thống kê tần suất lấy mẫu.
bool boNhoMpuSanSang = false; // Cho biết LittleFS mở thành công; chỉ khi đó mới được ghi nhật ký.
bool boNhoMpuDay = false; // Dừng ghi khi tệp đạt giới hạn dung lượng đã đặt.
BoPhatHienNga boPhatHienNga; // Lưu bộ thông số và tiến trình nhận dạng qua nhiều lần lấy mẫu.
constexpr const char *DUONG_DAN_NHAT_KY_MPU = "/mpu.csv"; // Đường dẫn cố định dùng ghi và đọc nhật ký.
// Khai báo trước vì phần xử lý Serial và nhận dạng ngã gọi các hàm này trước phần định nghĩa bên dưới.
void xepSuKienSos(uint16_t maSuKien, bool laThuNghiem = false);
bool kiemTraDiaChiI2c(TwoWire &busI2c, uint8_t diaChi);

// Khôi phục giao tiếp sau lỗi I2C: tạo xung SCL để nhả SDA rồi khởi tạo lại bus theo chân chuẩn.
void khoiPhucBusMpu() {
  // Nếu MPU giữ SDA ở mức thấp sau giao dịch lỗi, giải phóng bus I2C trước khi thử lại.
  busMpu.end();
  pinMode(8, INPUT_PULLUP);
  pinMode(9, OUTPUT_OPEN_DRAIN);
  digitalWrite(9, HIGH);
  // Giới hạn tối đa 9 xung theo quy trình I2C, tránh lặp vô hạn nếu đường dây bị chập.
  for (uint8_t chiSo = 0; chiSo < 9 && digitalRead(8) == LOW; ++chiSo) {
    digitalWrite(9, LOW);
    delayMicroseconds(5);
    digitalWrite(9, HIGH);
    delayMicroseconds(5);
  }
  pinMode(8, OUTPUT_OPEN_DRAIN);
  digitalWrite(8, LOW);
  delayMicroseconds(5);
  digitalWrite(9, HIGH);
  delayMicroseconds(5);
  digitalWrite(8, HIGH);
  busMpu.begin(8, 9, 100000);
  busMpu.setTimeOut(5);
}

// Khởi tạo MPU và cấu hình độ nhạy; thời hạn thử lại ngăn khởi tạo liên tục khi cảm biến mất kết nối.
void khoiTaoMpu() {
  // Hai mốc tồn tại qua các lần gọi để giữ thời hạn thử lại mà không cần biến toàn cục.
  static uint32_t lucThuLaiMs = 0;
  uint32_t hienTaiMs = millis();
  if (lucThuLaiMs != 0 && int32_t(hienTaiMs - lucThuLaiMs) < 0) return;
  lucThuLaiMs = hienTaiMs + 5000;
  if (!kiemTraDiaChiI2c(busMpu, DIA_CHI_MPU)) khoiPhucBusMpu();
  mpuSanSang = camBienMpu.begin(DIA_CHI_MPU, &busMpu);
  if (mpuSanSang) {
    // Chọn thang ±16g vì 25 m/s² vượt thang ±2g mặc định; lấy mẫu ở tần số 100 Hz.
    camBienMpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    camBienMpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    camBienMpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
    camBienMpu.setSampleRateDivisor(9);
  }
  Serial.println(mpuSanSang ? "MPU-6050: san sang doc 6 truc" : "MPU-6050: khong khoi tao duoc");
}

// Mở hệ thống tệp LittleFS để ghi và đọc CSV; không tự tạo hệ thống tệp vì thao tác đó sẽ xóa dữ liệu flash.
void khoiTaoBoNhoMpu() {
  // Không tự tạo hệ thống tệp: giữ nguyên dữ liệu flash nếu mở hệ thống tệp thất bại.
  boNhoMpuSanSang = LittleFS.begin(false);
  if (!boNhoMpuSanSang) {
    Serial.println("LittleFS chua san sang. Gui FORMAT_MPU qua Serial de tao he thong tep.");
  }
}

// In tệp CSV ra Serial để thu thập mẫu; đóng tệp trước khi hàm kết thúc.
void inNhatKyMpu() {
  if (!boNhoMpuSanSang) return;
  File tep = LittleFS.open(DUONG_DAN_NHAT_KY_MPU, "r");
  if (!tep) {
    Serial.println("Chua co du lieu MPU.");
    return;
  }
  Serial.println("BEGIN_MPU_CSV");
  while (tep.available()) Serial.write(tep.read());
  tep.close();
  Serial.println("END_MPU_CSV");
}

// Xử lý lệnh kiểm tra và sửa lỗi nhận từ Serial, tách khỏi loop() để vòng lặp chính gọn hơn.
void xuLyLenhSerialMpu() {
  if (!Serial.available()) return;
  String lenh = Serial.readStringUntil('\n'); // Lưu dòng lệnh để so sánh mà không giữ dữ liệu Serial tạm thời.
  lenh.trim();
  if (lenh == "DUMP_MPU") {
    inNhatKyMpu();
  } else if (lenh == "TEST_SOS") {
    if (doDaiSos != 0) {
      Serial.println("TEST_SOS: dang cho SOS truoc do");
    } else {
      // Biến static giữ giá trị qua các lần gọi, nhờ đó mỗi cảnh báo thử nghiệm có mã riêng.
      static uint16_t maSuKienThu = 40000;
      if (++maSuKienThu == 0) maSuKienThu = 40000;
      xepSuKienSos(maSuKienThu, true);
      Serial.printf("TEST_SOS: da tao canh bao gia, id=%u\n", maSuKienThu);
    }
  } else if (lenh == "TEST_SOS_REAL") {
    if (!bleDaKetNoi.load() || !daDangKySuKien.load()) {
      Serial.println("TEST_SOS_REAL: Android chua dang ky nhan event BLE");
    } else if (doDaiSos != 0) {
      Serial.println("TEST_SOS_REAL: dang cho SOS truoc do");
    } else {
      // Dùng khoảng mã khác để phân biệt phép thử SOS thật với cảnh báo thử nghiệm thông thường.
      static uint16_t maSuKienThu = 50000;
      if (++maSuKienThu == 0) maSuKienThu = 50000;
      xepSuKienSos(maSuKienThu);
      Serial.printf("TEST_SOS_REAL: da tao SOS that, id=%u\n", maSuKienThu);
    }
  } else if (lenh == "FORMAT_MPU" && !boNhoMpuSanSang) {
    if (LittleFS.format()) {
      khoiTaoBoNhoMpu();
      Serial.println(boNhoMpuSanSang ? "LittleFS da san sang" : "LittleFS mount that bai");
    } else {
      Serial.println("LittleFS format that bai");
    }
  }
}

// Đọc đầy đủ dữ liệu để hiển thị và ghi nhật ký; gọi ít thường xuyên hơn phép lấy mẫu nhận dạng ngã.
void docVaLuuMpu() {
  if (!mpuSanSang) khoiTaoMpu();
  if (!mpuSanSang || !kiemTraDiaChiI2c(busMpu, DIA_CHI_MPU)) {
    mauMpuMoiNhat.hopLe = false;
    Serial.println("MPU-6050: khong co mau moi");
    return;
  }

  // Thư viện trả về ba sự kiện gia tốc, con quay hồi chuyển và nhiệt độ; chép các trục vào bản ghi MauMpu.
  sensors_event_t giaToc, xoay, nhietDo;
  if (!camBienMpu.getEvent(&giaToc, &xoay, &nhietDo)) {
    mauMpuMoiNhat.hopLe = false;
    Serial.println("MPU-6050: doc that bai");
    return;
  }
  mauMpuMoiNhat = {millis(), giaToc.acceleration.x, giaToc.acceleration.y,
               giaToc.acceleration.z, xoay.gyro.x, xoay.gyro.y,
               xoay.gyro.z, true};
  Serial.printf("MPU ax=%.3f ay=%.3f az=%.3f m/s^2 | gx=%.3f gy=%.3f gz=%.3f rad/s\n",
                mauMpuMoiNhat.giaTocX, mauMpuMoiNhat.giaTocY, mauMpuMoiNhat.giaTocZ,
                mauMpuMoiNhat.tocDoXoayX, mauMpuMoiNhat.tocDoXoayY, mauMpuMoiNhat.tocDoXoayZ);

  if (!boNhoMpuSanSang || boNhoMpuDay) return;
  // Mở tệp ở chế độ ghi nối tiếp để không ghi đè các mẫu cũ.
  File tep = LittleFS.open(DUONG_DAN_NHAT_KY_MPU, "a");
  if (!tep) {
    Serial.println("MPU-6050: khong mo duoc tep CSV");
    return;
  }
  if (tep.size() >= LittleFS.totalBytes() * 3 / 4) {
    boNhoMpuDay = true;
    tep.close();
    Serial.println("MPU-6050: tep CSV da dat gioi han 75% dung luong LittleFS");
    return;
  }
  if (tep.size() == 0) tep.println("time_ms,ax_m_s2,ay_m_s2,az_m_s2,gx_rad_s,gy_rad_s,gz_rad_s");
  tep.printf("%lu,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
              (unsigned long)mauMpuMoiNhat.thoiDiemMs, mauMpuMoiNhat.giaTocX, mauMpuMoiNhat.giaTocY,
              mauMpuMoiNhat.giaTocZ, mauMpuMoiNhat.tocDoXoayX, mauMpuMoiNhat.tocDoXoayY, mauMpuMoiNhat.tocDoXoayZ);
  tep.close();
}

// Đọc nhanh một dãy thanh ghi liên tiếp, tính độ lớn ba trục và trả kết quả để bộ nhận dạng dùng ngay.
bool layMauMpuPhatHienNga(float &doLonGiaTocMs2, float &doLonXoayDps) {
  // Biến này giữ số lỗi qua các lần gọi; chỉ đánh dấu mất cảm biến sau 3 lỗi liên tiếp.
  static uint8_t soLoiLienTiep = 0;
  if (!mpuSanSang) return false;
  uint8_t cacByte[14]; // MPU trả 14 byte gồm gia tốc, nhiệt độ và con quay hồi chuyển trong một lượt đọc liên tiếp.
  busMpu.beginTransmission(DIA_CHI_MPU);
  busMpu.write(0x3B); // ACCEL_XOUT_H: địa chỉ đầu tiên của dãy thanh ghi gia tốc và nhiệt độ.
  if (busMpu.endTransmission(false) != 0 ||
      busMpu.requestFrom(DIA_CHI_MPU, (uint8_t)sizeof(cacByte)) != sizeof(cacByte)) {
    if (++soLoiLienTiep >= 3) mpuSanSang = false;
    while (busMpu.available()) busMpu.read();
    mauMpuMoiNhat.hopLe = false;
    return false;
  }
  soLoiLienTiep = 0;
  for (uint8_t chiSo = 0; chiSo < sizeof(cacByte); ++chiSo) cacByte[chiSo] = busMpu.read();
  // Hàm phụ ghép hai byte cao và thấp thành số có dấu 16 bit tại vị trí của từng trục.
  auto docTruc = [&](uint8_t chiSo) -> int16_t {
    return int16_t((uint16_t(cacByte[chiSo]) << 8) | cacByte[chiSo + 1]);
  };
  constexpr float heSoGiaToc = 9.80665f / 2048.0f; // Đổi số đếm thô ở thang ±16g sang m/s².
  constexpr float heSoXoay = 1.0f / 16.4f;         // Đổi số đếm thô ở thang ±2000 độ/giây sang độ/giây.
  float giaTocX = docTruc(0) * heSoGiaToc, giaTocY = docTruc(2) * heSoGiaToc, giaTocZ = docTruc(4) * heSoGiaToc;
  float tocDoXoayX = docTruc(8) * heSoXoay, tocDoXoayY = docTruc(10) * heSoXoay, tocDoXoayZ = docTruc(12) * heSoXoay;
  doLonGiaTocMs2 = sqrtf(giaTocX * giaTocX + giaTocY * giaTocY + giaTocZ * giaTocZ);
  doLonXoayDps = sqrtf(tocDoXoayX * tocDoXoayX + tocDoXoayY * tocDoXoayY + tocDoXoayZ * tocDoXoayZ);
  mauMpuMoiNhat = {millis(), giaTocX, giaTocY, giaTocZ, tocDoXoayX * 0.017453293f, tocDoXoayY * 0.017453293f,
               tocDoXoayZ * 0.017453293f, true};
  ++soMauMpu;
  return true;
}

// Các tên onConnect, onDisconnect và onMtuChanged phải khớp với tên hàm mà thư viện BLE gọi.
// Hàm gọi lại chỉ cập nhật cờ nguyên tử; loop() đảm nhiệm gửi dữ liệu và khởi động lại quảng bá.
class XuLyMayChuBle : public BLEServerCallbacks {
  void onConnect(BLEServer *) override {
    mtuBle.store(23);
    daDangKySuKien.store(false);
    bleDaKetNoi.store(true);
  }

  void onDisconnect(BLEServer *) override {
    bleDaKetNoi.store(false);
    daDangKySuKien.store(false);
    canGuiDuLieu.store(false);
    canGuiLaiSos.store(true);
    canQuangBaLai.store(true);
  }

#if defined(CONFIG_NIMBLE_ENABLED)
  void onMtuChanged(BLEServer *, ble_gap_conn_desc *, uint16_t kichThuocMtu) override {
    mtuBle.store(kichThuocMtu);
  }
#elif defined(CONFIG_BLUEDROID_ENABLED)
  void onMtuChanged(BLEServer *, esp_ble_gatts_cb_param_t *thamSo) override {
    mtuBle.store(thamSo->kichThuocMtu.kichThuocMtu);
  }
#endif
};

// onWrite là hàm thư viện gọi khi mô tả BLE được ghi; đọc cờ cho biết Android đã bật thông báo hay chưa.
class XuLyMoTaSuKien : public BLEDescriptorCallbacks {
  void onWrite(BLEDescriptor *boMoTa) override {
    const uint8_t *giaTri = boMoTa->getValue();
    daDangKySuKien.store(boMoTa->getLength() >= 2 && (giaTri[0] & 1) != 0);
  }
};

// Hàm gọi lại theo dõi kết quả gửi thông báo và trạng thái đăng ký nhận tin trên kênh SOS.
class XuLyDacTinhSuKien : public BLECharacteristicCallbacks {
  void onStatus(BLECharacteristic *, Status trangThaiGui, uint32_t) override {
    if (trangThaiGui != SUCCESS_NOTIFY) {
      guiThongBaoSosLoi.store(true);
      if (trangThaiGui == ERROR_NOTIFY_DISABLED || trangThaiGui == ERROR_NO_SUBSCRIBER)
        daDangKySuKien.store(false);
    }
  }
#if defined(CONFIG_NIMBLE_ENABLED)
  void onSubscribe(BLECharacteristic *, ble_gap_conn_desc *, uint16_t giaTri) override {
    daDangKySuKien.store((giaTri & 1) != 0);
  }
#endif
};

// Nhận lệnh từ điện thoại. Hàm gọi lại chỉ đặt cờ hoặc sao chép dữ liệu nhỏ, không xử lý I2C hay JSON nặng.
class XuLyYeuCauBle : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *dacTinh) override {
    // Hàm gọi lại BLE chỉ đặt cờ; việc đóng gói và giao tiếp I2C được thực hiện trong loop().
    String giaTri = dacTinh->getValue(); // Bản sao lệnh vừa nhận để phân tích, không giữ con trỏ tới dữ liệu của thư viện.
    if (giaTri == "GET_TELEMETRY") {
      canGuiDuLieu.store(true);
    } else if (giaTri.startsWith("SOS_ACK:")) {
      long maSo = giaTri.substring(8).toInt(); // Đổi mã xác nhận từ chuỗi sang số trước khi lưu vào cờ nguyên tử.
      if (maSo > 0 && maSo <= 65535) maSosDaXacNhan.store(uint16_t(maSo));
    } else if (!coCauHinhChoXuLy.load() && giaTri.length() > 0 && giaTri.length() < sizeof(boDemCauHinh) && giaTri[0] == '{') {
      memcpy(boDemCauHinh, giaTri.c_str(), giaTri.length());
      boDemCauHinh[giaTri.length()] = 0;
      doDaiCauHinh = giaTri.length();
      coCauHinhChoXuLy.store(true);
    }
  }
};

// Tạo dịch vụ và các kênh GATT Bluetooth theo UUID ứng dụng Android đang dùng, sau đó bật quảng bá.
void khoiTaoBle() {
  // Thêm phần cuối địa chỉ MAC vào tên để điện thoại phân biệt các bo mạch trùng tên.
  char tenThietBi[20];
  snprintf(tenThietBi, sizeof(tenThietBi), "FALLSAFE-%04X", (unsigned)(ESP.getEfuseMac() & 0xFFFF));

  BLEDevice::init(tenThietBi);
  BLEDevice::setMTU(512);
  // Các con trỏ trỏ tới đối tượng BLE do thư viện quản lý, dùng để gắn hàm gọi lại và tạo cây dịch vụ.
  BLEServer *mayChu = BLEDevice::createServer();
  mayChu->setCallbacks(new XuLyMayChuBle());
  BLEService *dichVu = mayChu->createService(UUID_DICH_VU_BLE);
  dacTinhDuLieu = dichVu->createCharacteristic(UUID_DU_LIEU_BLE,
                                                          BLECharacteristic::PROPERTY_NOTIFY);
  dacTinhDuLieu->addDescriptor(new BLE2902());
  dacTinhSuKien = dichVu->createCharacteristic(UUID_SU_KIEN_BLE,
                                                      BLECharacteristic::PROPERTY_NOTIFY);
  dacTinhSuKien->setCallbacks(new XuLyDacTinhSuKien());
  BLE2902 *boMoTaSuKien = new BLE2902();
  boMoTaSuKien->setCallbacks(new XuLyMoTaSuKien());
  dacTinhSuKien->addDescriptor(boMoTaSuKien);
  BLECharacteristic *dacTinhYeuCau = dichVu->createCharacteristic(
      UUID_YEU_CAU_BLE, BLECharacteristic::PROPERTY_WRITE);
  dacTinhYeuCau->setCallbacks(new XuLyYeuCauBle());
  dacTinhXacNhan = dichVu->createCharacteristic(UUID_XAC_NHAN_BLE,
                                                    BLECharacteristic::PROPERTY_NOTIFY);
  dacTinhXacNhan->addDescriptor(new BLE2902());
  dichVu->start();

  BLEAdvertising *quangBa = BLEDevice::getAdvertising();
  quangBa->addServiceUUID(UUID_DICH_VU_BLE);
  quangBa->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.printf("BLE dang quang ba: %s\n", tenThietBi);
}

// Kiểm tra địa chỉ I2C; trả về true nếu thiết bị phản hồi. Tham số bus cho phép dùng với cả hai đường I2C.
bool kiemTraDiaChiI2c(TwoWire &busI2c, uint8_t diaChi) {
  busI2c.beginTransmission(diaChi);
  return busI2c.endTransmission() == 0;
}

// Đọc một thanh ghi trên một bus I2C bất kỳ; giaTri nhận kết quả, còn false báo giao dịch lỗi.
bool docThanhGhiI2c(TwoWire &busI2c, uint8_t diaChi, uint8_t thanhGhi, uint8_t &giaTri) {
  busI2c.beginTransmission(diaChi);
  busI2c.write(thanhGhi);
  if (busI2c.endTransmission(false) != 0) return false;
  if (busI2c.requestFrom(diaChi, (uint8_t)1) != 1) return false;
  giaTri = busI2c.read();
  return true;
}

// Kiểm tra lúc khởi động: quét địa chỉ, thử bus I2C đảo chân và đọc WHO_AM_I nếu tìm thấy MPU.
void kiemTraMpu() {
  if (!kiemTraDiaChiI2c(busMpu, DIA_CHI_MPU)) {
    Serial.println("MPU-6050: khong phan hoi tai 0x68");
    Serial.print("I2C MPU GPIO8/9 tim thay:");
    bool timThay = false; // Phân biệt bus I2C hoàn toàn trống với trường hợp chỉ không có MPU ở địa chỉ chuẩn.
    for (uint8_t diaChi = 0x08; diaChi <= 0x77; ++diaChi) {
      if (kiemTraDiaChiI2c(busMpu, diaChi)) {
        Serial.printf(" 0x%02X", diaChi);
        timThay = true;
      }
    }
    if (!timThay) Serial.print(" khong co thiet bi");
    Serial.println();
    if (kiemTraDiaChiI2c(busGy63, 0x68) || kiemTraDiaChiI2c(busGy63, 0x69))
      Serial.println("MPU co the dang noi nham bus GPIO6/7");
    busMpu.end();
    busMpu.begin(9, 8, 100000); // Chỉ thử trường hợp SDA/SCL bị đảo, sau đó khôi phục sơ đồ chân chuẩn.
    if (kiemTraDiaChiI2c(busMpu, 0x68) || kiemTraDiaChiI2c(busMpu, 0x69))
      Serial.println("MPU co the dang dao SDA/SCL GPIO8/9");
    busMpu.end();
    busMpu.begin(8, 9, 100000);
    busMpu.setTimeOut(5);
    return;
  }
  uint8_t maSo; // WHO_AM_I là một byte, dùng xác nhận đúng chip đang phản hồi tại địa chỉ MPU.
  if (!docThanhGhiI2c(busMpu, DIA_CHI_MPU, 0x75, maSo)) {
    Serial.println("MPU-6050: loi doc WHO_AM_I");
    return;
  }
  Serial.printf("MPU-6050: dia chi 0x%02X, WHO_AM_I=0x%02X %s\n",
                DIA_CHI_MPU, maSo, (maSo == 0x68) ? "OK" : "(kiem tra lai chip)");
}

// Kiểm tra GY-63 có phản hồi và đọc hệ số PROM C1 để xác nhận giao tiếp I2C.
void kiemTraGy63() {
  if (!kiemTraDiaChiI2c(busGy63, DIA_CHI_GY63)) {
    Serial.println("GY-63/MS5611: khong phan hoi tai 0x77");
    return;
  }
  busGy63.beginTransmission(DIA_CHI_GY63);
  busGy63.write(0x1E); // Lệnh đặt lại MS5611.
  if (busGy63.endTransmission() != 0) {
    Serial.println("GY-63/MS5611: loi reset");
    return;
  }
  delay(4);
  uint8_t byteCao, byteThap; // Hai byte PROM được ghép thành hệ số hiệu chuẩn 16 bit bên dưới.
  busGy63.beginTransmission(DIA_CHI_GY63);
  busGy63.write(0xA2); // Địa chỉ PROM C1, chứa hệ số hiệu chuẩn áp suất.
  if (busGy63.endTransmission(false) != 0 || busGy63.requestFrom(DIA_CHI_GY63, (uint8_t)2) != 2) {
    Serial.println("GY-63/MS5611: loi doc PROM");
    return;
  }
  byteCao = busGy63.read();
  byteThap = busGy63.read();
  uint16_t heSoC1 = ((uint16_t)byteCao << 8) | byteThap;
  Serial.printf("GY-63/MS5611: dia chi 0x%02X, PROM C1=%u %s\n",
                DIA_CHI_GY63, heSoC1, (heSoC1 != 0 && heSoC1 != 0xFFFF) ? "OK" : "(gia tri bat thuong)");
}

// Gửi một lệnh MS5611 (đặt lại hoặc bắt đầu phép đo) và báo lỗi I2C cho hàm gọi.
bool guiLenhGy63(uint8_t lenh) {
  busGy63.beginTransmission(DIA_CHI_GY63);
  busGy63.write(lenh);
  return busGy63.endTransmission() == 0;
}

// Gửi địa chỉ lệnh rồi đọc sốLuong byte vào dauRa; dùng chung cho PROM và bộ chuyển đổi tương tự sang số.
bool docByteGy63(uint8_t lenh, uint8_t *dauRa, uint8_t soLuong) {
  busGy63.beginTransmission(DIA_CHI_GY63);
  busGy63.write(lenh);
  if (busGy63.endTransmission(false) != 0) return false;
  if (busGy63.requestFrom(DIA_CHI_GY63, soLuong) != soLuong) {
    while (busGy63.available()) busGy63.read();
    return false;
  }
  for (uint8_t chiSo = 0; chiSo < soLuong; ++chiSo) dauRa[chiSo] = busGy63.read();
  return true;
}

// Tính mã kiểm tra CRC 4 bit của 8 từ PROM, giúp phát hiện hệ số cảm biến bị đọc sai.
uint8_t tinhCrcGy63(const uint16_t *cacHeSoProm) {
  uint16_t phanDu = 0;
  for (uint8_t chiSo = 0; chiSo < 16; ++chiSo) {
    uint16_t tuDuLieu = cacHeSoProm[chiSo / 2];
    if (chiSo / 2 == 7) tuDuLieu &= 0xFF00; // Bỏ 4 bit CRC được lưu trong PROM[7].
    phanDu ^= (chiSo & 1) ? (tuDuLieu & 0xFF) : (tuDuLieu >> 8);
    for (uint8_t bitThu = 0; bitThu < 8; ++bitThu) {
      phanDu = (phanDu & 0x8000) ? uint16_t((phanDu << 1) ^ 0x3000)
                                         : uint16_t(phanDu << 1);
    }
  }
  return (phanDu >> 12) & 0x0F;
}

// Đặt lại GY-63, nạp PROM và chỉ báo sẵn sàng nếu CRC hoặc kết quả đối chiếu hệ số hợp lệ.
void khoiTaoGy63() {
  gy63SanSang = false;
  mauGy63MoiNhat.hopLe = false;
  giaiDoanGy63 = GiaiDoanGy63::RANH;
  lucDocGy63TiepTheoMs = millis() + 5000; // Kể cả lỗi sớm cũng không thử lại ở mọi vòng lặp.
  if (!guiLenhGy63(0x1E)) return; // Đặt lại cảm biến rồi nạp lại hệ số hiệu chuẩn.
  delay(4);
  for (uint8_t chiSo = 0; chiSo < 8; ++chiSo) {
    uint8_t cacByte[2]; // Mỗi hệ số PROM dài 16 bit, cần hai byte I2C để ghép lại.
    if (!docByteGy63(0xA0 + 2 * chiSo, cacByte, 2)) return;
    heSoHieuChuanGy63[chiSo] = (uint16_t(cacByte[0]) << 8) | cacByte[1];
    if (chiSo >= 1 && chiSo <= 6 && (heSoHieuChuanGy63[chiSo] == 0 || heSoHieuChuanGy63[chiSo] == 0xFFFF)) return;
  }
  gy63SanSang = tinhCrcGy63(heSoHieuChuanGy63) == (heSoHieuChuanGy63[7] & 0x0F);
  if (!gy63SanSang && (heSoHieuChuanGy63[7] & 0x0F) == 0) {
    // Mô-đun đang dùng giá trị 0 cho CRC; chỉ chấp nhận nếu đọc lại cả 8 từ PROM đều trùng khớp.
    gy63SanSang = true;
    // Nếu PROM không cung cấp CRC, đọc lại từng từ để xác nhận hệ số ổn định thay vì tin vào một lượt đọc.
    for (uint8_t chiSo = 0; chiSo < 8; ++chiSo) {
      uint8_t cacByte[2];
      if (!docByteGy63(0xA0 + 2 * chiSo, cacByte, 2) ||
          heSoHieuChuanGy63[chiSo] != ((uint16_t(cacByte[0]) << 8) | cacByte[1])) {
        gy63SanSang = false;
        break;
      }
    }
    if (gy63SanSang) Serial.println("GY-63: CRC PROM=0, da doi chieu he so 2 lan");
  } else if (gy63SanSang) {
    Serial.println("GY-63: he so hieu chuan CRC OK");
  }
  if (!gy63SanSang) Serial.println("GY-63: he so hieu chuan khong hop le");
  if (gy63SanSang) {
    apSuatMocPa = 0;
    apSuatLocPa = 0;
    tongApSuatKhoiDongPa = 0;
    soMauKhoiDongGy63 = 0;
    lucDocGy63TiepTheoMs = millis();
  } else {
    lucDocGy63TiepTheoMs = millis() + 5000;
  }
}

// Đọc bộ chuyển đổi tương tự sang số 24 bit; dùng tham chiếu để trả kết quả và nhận biết lỗi đọc.
bool docAdcGy63(uint32_t &giaTriAdc) {
  uint8_t cacByte[3]; // Bộ chuyển đổi MS5611 có độ dài 24 bit, cần ba byte để ghép thành một số đo thô.
  if (!docByteGy63(0x00, cacByte, 3)) return false;
  giaTriAdc = (uint32_t(cacByte[0]) << 16) | (uint32_t(cacByte[1]) << 8) | cacByte[2];
  return giaTriAdc != 0 && giaTriAdc != 0xFFFFFF;
}

// Máy trạng thái GY-63 không chặn chương trình: bắt đầu chuyển đổi rồi đọc kết quả khi đủ thời gian.
void docGy63() {
  uint32_t hienTaiMs = millis(); // Dùng millis() cho thời hạn; khoảng chuyển đổi ngắn được đo riêng bằng micros().
  if (!gy63SanSang && int32_t(hienTaiMs - lucDocGy63TiepTheoMs) >= 0) khoiTaoGy63();
  if (!gy63SanSang) {
    mauGy63MoiNhat.hopLe = false;
    return;
  }

  if (giaiDoanGy63 == GiaiDoanGy63::RANH) {
    if (int32_t(hienTaiMs - lucDocGy63TiepTheoMs) < 0) return;
    if (guiLenhGy63(0x58)) { // D2: bắt đầu đo áp suất ở mức lấy mẫu 4096.
      lucBatDauChuyenDoiUs = micros();
      giaiDoanGy63 = GiaiDoanGy63::CHO_NHIET_DO;
      return;
    }
  } else if (uint32_t(micros() - lucBatDauChuyenDoiUs) < 10000) {
    return; // Mỗi phép đo cần tối đa 9,04 ms; không chặn việc lấy mẫu MPU.
  } else if (giaiDoanGy63 == GiaiDoanGy63::CHO_NHIET_DO) {
    if (docAdcGy63(nhietDoThoGy63) && guiLenhGy63(0x48)) {
      lucBatDauChuyenDoiUs = micros();
      giaiDoanGy63 = GiaiDoanGy63::CHO_AP_SUAT;
      return;
    }
  } else {
    uint32_t giaTriApSuatTho; // Số đo áp suất thô, chưa hiệu chỉnh, dùng làm đầu vào cho công thức PROM.
    if (docAdcGy63(giaTriApSuatTho)) {
      giaiDoanGy63 = GiaiDoanGy63::RANH;
      lucDocGy63TiepTheoMs = millis() + 20; // Lấy khoảng 25 cặp mẫu áp suất mỗi giây.
      uint32_t giaTriNhietDoTho = nhietDoThoGy63; // Giữ tên riêng cho số đo nhiệt độ thô lấy ở bước trước.
      // Biến trung gian 64 bit cần thiết vì phép bù nhiệt độ nhân các số đo thô có giá trị lớn.
      int64_t saiLechNhietDo = int64_t(giaTriNhietDoTho) - int64_t(heSoHieuChuanGy63[5]) * 256;
      // Nhiệt độ, độ lệch và độ nhạy là các giá trị trung gian theo công thức trong tài liệu MS5611.
      // Tách riêng để bù nhiệt độ thấp trước khi tính áp suất cuối cùng.
      int64_t nhietDo = 2000 + saiLechNhietDo * heSoHieuChuanGy63[6] / 8388608;
      int64_t doLech = int64_t(heSoHieuChuanGy63[2]) * 65536 + int64_t(heSoHieuChuanGy63[4]) * saiLechNhietDo / 128;
      int64_t doNhay = int64_t(heSoHieuChuanGy63[1]) * 32768 + int64_t(heSoHieuChuanGy63[3]) * saiLechNhietDo / 256;
      if (nhietDo < 2000) {
        // Các giá trị bình phương và bù dùng hiệu chỉnh lần hai khi cảm biến lạnh.
        int64_t binhPhuong = (nhietDo - 2000) * (nhietDo - 2000);
        int64_t buDoLech = 5 * binhPhuong / 2;
        int64_t buDoNhay = 5 * binhPhuong / 4;
        if (nhietDo < -1500) {
          binhPhuong = (nhietDo + 1500) * (nhietDo + 1500);
          buDoLech += 7 * binhPhuong;
          buDoNhay += 11 * binhPhuong / 2;
        }
        nhietDo -= saiLechNhietDo * saiLechNhietDo / 2147483648LL;
        doLech -= buDoLech;
        doNhay -= buDoNhay;
      }
      float apSuatPa = float((int64_t(giaTriApSuatTho) * doNhay / 2097152 - doLech) / 32768); // Áp suất đã hiệu chỉnh, tính bằng Pa.
      float nhietDoC = float(nhietDo) / 100.0f; // Tài liệu biểu diễn nhiệt độ theo 0,01 độ C; đổi về độ C.
      if (apSuatPa < 1000 || apSuatPa > 120000 || nhietDoC < -40 || nhietDoC > 85) {
        mauGy63MoiNhat.hopLe = false;
        return;
      }
      ++soMauGy63; // Chỉ đếm mẫu sau khi áp suất và nhiệt độ vượt qua kiểm tra miền hợp lý.
      if (soMauKhoiDongGy63 < 25) {
        tongApSuatKhoiDongPa += apSuatPa; // Cộng dồn để mốc nền không phụ thuộc vào một mẫu áp suất nhiễu.
        if (++soMauKhoiDongGy63 == 25) {
          apSuatMocPa = float(tongApSuatKhoiDongPa / 25);
          apSuatLocPa = apSuatPa;
          Serial.printf("GY-63 P0=%.1f Pa (25 mau)\n", apSuatMocPa);
        } else {
          mauGy63MoiNhat.hopLe = false;
          return;
        }
      }
      apSuatLocPa += 0.20f * (apSuatPa - apSuatLocPa);
      float chenhlechDoCaoM = 44330.0f * (1.0f - powf(apSuatLocPa / apSuatMocPa, 0.19029495f));
      mauGy63MoiNhat = {millis(), apSuatPa, nhietDoC, chenhlechDoCaoM, true};
      return;
    }
  }

  // Khi I2C hoặc bộ chuyển đổi lỗi, không dùng mẫu cũ để xác nhận ngã.
  giaiDoanGy63 = GiaiDoanGy63::RANH;
  gy63SanSang = false;
  lucDocGy63TiepTheoMs = millis() + 5000;
  mauGy63MoiNhat.hopLe = false;
  Serial.println("GY-63: doc ADC that bai, thu lai sau 5s");
}

// Đổi trạng thái nội bộ thành chuỗi ngắn để đọc nhật ký Serial; giữ nguyên chữ dùng bởi công cụ hiện tại.
const char *tenTrangThaiNga(TrangThaiNga trangThai) {
  switch (trangThai) {
    case TrangThaiNga::THEO_DOI: return "MONITORING";
    case TrangThaiNga::NGHI_NGO: return "SUSPECTED";
    case TrangThaiNga::XAC_MINH: return "VERIFYING";
    case TrangThaiNga::CANH_BAO: return "ALERT";
  }
  return "UNKNOWN";
}

// Tạo JSON SOS và đưa vào hàng đợi ưu tiên. Tham số cho biết mã sự kiện và đây có phải cảnh báo thử hay không.
void xepSuKienSos(uint16_t maSuKien, bool laThuNghiem) {
  // soLuong là số ký tự snprintf đã ghi; cần kiểm tra trước khi dùng làm độ dài dữ liệu.
  int soLuong = snprintf(boDemSos, sizeof(boDemSos),
      "{\"eventId\":\"%u\",\"deviceId\":\"FALLSAFE-%04X\",\"eventType\":\"SOS_PRESSED\",\"test\":%s}",
      maSuKien, (unsigned)(ESP.getEfuseMac() & 0xFFFF), laThuNghiem ? "true" : "false");
  if (soLuong <= 0 || size_t(soLuong) >= sizeof(boDemSos)) return;
  sosLaThuNghiem = laThuNghiem;
  maSuKienSos = maSuKien;
  doDaiSos = size_t(soLuong);
  viTriSos = 0;
  soKhungSos = 0;
  sosDangChoXacNhan = false;
  lucGuiSosGanNhatMs = 0;
  lucXepHangSosMs = millis();
  lucBatDauNghiCamBienMs = lucXepHangSosMs;
  dangNghiCamBienSauSos = true;
  doDaiDuLieu = 0; // Bỏ gói cảm biến đang dở để SOS không phải chờ phía sau.
  canGuiDuLieu.store(false);
}

// Mỗi lần gọi chỉ gửi một khung; notify() không có nghĩa là Android đã nhận được dữ liệu.
// Chờ Android gửi SOS_ACK:<eventId> qua kênh lệnh rồi mới xóa SOS khỏi hàng đợi.
// Thời hạn và mốc gửi lại ngăn chờ xác nhận hoặc gửi lặp vô hạn.
bool guiTungBuocSos(uint32_t hienTaiMs) {
  static uint32_t lucGuiKhungTruocMs = 0;
  if (doDaiSos != 0 && maSosDaXacNhan.exchange(0) == maSuKienSos) {
    Serial.printf("SOS Android da nhan: eventId=%u\n", maSuKienSos);
    doDaiSos = 0;
    sosDangChoXacNhan = false;
    return false;
  }
  if (doDaiSos != 0 && uint32_t(hienTaiMs - lucXepHangSosMs) >= THOI_HAN_XAC_NHAN_SOS_MS) {
    Serial.printf("SOS khong co Android ACK sau %lu ms, bo hang doi: eventId=%u\n",
                  (unsigned long)THOI_HAN_XAC_NHAN_SOS_MS, maSuKienSos);
    doDaiSos = 0;
    sosDangChoXacNhan = false;
    return false;
  }
  if (canGuiLaiSos.exchange(false)) {
    viTriSos = 0;
    soKhungSos = 0;
    sosDangChoXacNhan = false;
  }
  if (doDaiSos == 0 || !bleDaKetNoi.load() || !daDangKySuKien.load() ||
      !dacTinhSuKien) return false;
  if (sosDangChoXacNhan) {
    if (uint32_t(hienTaiMs - lucGuiSosGanNhatMs) < 1000) return false;
    viTriSos = 0;
    soKhungSos = 0;
    sosDangChoXacNhan = false;
    Serial.printf("SOS chua co ACK, gui lai: eventId=%u\n", maSuKienSos);
  }
  if (lucGuiKhungTruocMs != 0 && uint32_t(hienTaiMs - lucGuiKhungTruocMs) < 10) return true;

  uint16_t kichThuocMtu = mtuBle.load(); // Giới hạn kích thước gói, dùng tính phần dữ liệu của khung sắp gửi.
  if (kichThuocMtu < 23) kichThuocMtu = 23;
  if (viTriSos == 0) {
    tongKhungSos = doDaiSos <= kichThuocMtu - 3 ? 0 :
                    (doDaiSos + (kichThuocMtu - 19) - 1) / (kichThuocMtu - 19);
  }
  if (tongKhungSos == 0) {
    guiThongBaoSosLoi.store(false);
    dacTinhSuKien->setValue((uint8_t *)boDemSos, doDaiSos);
    dacTinhSuKien->notify();
    if (!guiThongBaoSosLoi.load() && bleDaKetNoi.load() && daDangKySuKien.load()) sosDangChoXacNhan = true;
  } else {
    size_t doDai = min(size_t(kichThuocMtu - 19), doDaiSos - viTriSos); // Đoạn dữ liệu không vượt giới hạn gói hoặc số byte còn lại.
    uint8_t khung[112] = {0x46, 0x53, 1, 2}; // Đầu khung IF-003 và dữ liệu; vùng đệm có kích thước cố định.
    uint32_t maSo = maSuKienSos; // Giao thức dùng mã 32 bit nên mở rộng mã nội bộ trước khi ghi vào đầu khung.
    khung[4] = maSo; khung[5] = maSo >> 8; khung[6] = maSo >> 16; khung[7] = maSo >> 24;
    khung[8] = soKhungSos; khung[9] = soKhungSos >> 8;
    khung[10] = tongKhungSos; khung[11] = tongKhungSos >> 8;
    khung[12] = doDaiSos; khung[13] = doDaiSos >> 8;
    khung[14] = viTriSos; khung[15] = viTriSos >> 8;
    memcpy(khung + 16, boDemSos + viTriSos, doDai);
    guiThongBaoSosLoi.store(false);
    dacTinhSuKien->setValue(khung, 16 + doDai);
    dacTinhSuKien->notify();
    if (!guiThongBaoSosLoi.load() && bleDaKetNoi.load() && daDangKySuKien.load()) {
      viTriSos += doDai;
      ++soKhungSos;
      if (viTriSos == doDaiSos) sosDangChoXacNhan = true;
    }
  }
  lucGuiKhungTruocMs = hienTaiMs;
  if (sosDangChoXacNhan) {
    lucGuiSosGanNhatMs = hienTaiMs;
    Serial.printf("SOS BLE da xep gui, cho Android ACK: eventId=%u test=%u\n", maSuKienSos, sosLaThuNghiem);
  }
  return true;
}

// Ghép mẫu mới của hai cảm biến, chạy bộ nhận dạng và chỉ tạo SOS một lần khi vừa xác nhận ngã.
void capNhatPhatHienNga(uint32_t hienTaiMs) {
  float doLonGiaTocMs2 = 0, doLonXoayDps = 0;
  bool imuHopLe = layMauMpuPhatHienNga(doLonGiaTocMs2, doLonXoayDps);
  bool mpuConMoi = mauMpuMoiNhat.hopLe && uint32_t(hienTaiMs - mauMpuMoiNhat.thoiDiemMs) <= boPhatHienNga.cauHinh.thoiHanMauMs;
  bool khiApKeHopLe = mauGy63MoiNhat.hopLe && uint32_t(hienTaiMs - mauGy63MoiNhat.thoiDiemMs) <= boPhatHienNga.cauHinh.thoiHanMauMs;
  TrangThaiNga truocDo = boPhatHienNga.trangThai; // Chỉ in nhật ký khi trạng thái đổi, tránh in quá nhiều ở mỗi mẫu.
  bool daXacNhan = boPhatHienNga.xuLyMau(hienTaiMs, doLonGiaTocMs2, doLonXoayDps,
                                     mauGy63MoiNhat.thayDoiDoCaoM, imuHopLe && mpuConMoi && khiApKeHopLe,
                                     mauGy63MoiNhat.apSuatPa);
  if (daXacNhan) {
    xepSuKienSos(boPhatHienNga.maSuKien);
  } else if (boPhatHienNga.trangThai != truocDo) {
    Serial.printf("FALL_STATE: %s\n", tenTrangThaiNga(boPhatHienNga.trangThai));
  }
}

// Kiểm tra cấu trúc, kiểu và miền giá trị trước khi ghi thông số; dauRa chỉ nhận cấu hình hoàn chỉnh, hợp lệ.
bool phanTichCauHinhNga(const char *chuoiJson, size_t doDai, CauHinhNga &dauRa) {
  // ketThuc giúp xác nhận đã phân tích hết JSON, không chấp nhận dữ liệu thừa ở cuối.
  const char *ketThuc = nullptr;
  cJSON *gocJson = cJSON_ParseWithLengthOpts(chuoiJson, doDai + 1, &ketThuc, true);
  if (!gocJson) return false;
  // Tên trường là quy ước JSON với Android, vì vậy phải giữ nguyên cách viết trong giao thức.
  const char *tenTruong[14] = {
      "impactAccelerationMs2", "stillnessTargetAccelerationMs2", "stillnessToleranceMs2",
      "postImpactWindowMs", "postImpactStillnessDurationMs", "minimumStillnessSamples",
      "maximumSampleGapMs", "freeFallThresholdMs2", "freeFallMinDurationMs",
      "gyroTurnThresholdDps", "pressureEvidenceMinRisePa", "pressureWindowMs",
      "altitudeDropMinM", "sampleWatchdogMs"};
  double cacGiaTri[14] = {}; // Mảng tạm để không sửa đầu ra nếu JSON chỉ hợp lệ một phần.
  bool hopLe = cJSON_IsObject(gocJson) && ketThuc == chuoiJson + doDai; // Cờ chung để các bước sau dừng khi phát hiện lỗi.
  if (hopLe) {
    int soLuong = 0; // Đếm tên trường để từ chối JSON thiếu trường hoặc có trường lạ.
    for (const cJSON *truong = gocJson->child; truong; truong = truong->next) ++soLuong;
    hopLe = soLuong == 14;
  }
  for (uint8_t chiSo = 0; hopLe && chiSo < 14; ++chiSo) {
    const cJSON *truong = cJSON_GetObjectItemCaseSensitive(gocJson, tenTruong[chiSo]); // Tìm tên trường có phân biệt hoa thường theo cấu trúc dữ liệu.
    hopLe = cJSON_IsNumber(truong) && isfinite(truong->valuedouble);
    if (hopLe) cacGiaTri[chiSo] = truong->valuedouble;
  }
  for (uint8_t chiSo : {3, 4, 5, 6, 8, 11, 13}) {
    if (hopLe && (cacGiaTri[chiSo] < 0 || cacGiaTri[chiSo] > 10000 || floor(cacGiaTri[chiSo]) != cacGiaTri[chiSo])) hopLe = false;
  }
  if (hopLe) {
    dauRa.nguongVaDapMs2 = cacGiaTri[0];
    dauRa.giaTocNamYenMs2 = cacGiaTri[1];
    dauRa.saiSoNamYenMs2 = cacGiaTri[2];
    dauRa.cuaSoSauVaDapMs = cacGiaTri[3];
    dauRa.thoiGianNamYenSauVaDapMs = cacGiaTri[4];
    dauRa.soMauNamYenToiThieu = cacGiaTri[5];
    dauRa.khoangDutMauToiDaMs = cacGiaTri[6];
    dauRa.nguongRoiTuDoMs2 = cacGiaTri[7];
    dauRa.thoiGianRoiTuDoToiThieuMs = cacGiaTri[8];
    dauRa.nguongXoayManhDps = cacGiaTri[9];
    dauRa.mucTangApSuatToiThieuPa = cacGiaTri[10];
    dauRa.cuaSoApSuatMs = cacGiaTri[11];
    dauRa.mucGiamDoCaoToiThieuM = cacGiaTri[12];
    dauRa.thoiHanMauMs = cacGiaTri[13];
    hopLe = dauRa.hopLe();
  }
  cJSON_Delete(gocJson);
  return hopLe;
}

// Trả kết quả cấu hình cho Android; chỉ gửi thông báo khi đã kết nối và dữ liệu vừa kích thước gói tối đa.
void guiXacNhanCauHinh(const char *trangThaiGui, const char *maLoi, const char *thongDiep) {
  if (!bleDaKetNoi.load() || !dacTinhXacNhan) return;
  char boDemXacNhan[240]; // Vùng đệm có giới hạn cho JSON xác nhận, không cần cấp phát bộ nhớ động.
  int soLuong = snprintf(boDemXacNhan, sizeof(boDemXacNhan),
      "{\"protocolVersion\":1,\"commandId\":\"profile\",\"deviceId\":\"FALLSAFE-%04X\","
      "\"timestampMs\":%lu,\"commandStatus\":\"%s\",\"errorCode\":%s,\"message\":\"%s\"}",
      (unsigned)(ESP.getEfuseMac() & 0xFFFF), (unsigned long)millis(), trangThaiGui, maLoi, thongDiep);
  if (soLuong > 0 && size_t(soLuong) < sizeof(boDemXacNhan) && soLuong <= mtuBle.load() - 3) {
    dacTinhXacNhan->setValue((uint8_t *)boDemXacNhan, soLuong);
    dacTinhXacNhan->notify();
  }
}

// Nạp bộ thông số từ NVS lúc khởi động; nếu chưa có hoặc bị lỗi thì giữ giá trị mặc định.
void napCauHinhNga() {
  if (!luuTruCauHinh.begin("fallsafe", false)) {
    Serial.println("FallProfile: NVS khong san sang, dung mac dinh");
    return;
  }
  if (luuTruCauHinh.getBytesLength("profile") != sizeof(CauHinhNga)) return;
  CauHinhNga cauHinhDaLuu; // Biến tạm, chỉ áp dụng sau khi đọc đủ byte và kiểm tra hợp lệ.
  if (luuTruCauHinh.getBytes("profile", &cauHinhDaLuu, sizeof(cauHinhDaLuu)) == sizeof(cauHinhDaLuu) && cauHinhDaLuu.hopLe()) {
    boPhatHienNga.apDungCauHinh(cauHinhDaLuu);
    Serial.println("FallProfile: da nap tu NVS");
  }
}

// Xử lý JSON thông số đã được hàm gọi lại đánh dấu; ghi NVS và cập nhật bộ nhận dạng khi dữ liệu hợp lệ.
void xuLyYeuCauCauHinh() {
  if (!coCauHinhChoXuLy.load()) return;
  if (!bleDaKetNoi.load()) {
    coCauHinhChoXuLy.store(false);
    return;
  }
  CauHinhNga cauHinhDuKien; // Bộ thông số tạm; không thay cấu hình đang chạy nếu phân tích hoặc ghi flash thất bại.
  if (!phanTichCauHinhNga(boDemCauHinh, doDaiCauHinh, cauHinhDuKien)) {
    guiXacNhanCauHinh("REJECTED", "\"INVALID_PROFILE\"", "Invalid profile");
  } else {
    cauHinhDuKien.nguongVaDapMs2 = NGUONG_VA_DAP_MS2;
    if (luuTruCauHinh.putBytes("profile", &cauHinhDuKien, sizeof(cauHinhDuKien)) != sizeof(cauHinhDuKien)) {
      guiXacNhanCauHinh("FAILED", "\"STORAGE_ERROR\"", "Could not save profile");
    } else {
      boPhatHienNga.apDungCauHinh(cauHinhDuKien);
      guiXacNhanCauHinh("COMPLETED", "null", "Applied and saved");
      Serial.println("FallProfile: da cap nhat va luu NVS");
    }
  }
  coCauHinhChoXuLy.store(false);
}

// Tạo một bản ghi JSON từ các mẫu cảm biến còn mới; cảm biến lỗi được gửi giá trị null thay vì dùng dữ liệu cũ.
void chuanBiDuLieu(uint32_t hienTaiMs) {
  bool mpuHopLe = mauMpuMoiNhat.hopLe && uint32_t(hienTaiMs - mauMpuMoiNhat.thoiDiemMs) <= 150;
  bool gy63HopLe = mauGy63MoiNhat.hopLe && uint32_t(hienTaiMs - mauGy63MoiNhat.thoiDiemMs) <= 200;
  // Dùng chuỗi để có thể biểu diễn cả số thực lẫn null trong JSON hợp lệ.
  char apSuatChuoi[24] = "null", nhietDo[24] = "null", doCaoChuoi[24] = "null";
  // Nếu áp kế chưa có mẫu mới, các trường vẫn mang giá trị null để JSON đúng cú pháp.
  if (gy63HopLe) {
    snprintf(apSuatChuoi, sizeof(apSuatChuoi), "%.1f", mauGy63MoiNhat.apSuatPa);
    snprintf(nhietDo, sizeof(nhietDo), "%.2f", mauGy63MoiNhat.nhietDoC);
    snprintf(doCaoChuoi, sizeof(doCaoChuoi), "%.3f", mauGy63MoiNhat.thayDoiDoCaoM);
  }
  char maSo[20]; // Mã thiết bị đi kèm mỗi gói để ứng dụng nhận biết nguồn dữ liệu cảm biến.
  snprintf(maSo, sizeof(maSo), "FALLSAFE-%04X", (unsigned)(ESP.getEfuseMac() & 0xFFFF));
  uint32_t soThuTu = ++soThuTuDuLieu; // Tăng số thứ tự mỗi lần tạo gói, kể cả khi phải chia thành nhiều khung.
  int soLuong; // Kết quả snprintf, dùng kiểm tra tràn vùng đệm và xác định độ dài gói.
  if (mpuHopLe) {
    constexpr float heSoDoiSangDoMoiGiay = 57.2957795f; // Đổi tốc độ xoay rad/s của MPU sang độ/giây theo định dạng Android.
    soLuong = snprintf(boDemDuLieu, sizeof(boDemDuLieu),
        "{\"protocolVersion\":1,\"deviceId\":\"%s\",\"sequenceNumber\":%lu,\"timestampMs\":%lu,"
        "\"accelXMs2\":%.3f,\"accelYMs2\":%.3f,\"accelZMs2\":%.3f,"
        "\"gyroXDps\":%.2f,\"gyroYDps\":%.2f,\"gyroZDps\":%.2f,"
        "\"pressurePa\":%s,\"temperatureC\":%s,\"altitudeDeltaM\":%s,"
        "\"batteryPercent\":-1,\"batteryVoltageMv\":null,\"isCharging\":false,"
        "\"sosButtonPressed\":false,\"sensorQuality\":%u}",
        maSo, (unsigned long)soThuTu, (unsigned long)hienTaiMs,
        mauMpuMoiNhat.giaTocX, mauMpuMoiNhat.giaTocY, mauMpuMoiNhat.giaTocZ,
        mauMpuMoiNhat.tocDoXoayX * heSoDoiSangDoMoiGiay, mauMpuMoiNhat.tocDoXoayY * heSoDoiSangDoMoiGiay, mauMpuMoiNhat.tocDoXoayZ * heSoDoiSangDoMoiGiay,
        apSuatChuoi, nhietDo, doCaoChuoi, gy63HopLe ? 100u : 50u);
  } else {
    soLuong = snprintf(boDemDuLieu, sizeof(boDemDuLieu),
        "{\"protocolVersion\":1,\"deviceId\":\"%s\",\"sequenceNumber\":%lu,\"timestampMs\":%lu,"
        "\"accelXMs2\":null,\"accelYMs2\":null,\"accelZMs2\":null,"
        "\"gyroXDps\":null,\"gyroYDps\":null,\"gyroZDps\":null,"
        "\"pressurePa\":%s,\"temperatureC\":%s,\"altitudeDeltaM\":%s,"
        "\"batteryPercent\":-1,\"batteryVoltageMv\":null,\"isCharging\":false,"
        "\"sosButtonPressed\":false,\"sensorQuality\":0}",
        maSo, (unsigned long)soThuTu, (unsigned long)hienTaiMs, apSuatChuoi, nhietDo, doCaoChuoi);
  }
  if (soLuong <= 0 || size_t(soLuong) >= sizeof(boDemDuLieu)) {
    doDaiDuLieu = 0;
    Serial.println("BLE: telemetry vuot bo dem");
    return;
  }
  doDaiDuLieu = size_t(soLuong);
  viTriDuLieu = 0;
  soKhungDuLieu = 0;
  uint16_t kichThuocMtu = mtuBle.load(); // Giới hạn kích thước thông báo đã thống nhất với Android.
  if (kichThuocMtu < 23) kichThuocMtu = 23;
  if (doDaiDuLieu <= kichThuocMtu - 3) {
    tongKhungDuLieu = 0; // JSON vừa một thông báo.
  } else {
    uint16_t kichThuocMang = kichThuocMtu - 19; // Phần còn lại sau đầu khung IF-003 dài 16 byte và phần tiêu đề ATT.
    tongKhungDuLieu = (doDaiDuLieu + kichThuocMang - 1) / kichThuocMang;
  }
}

// Gửi dữ liệu cảm biến theo yêu cầu; mỗi vòng loop chỉ gửi một khung để các tác vụ khác vẫn có thời gian chạy.
void guiTungBuocDuLieu(uint32_t hienTaiMs) {
  static uint32_t lucGuiKhungTruocMs = 0;
  if (!bleDaKetNoi.load()) {
    doDaiDuLieu = 0;
    canGuiDuLieu.store(false);
    return;
  }
  if (doDaiDuLieu == 0 && canGuiDuLieu.exchange(false)) chuanBiDuLieu(hienTaiMs);
  if (doDaiDuLieu == 0 || !dacTinhDuLieu ||
      (soKhungDuLieu > 0 && uint32_t(hienTaiMs - lucGuiKhungTruocMs) < 10)) return;

  if (tongKhungDuLieu == 0) {
    dacTinhDuLieu->setValue((uint8_t *)boDemDuLieu, doDaiDuLieu);
    dacTinhDuLieu->notify();
    doDaiDuLieu = 0;
  } else {
    uint16_t kichThuocMang = mtuBle.load() - 19; // Kích thước tối đa trừ phần đầu khung giao thức và tiêu đề ATT.
    size_t doDai = min(size_t(kichThuocMang), doDaiDuLieu - viTriDuLieu); // Số byte tối đa có thể đặt trong khung hiện tại.
    uint8_t khung[512] = {0x46, 0x53, 1, 1}; // Vùng đệm gồm đầu khung IF-003 và phần dữ liệu cảm biến.
    uint32_t maSo = soThuTuDuLieu; // Số thứ tự 32 bit trong đầu khung để Android ghép đúng các phần dữ liệu.
    khung[4] = maSo; khung[5] = maSo >> 8; khung[6] = maSo >> 16; khung[7] = maSo >> 24;
    khung[8] = soKhungDuLieu; khung[9] = soKhungDuLieu >> 8;
    khung[10] = tongKhungDuLieu; khung[11] = tongKhungDuLieu >> 8;
    khung[12] = doDaiDuLieu; khung[13] = doDaiDuLieu >> 8;
    khung[14] = viTriDuLieu; khung[15] = viTriDuLieu >> 8;
    memcpy(khung + 16, boDemDuLieu + viTriDuLieu, doDai);
    dacTinhDuLieu->setValue(khung, 16 + doDai);
    dacTinhDuLieu->notify();
    viTriDuLieu += doDai;
    ++soKhungDuLieu;
    if (viTriDuLieu == doDaiDuLieu) doDaiDuLieu = 0;
  }
  lucGuiKhungTruocMs = hienTaiMs;
}

// Arduino bắt đầu chương trình tại setup(); khởi tạo bus I2C và cảm biến trước khi bật BLE.
void setup() {
  Serial.begin(115200);
  delay(1000); // Chờ cổng USB nối tiếp sẵn sàng để không mất thông báo khởi động.
  busMpu.begin(8, 9, 100000); // Bật bus I2C của MPU theo chân phần cứng, ở tốc độ 100 kHz.
  busGy63.begin(6, 7, 100000); // Bật bus I2C riêng cho GY-63, cũng ở tốc độ 100 kHz.
  busMpu.setTimeOut(5);
  busGy63.setTimeOut(5);
  Serial.println("Kiem tra I2C ESP32-S3");
  // Nạp thông số trước khi đọc cảm biến để ngưỡng người dùng đã lưu có hiệu lực ngay.
  napCauHinhNga();
  boPhatHienNga.cauHinh.nguongVaDapMs2 = NGUONG_VA_DAP_MS2;
  khoiTaoMpu();
  kiemTraMpu();
  kiemTraGy63();
  khoiTaoGy63();
  khoiTaoBoNhoMpu();
  khoiTaoBle();
}

// Arduino gọi loop() liên tục để lấy mẫu theo lịch, xử lý SOS và gửi dữ liệu từng bước.
void loop() {
  // Các biến giữ mốc và số mẫu qua nhiều vòng để tính chu kỳ, tần suất mà không phải chờ lâu bằng delay.
  static uint32_t lucInMpuTruocMs = 0; // Mốc in nhật ký mỗi 3 giây và tính tần suất lấy mẫu.
  static uint32_t lucLayMauNgaTiepTheoMs = 0; // Thời điểm lấy mẫu nhận dạng tiếp theo theo chu kỳ 10 ms.
  static uint32_t soMauMpuTruoc = 0, soMauGy63Truoc = 0; // Số mẫu ở lần trước, dùng tính Hz trong chu kỳ nhật ký này.
  if (canQuangBaLai.exchange(false)) {
    BLEDevice::startAdvertising();
    Serial.println("BLE da quang ba lai sau ngat ket noi");
  }
  uint32_t hienTaiMs = millis(); // Mốc thời gian chung giúp các thời hạn trong vòng lặp nhất quán.
  if (dangNghiCamBienSauSos &&
      uint32_t(hienTaiMs - lucBatDauNghiCamBienMs) >= THOI_GIAN_NGHI_SAU_SOS_MS) {
    dangNghiCamBienSauSos = false;
    boPhatHienNga.trangThai = TrangThaiNga::THEO_DOI;
    boPhatHienNga.xoaUngVienNga();
    boPhatHienNga.daCoMau = false;
    mauMpuMoiNhat.hopLe = false;
    mauGy63MoiNhat.hopLe = false;
    giaiDoanGy63 = GiaiDoanGy63::RANH;
    lucLayMauNgaTiepTheoMs = hienTaiMs + 10;
    Serial.println("SOS: het 5 phut, tiep tuc doc cam bien.");
  }
  if (!dangNghiCamBienSauSos && int32_t(hienTaiMs - lucLayMauNgaTiepTheoMs) >= 0) {
    lucLayMauNgaTiepTheoMs = hienTaiMs + 10; // Không lấy bù các mẫu đã lỡ khi chương trình bị trễ.
    capNhatPhatHienNga(hienTaiMs);
  }
  if (!dangNghiCamBienSauSos) docGy63(); // Tạm dừng cả hai cảm biến ngay khi SOS được xếp hàng.
  bool sosDangXuLy = guiTungBuocSos(millis()); // Khi gửi SOS hoặc chờ xác nhận, tạm ưu tiên SOS hơn dữ liệu và lệnh thường.
  if (dangNghiCamBienSauSos) {
    canGuiDuLieu.store(false);
    delay(1);
    return;
  }
  if (!sosDangXuLy) {
    xuLyLenhSerialMpu();
    if (dangNghiCamBienSauSos) return;
    xuLyYeuCauCauHinh();
    guiTungBuocDuLieu(hienTaiMs);
  }
  if (uint32_t(hienTaiMs - lucInMpuTruocMs) >= 3000) {
    uint32_t thoiGianTroiQuaMs = hienTaiMs - lucInMpuTruocMs; // Thời gian thực dùng tính Hz nếu vòng lặp bị trễ.
    lucInMpuTruocMs = hienTaiMs;
    if (!sosDangXuLy && boPhatHienNga.trangThai == TrangThaiNga::THEO_DOI) docVaLuuMpu();
    if (mauGy63MoiNhat.hopLe) {
      Serial.printf("GY-63 P=%.1f Pa | T=%.2f C | dH=%+.3f m\n",
                    mauGy63MoiNhat.apSuatPa, mauGy63MoiNhat.nhietDoC,
                    mauGy63MoiNhat.thayDoiDoCaoM);
    } else {
      Serial.println("GY-63: chua co mau hop le");
    }
    Serial.printf("FALL: %s | sample_gaps=%lu\n", tenTrangThaiNga(boPhatHienNga.trangThai),
                  (unsigned long)boPhatHienNga.soLanDutMau);
    Serial.printf("SAMPLE_RATE MPU=%lu Hz GY63=%lu Hz\n",
                  (unsigned long)((soMauMpu - soMauMpuTruoc) * 1000 / thoiGianTroiQuaMs),
                  (unsigned long)((soMauGy63 - soMauGy63Truoc) * 1000 / thoiGianTroiQuaMs));
    soMauMpuTruoc = soMauMpu;
    soMauGy63Truoc = soMauGy63;
    Serial.printf("BLE: %s | SOS: %s\n\n",
                  bleDaKetNoi.load() ? "da ket noi" : "dang quang ba",
                  doDaiSos == 0 ? "khong cho" : (sosDangChoXacNhan ? "cho Android ACK" : "dang gui"));
  }
  delay(1);
}
