#include <Wire.h>

// ESP32-S3 Super Mini, theo esp.md
TwoWire &mpuBus = Wire;   // SDA 8, SCL 9
TwoWire &gyBus = Wire1;   // SDA 6, SCL 7

bool ping(TwoWire &bus, uint8_t addr) {
  bus.beginTransmission(addr);
  return bus.endTransmission() == 0;
}

void scanBus(TwoWire &bus, const char *name) {
  Serial.printf("%s: ", name);
  bool found = false;
  for (uint8_t addr = 1; addr < 127; ++addr) {
    if (ping(bus, addr)) {
      Serial.printf("0x%02X ", addr);
      found = true;
    }
  }
  if (!found) Serial.print("khong tim thay thiet bi");
  Serial.println();
}

bool readRegister(TwoWire &bus, uint8_t addr, uint8_t reg, uint8_t &value) {
  bus.beginTransmission(addr);
  bus.write(reg);
  if (bus.endTransmission(false) != 0) return false;
  if (bus.requestFrom(addr, (uint8_t)1) != 1) return false;
  value = bus.read();
  return true;
}

void testMPU() {
  uint8_t addr = ping(mpuBus, 0x68) ? 0x68 : (ping(mpuBus, 0x69) ? 0x69 : 0);
  if (!addr) {
    Serial.println("MPU-6050: khong phan hoi tai 0x68/0x69");
    return;
  }
  uint8_t id;
  if (!readRegister(mpuBus, addr, 0x75, id)) {
    Serial.println("MPU-6050: loi doc WHO_AM_I");
    return;
  }
  Serial.printf("MPU-6050: dia chi 0x%02X, WHO_AM_I=0x%02X %s\n",
                addr, id, (id == 0x68) ? "OK" : "(kiem tra lai chip)");
}

void testGY63() {
  uint8_t addr = ping(gyBus, 0x77) ? 0x77 : (ping(gyBus, 0x76) ? 0x76 : 0);
  if (!addr) {
    Serial.println("GY-63/MS5611: khong phan hoi tai 0x77/0x76");
    return;
  }
  gyBus.beginTransmission(addr);
  gyBus.write(0x1E); // Lenh reset MS5611
  if (gyBus.endTransmission() != 0) {
    Serial.println("GY-63/MS5611: loi reset");
    return;
  }
  delay(4);
  uint8_t hi, lo;
  gyBus.beginTransmission(addr);
  gyBus.write(0xA2); // PROM C1, he so hieu chuan ap suat
  if (gyBus.endTransmission(false) != 0 || gyBus.requestFrom(addr, (uint8_t)2) != 2) {
    Serial.println("GY-63/MS5611: loi doc PROM");
    return;
  }
  hi = gyBus.read();
  lo = gyBus.read();
  uint16_t c1 = ((uint16_t)hi << 8) | lo;
  Serial.printf("GY-63/MS5611: dia chi 0x%02X, PROM C1=%u %s\n",
                addr, c1, (c1 != 0 && c1 != 0xFFFF) ? "OK" : "(gia tri bat thuong)");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  mpuBus.begin(8, 9, 100000);
  gyBus.begin(6, 7, 100000);
  mpuBus.setTimeOut(50);
  gyBus.setTimeOut(50);
  Serial.println("Kiem tra I2C ESP32-S3");
}

void loop() {
  scanBus(mpuBus, "I2C0 MPU (SDA 8, SCL 9)");
  scanBus(gyBus, "I2C1 GY-63 (SDA 6, SCL 7)");
  testMPU();
  testGY63();
  Serial.println();
  delay(3000);
}
