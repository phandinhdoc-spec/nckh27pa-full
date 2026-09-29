#include <Arduino.h>
#include <Wire.h>

// ==========================================
// CẤU HÌNH GPIO THEO YÊU CẦU PHẦN CỨNG
// ==========================================
// Bus 0: GY-521 (MPU-6050)
#define I2C0_SDA_PIN    8
#define I2C0_SCL_PIN    9

// Bus 1: GY-63 (MS5611)
#define I2C1_SDA_PIN    7
#define I2C1_SCL_PIN    6

// Tần số I2C tiêu chuẩn (100 kHz để kiểm tra độ tin cậy kết nối)
#define I2C_FREQUENCY   100000

// Hàm nhận diện tên thiết bị theo địa chỉ I2C đã biết
const char* identifyDevice(uint8_t address, uint8_t busNum) {
  if (busNum == 0) {
    if (address == 0x68) return "MPU-6050 (GY-521 - AD0 = LOW / Hở)";
    if (address == 0x69) return "MPU-6050 (GY-521 - AD0 = HIGH)";
  } else if (busNum == 1) {
    if (address == 0x77) return "MS5611 (GY-63 - CSB = LOW / Hở)";
    if (address == 0x76) return "MS5611 (GY-63 - CSB = HIGH)";
  }
  return "Thiết bị I2C không xác định";
}

// Hàm quét toàn bộ bus I2C và in báo cáo chi tiết
int scanBus(TwoWire &i2cPort, uint8_t busNum, int sdaPin, int sclPin) {
  int deviceCount = 0;
  Serial.println(F("--------------------------------------------------"));
  Serial.printf(">>> QUÉT BUS I2C%d (SDA: GPIO %d | SCL: GPIO %d) <<<\n", busNum, sdaPin, sclPin);
  Serial.println(F("--------------------------------------------------"));

  for (uint8_t address = 1; address < 127; address++) {
    // Bắt đầu truyền thử nghiệm tới địa chỉ
    i2cPort.beginTransmission(address);
    uint8_t error = i2cPort.endTransmission();

    if (error == 0) {
      Serial.printf("[+] Tìm thấy thiết bị tại địa chỉ: 0x%02X -> %s\n", 
                    address, identifyDevice(address, busNum));
      deviceCount++;
    } else if (error == 4) {
      Serial.printf("[!] Lỗi không xác định tại địa chỉ 0x%02X\n", address);
    }
  }

  if (deviceCount == 0) {
    Serial.printf("[-] KẾT QUẢ: Không tìm thấy bất kỳ thiết bị nào trên Bus I2C%d!\n", busNum);
    Serial.println(F("    Gợi ý kiểm tra:"));
    Serial.printf("    - Kiểm tra dây nối: SDA -> GPIO %d, SCL -> GPIO %d\n", sdaPin, sclPin);
    Serial.println(F("    - Kiểm tra nguồn cấp (VCC) và mass (GND) của module."));
    Serial.println(F("    - Kiểm tra tiếp xúc dây cắm (breadboard/mối hàn)."));
  } else {
    Serial.printf("[OK] KẾT QUẢ: Phát hiện thành công %d thiết bị trên Bus I2C%d.\n", deviceCount, busNum);
  }
  Serial.println();
  return deviceCount;
}

void setup() {
  // Khởi tạo Serial giao tiếp máy tính
  Serial.begin(115200);

  // Chờ kết nối Serial trên ESP32-S3 (hỗ trợ USB CDC trên macOS)
  unsigned long startMillis = millis();
  while (!Serial && (millis() - startMillis < 3000)) {
    delay(10);
  }

  Serial.println(F("\n=================================================="));
  Serial.println(F("  ESP32-S3 SUPER MINI - DUAL I2C HARDWARE TEST    "));
  Serial.println(F("=================================================="));

  // Khởi tạo Bus 0 (Wire) cho GY-521
  Serial.println(F("[*] Đang khởi tạo I2C0 cho GY-521..."));
  bool i2c0_ok = Wire.begin(I2C0_SDA_PIN, I2C0_SCL_PIN, I2C_FREQUENCY);
  if (!i2c0_ok) {
    Serial.println(F("[LỖI] Không thể khởi tạo ngoại vi Wire (I2C0)!"));
  }

  // Khởi tạo Bus 1 (Wire1) cho GY-63
  Serial.println(F("[*] Đang khởi tạo I2C1 cho GY-63..."));
  bool i2c1_ok = Wire1.begin(I2C1_SDA_PIN, I2C1_SCL_PIN, I2C_FREQUENCY);
  if (!i2c1_ok) {
    Serial.println(F("[LỖI] Không thể khởi tạo ngoại vi Wire1 (I2C1)!"));
  }

  Serial.println(F("[*] Cả 2 bus đã sẵn sàng. Bắt đầu quét...\n"));
}

void loop() {
  Serial.printf("\n--- BẮT ĐẦU CHU KỲ QUÉT (Thời gian: %lu ms) ---\n", millis());

  // Quét I2C0 (GY-521)
  int devI2C0 = scanBus(Wire, 0, I2C0_SDA_PIN, I2C0_SCL_PIN);

  // Quét I2C1 (GY-63)
  int devI2C1 = scanBus(Wire1, 1, I2C1_SDA_PIN, I2C1_SCL_PIN);

  // Đánh giá tổng hợp trạng thái
  Serial.println(F("================ TỔNG KẾT HỆ THỐNG ================"));
  if (devI2C0 > 0 && devI2C1 > 0) {
    Serial.println(F("[THÀNH CÔNG] Cả 2 cảm biến GY-521 và GY-63 đều được nhận diện!"));
  } else {
    Serial.println(F("[CẢNH BÁO] Hệ thống chưa nhận đủ 2 module:"));
    if (devI2C0 == 0) Serial.println(F("  -> GY-521 (I2C0) CHƯA phản hồi."));
    if (devI2C1 == 0) Serial.println(F("  -> GY-63 (I2C1) CHƯA phản hồi."));
  }
  Serial.println(F("===================================================="));

  // Tạm dừng 5 giây trước khi thực hiện lần quét tiếp theo
  Serial.println(F("Chờ 5 giây cho lượt quét tiếp theo...\n"));
  delay(5000);
}