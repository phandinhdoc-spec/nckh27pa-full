// ============================================================================
// DU AN: NCKH27PA - V5. ESP32-S3 Super Mini Fall Detection
// Hardware: ESP32-S3 Super Mini, MPU-6050 (I2C0), MS5611/GY-63 (I2C1), IP5306
// Protocol: Pure BLE GATT (No WiFi), Custom App Protocol, Dual-Bus Isolated
// ============================================================================
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MS5611.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <math.h>
#include <ctype.h>

// --- CAU HINH CHAN GPIO & DIA CHI I2C ----------------------------------------
// Bus I2C0 doc lap cho MPU6050 va IP5306
static const int CHAN_I2C0_SDA = 8;
static const int CHAN_I2C0_SCL = 9;

// Bus I2C1 doc lap cho MS5611 (GY-63)
static const int CHAN_I2C1_SDA = 6;
static const int CHAN_I2C1_SCL = 7;

// LED chi thi tren bo mach ESP32-S3 Super Mini
static const int CHAN_LED_BOARD = 21;

#define DIA_CHI_MPU6050_A  0x68
#define DIA_CHI_MPU6050_B  0x69
#define DIA_CHI_MS5611_A   0x77  // CSB -> GND (Mac dinh tren module GY-63)
#define DIA_CHI_MS5611_B   0x76  // CSB -> 3V3
#define DIA_CHI_IP5306     0x75

// --- BLE UUID GIAO TIEP APP FALLSAFE ---------------------------------------
static const char *UUID_SERVICE = "7d2a0001-6f45-4c2b-9a1e-38a8f5c10001";
static const char *UUID_STREAM  = "7d2a0002-6f45-4c2b-9a1e-38a8f5c10001"; // Notify
static const char *UUID_EVENT   = "7d2a0003-6f45-4c2b-9a1e-38a8f5c10001"; // Notify
static const char *UUID_COMMAND = "7d2a0005-6f45-4c2b-9a1e-38a8f5c10001"; // Write / WriteNR
static const char *UUID_ACK     = "7d2a0006-6f45-4c2b-9a1e-38a8f5c10001"; // Notify

static constexpr float G = 9.80665f;

// --- PROFILE CAU HINH PHAT HIEN NGA ----------------------------------------
struct CauHinh {
  float lowG, impact, gyro, deltaH;
  uint32_t windowMs, stillMs;
  float stillTarget, stillTolerance, pressureRisePa;
  uint32_t minStillSamples, maxSampleGapMs, lowDurationMs;
  uint32_t pressureWindowMs, sampleWatchdogMs;
  bool cheDoThuNghiem;
};

static const CauHinh NORMAL = {
  0.50f, 25.0f, 120.0f, -0.40f, 3000, 1000,
  9.81f, 1.0f, 12.0f, 6, 250, 80, 5000, 100, false
};
static const CauHinh TEST = {
  0.75f, 15.0f, 60.0f, -0.15f, 4000, 500,
  9.81f, 1.0f, 6.0f, 4, 250, 30, 4000, 100, true
};
static CauHinh cauHinh = NORMAL;

// --- BIEN TRANG THAI DU LIEU & CAM BIEN ------------------------------------
struct Mau {
  float ax, ay, az, gx, gy, gz, doLonGiaToc, vanTocGoc;
  uint32_t timestampMs;
  bool hopLe;
};
static Mau mau = {};

static bool mpuOk = false, gy63Ok = false, coMoc = false, coKhiAp = false;
static bool ketNoi = false, streamEnabled = true;
static uint8_t loiMpu = 0, loiGy63 = 0;
static float apSuat = 0, nhietDo = 0, apSuatMoc = 0, doCaoTuongDoi = 0;
static float tongMoc = 0;
static uint8_t soMauMoc = 0;
static uint32_t lucBaro = 0, lucThuLai = 0, lucIn = 0, lucBle = 0, lucPin = 0;
static uint32_t lucLowG = 0, lucImpact = 0, lucBatDong = 0;
static uint32_t lucMauHopLe = 0, lucApSuatGoc = 0, soMauBatDong = 0;
static float apSuatGoc = 0;
static uint32_t soThuTu = 0, soKhung = 0, mocLayMauUs = 0;
static bool dangNghiNgo = false, daCoLowG = false, daBaoLowG = false, daBaoBatDong = false;

// Quan ly Pin IP5306
static int phanTramPin = 100;
static bool dangSacPin = false;
static bool coIp5306 = false;
static uint8_t diaChiIp5306 = 0;

// --- BO DEM LENH THREAD-SAFE (Ring Buffer) ----------------------------------
#define SO_SLOT_LENH 4
#define DO_DAI_LENH 768
static char lenhCho[SO_SLOT_LENH][DO_DAI_LENH];
static volatile uint8_t dauDoc = 0, dauGhi = 0;
static portMUX_TYPE khoaHangDoi = portMUX_INITIALIZER_UNLOCKED;

// BLE Entities
static BLECharacteristic *streamChar = nullptr;
static BLECharacteristic *eventChar  = nullptr;
static BLECharacteristic *ackChar    = nullptr;
static BLEServer *mayChu             = nullptr;
static volatile bool quangBaCho      = false;
static char ackCommandId[65]         = "profile";
static char tenBle[24]               = "FALLSAFE-ESP32S3";

// ============================================================================
// I2C BUS RECOVERY - GIAI PHONG BUS KHI BI KHOA MAT DONG BO XUNG NHIP
// ============================================================================
void phucHoiBusI2C(int sdaPin, int sclPin) {
  pinMode(sdaPin, INPUT_PULLUP);
  pinMode(sclPin, OUTPUT_OPEN_DRAIN);
  digitalWrite(sclPin, HIGH);
  delayMicroseconds(10);

  // Phat 9 xung clock de Slave nha duong SDA neu dang giu muc LOW
  for (int i = 0; i < 9; i++) {
    if (digitalRead(sdaPin) == HIGH) break;
    digitalWrite(sclPin, LOW);
    delayMicroseconds(10);
    digitalWrite(sclPin, HIGH);
    delayMicroseconds(10);
  }

  // Tao dieu kien STOP tren bus
  pinMode(sdaPin, OUTPUT_OPEN_DRAIN);
  digitalWrite(sdaPin, LOW);
  delayMicroseconds(10);
  digitalWrite(sclPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sdaPin, HIGH);
  delayMicroseconds(10);

  pinMode(sdaPin, INPUT);
  pinMode(sclPin, INPUT);
}

// ============================================================================
// DRIVER MPU-6050 (BUS I2C0: SDA=8, SCL=9)
// ============================================================================
static Adafruit_MPU6050 s_mpu;
static uint8_t s_diaChiMpu = 0;
static bool s_mpuCheDoTrucTiep = false;

static uint8_t docThanhGhiI2C0(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0xFF;
  if (Wire.requestFrom(addr, (uint8_t)1) != 1) return 0xFF;
  return Wire.read();
}

static bool ghiThanhGhiI2C0(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  return (Wire.endTransmission() == 0);
}

bool khoiTaoMPU() {
  s_mpuCheDoTrucTiep = false;
  uint8_t ds[] = {DIA_CHI_MPU6050_A, DIA_CHI_MPU6050_B};

  for (int i = 0; i < 2; ++i) {
    uint8_t addr = ds[i];
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() != 0) continue;

    uint8_t whoami = docThanhGhiI2C0(addr, 0x75);
    Serial.printf("[MPU_PROBE] Addr 0x%02X WHO_AM_I=0x%02X\n", addr, whoami);

    if (s_mpu.begin(addr, &Wire)) {
      s_diaChiMpu = addr;
      s_mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
      s_mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
      s_mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);
      Serial.printf("[MPU] Adafruit Driver OK tai 0x%02X\n", addr);
      return mpuOk = true;
    }

    // Fallback doc thanh ghi truc tiep cho chip clone tu choi dinh danh
    ghiThanhGhiI2C0(addr, 0x6B, 0x00); delay(15);
    ghiThanhGhiI2C0(addr, 0x6B, 0x01); // PLL Gyro X
    ghiThanhGhiI2C0(addr, 0x19, 0x00); // 1kHz
    ghiThanhGhiI2C0(addr, 0x1A, 0x02); // DLPF 94Hz
    ghiThanhGhiI2C0(addr, 0x1B, 0x18); // +-2000 dps
    ghiThanhGhiI2C0(addr, 0x1C, 0x18); // +-16g
    s_diaChiMpu = addr;
    s_mpuCheDoTrucTiep = true;
    Serial.printf("[MPU] DIRECT REG OK tai 0x%02X (+-16g +-2000dps)\n", addr);
    return mpuOk = true;
  }
  s_diaChiMpu = 0;
  Serial.println("[MPU] Khong tim thay cam bien tai 0x68 hoac 0x69");
  return mpuOk = false;
}

static bool docMPUVaoMau() {
  if (!mpuOk) return false;
  constexpr float ACC_SCALE = G / 2048.0f;
  constexpr float GYRO_SCALE = 1.0f / 16.4f;

  if (s_mpuCheDoTrucTiep) {
    Wire.beginTransmission(s_diaChiMpu);
    Wire.write(0x3B);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(s_diaChiMpu, (uint8_t)14) != 14) return false;

    int16_t rawAx = (Wire.read() << 8) | Wire.read();
    int16_t rawAy = (Wire.read() << 8) | Wire.read();
    int16_t rawAz = (Wire.read() << 8) | Wire.read();
    Wire.read(); Wire.read(); // Bo qua thanh ghi nhiet do
    int16_t rawGx = (Wire.read() << 8) | Wire.read();
    int16_t rawGy = (Wire.read() << 8) | Wire.read();
    int16_t rawGz = (Wire.read() << 8) | Wire.read();

    mau.ax = rawAx * ACC_SCALE; mau.ay = rawAy * ACC_SCALE; mau.az = rawAz * ACC_SCALE;
    mau.gx = rawGx * GYRO_SCALE; mau.gy = rawGy * GYRO_SCALE; mau.gz = rawGz * GYRO_SCALE;
  } else {
    sensors_event_t a, g, temp;
    if (!s_mpu.getEvent(&a, &g, &temp)) return false;
    mau.ax = a.acceleration.x; mau.ay = a.acceleration.y; mau.az = a.acceleration.z;
    mau.gx = g.gyro.x * (180.0f / PI);
    mau.gy = g.gyro.y * (180.0f / PI);
    mau.gz = g.gyro.z * (180.0f / PI);
  }

  mau.doLonGiaToc = sqrtf(mau.ax*mau.ax + mau.ay*mau.ay + mau.az*mau.az);
  mau.vanTocGoc   = sqrtf(mau.gx*mau.gx + mau.gy*mau.gy + mau.gz*mau.gz);
  mau.timestampMs = millis();
  mau.hopLe = isfinite(mau.doLonGiaToc) && isfinite(mau.vanTocGoc);
  return mau.hopLe;
}

// ============================================================================
// DRIVER MS5611 (BUS I2C1: SDA=7, SCL=6) - CACH LY HOAN TOAN VOI I2C0
// ============================================================================
static MS5611 ms5611_77(DIA_CHI_MS5611_A, &Wire1);
static MS5611 ms5611_76(DIA_CHI_MS5611_B, &Wire1);
static MS5611 *ms5611 = nullptr;

static bool thuMS5611(MS5611 *sensor, uint8_t addr) {
  if (!sensor->begin()) {
    return false;
  }
  sensor->reset(1); // mathMode=1 de tra ve Pascal
  sensor->setOversampling(OSR_LOW);
  if (sensor->read() != MS5611_READ_OK) {
    return false;
  }

  float p = sensor->getPressure();
  float t = sensor->getTemperature();
  if (!isfinite(p) || p < 30000 || p > 120000 || !isfinite(t) || t < -40 || t > 85) {
    return false;
  }

  ms5611 = sensor;
  loiGy63 = 0;
  coKhiAp = false;
  coMoc = false;
  soMauMoc = 0;
  tongMoc = 0;
  Serial.printf("[GY63] MS5611 OK tai 0x%02X tren Wire1 (SDA=7, SCL=6) | P=%.2f hPa, T=%.2fC\n",
                addr, p / 100.0f, t);
  return true;
}

bool khoiTaoGY63() {
  ms5611 = nullptr;
  gy63Ok = false;
  loiGy63 = 0;

  // Thu dia chi 0x77 (mac dinh GY-63 khi CSB co tro keo GND)
  if (thuMS5611(&ms5611_77, DIA_CHI_MS5611_A)) {
    return gy63Ok = true;
  }
  // Thu tiep dia chi 0x76 (khi CSB duoc keo HIGH)
  if (thuMS5611(&ms5611_76, DIA_CHI_MS5611_B)) {
    return gy63Ok = true;
  }

  Serial.println("[GY63] Khong tim thay MS5611 tai 0x77 hoac 0x76 tren Wire1 (SDA 7, SCL 6).");
  Serial.println("       Kiem tra: VCC GY-63 can 5V (hoac 3V3), chan PS de ho, CSB noi GND.");
  return gy63Ok = false;
}

void docGY63(uint32_t now) {
  if (!gy63Ok || !ms5611 || (now - lucBaro < 40)) return;
  lucBaro = now;

  if (ms5611->read() != MS5611_READ_OK) {
    coKhiAp = false;
    if (++loiGy63 >= 5) {
      gy63Ok = false;
      coMoc = false;
      Serial.println("[GY63] Loi mat ket noi sensor");
    }
    return;
  }

  float p = ms5611->getPressure();
  float t = ms5611->getTemperature();

  if (!isfinite(p) || p < 30000 || p > 120000 || !isfinite(t)) {
    coKhiAp = false;
    if (++loiGy63 >= 5) {
      gy63Ok = false;
      coMoc = false;
      Serial.println("[GY63] Du lieu ap suat ngoai pham vi hop le");
    }
    return;
  }

  loiGy63 = 0;
  coKhiAp = true;
  apSuat = p;
  nhietDo = t;

  if (!coMoc) {
    tongMoc += p;
    if (++soMauMoc >= 30) {
      apSuatMoc = tongMoc / soMauMoc;
      coMoc = true;
      Serial.printf("[GY63] Da xac lap moc ap suat chuan: %.1f Pa\n", apSuatMoc);
    }
  }

  if (coMoc && apSuatMoc > 0) {
    doCaoTuongDoi = 44330.77f * (1.0f - powf(p / apSuatMoc, 0.190263f));
  }
}

// ============================================================================
// DRIVER IP5306 (BUS I2C0: SDA=8, SCL=9)
// ============================================================================
static bool timIp5306() {
  const uint8_t ds[] = {DIA_CHI_IP5306, 0x74, 0x73, 0x72};
  for (uint8_t addr : ds) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      diaChiIp5306 = addr;
      return true;
    }
  }
  diaChiIp5306 = 0;
  return false;
}

static bool docIP5306() {
  if (!diaChiIp5306 && !timIp5306()) {
    coIp5306 = false;
    return false;
  }

  Wire.beginTransmission(diaChiIp5306);
  Wire.write(0x78);
  if (Wire.endTransmission() != 0) return false;
  if (Wire.requestFrom(diaChiIp5306, (uint8_t)1) != 1) return false;

  uint8_t reg78 = Wire.read();
  switch (reg78 & 0xF0) {
    case 0x00: phanTramPin = 100; break;
    case 0x80: phanTramPin = 75;  break;
    case 0xC0: phanTramPin = 50;  break;
    case 0xE0: phanTramPin = 25;  break;
    default:   phanTramPin = 10;  break;
  }

  Wire.beginTransmission(diaChiIp5306);
  Wire.write(0x70);
  if (Wire.endTransmission() == 0 && Wire.requestFrom(diaChiIp5306, (uint8_t)1) == 1) {
    uint8_t reg70 = Wire.read();
    dangSacPin = (reg70 & 0x08) != 0;
  } else {
    dangSacPin = false;
  }

  coIp5306 = true;
  return true;
}

// ============================================================================
// BLE TRANSMISSION PROTOCOL (Phan manh MTU thich ung, khong chan MTU nho)
// ============================================================================
void guiKhung(BLECharacteristic *characteristic, uint8_t kind, const char *json) {
  if (!ketNoi || !characteristic) return;
  size_t length = strlen(json);
  if (length == 0 || length > 1024) return;

  uint16_t mtu = 23;
  if (mayChu) {
    mtu = mayChu->getPeerMTU(mayChu->getConnId());
  }
  if (mtu < 23) mtu = 23;
  if (mtu > 517) mtu = 517;

  // Header giao thuc app chiem 16 byte. Tinh toan chunk size an toan theo MTU:
  size_t chunk = (mtu > 19) ? (mtu - 19) : 4;
  uint16_t count = (uint16_t)((length + chunk - 1) / chunk);
  uint32_t id = ++soKhung;

  for (uint16_t i = 0; i < count; ++i) {
    size_t offset = i * chunk;
    size_t n = (i == count - 1) ? (length - offset) : chunk;

    uint8_t frame[514] = {0x46, 0x53, 1, kind}; // Header: 'F', 'S', ver=1, kind
    for (int b = 0; b < 4; ++b) frame[4 + b] = (uint8_t)(id >> (8 * b));
    frame[8]  = (uint8_t)(i & 0xFF);
    frame[9]  = (uint8_t)((i >> 8) & 0xFF);
    frame[10] = (uint8_t)(count & 0xFF);
    frame[11] = (uint8_t)((count >> 8) & 0xFF);
    frame[12] = (uint8_t)(length & 0xFF);
    frame[13] = (uint8_t)((length >> 8) & 0xFF);
    frame[14] = (uint8_t)(offset & 0xFF);
    frame[15] = (uint8_t)((offset >> 8) & 0xFF);
    memcpy(frame + 16, json + offset, n);

    characteristic->setValue(frame, 16 + (uint16_t)n);
    characteristic->notify();

    // Pacing delay ngan khi phan manh de tranh tran hang doi BLE TX
    if (count > 1) {
      delay(4);
    }
  }
}

void guiAck(bool ok, const char *lyDo) {
  char json[320];
  snprintf(json, sizeof(json),
    "{\"protocolVersion\":1,\"commandId\":\"%s\",\"deviceId\":\"%s\",\"timestampMs\":%lu,"
    "\"commandStatus\":\"%s\",\"message\":\"%s\"}",
    ackCommandId, tenBle, (unsigned long)millis(), ok ? "COMPLETED" : "REJECTED", lyDo);
  guiKhung(ackChar, 5, json);
}

void guiEvent(const char *type, int confidence) {
  char json[360];
  snprintf(json, sizeof(json),
    "{\"protocolVersion\":1,\"eventId\":\"evt-%lu\",\"deviceId\":\"%s\",\"sequenceNumber\":%lu,"
    "\"timestampMs\":%lu,\"eventType\":\"%s\",\"eventSeverity\":\"WARNING\","
    "\"sosButtonPressed\":false,\"eventConfidence\":%d}",
    (unsigned long)millis(), tenBle, (unsigned long)++soThuTu,
    (unsigned long)millis(), type, confidence);
  guiKhung(eventChar, 2, json);
}

void guiTrangThai() {
  if (!ketNoi || !ackChar) return;
  char json[320];
  snprintf(json, sizeof(json),
    "{\"protocolVersion\":1,\"deviceId\":\"%s\",\"timestampMs\":%lu,\"batteryPercent\":%d,"
    "\"isCharging\":%s,\"imuStatus\":\"%s\",\"barometerStatus\":\"%s\",\"freeHeap\":%u}",
    tenBle, (unsigned long)millis(), phanTramPin, dangSacPin ? "true" : "false",
    mpuOk ? "OK" : "ERROR", gy63Ok ? "OK" : "UNAVAILABLE", ESP.getFreeHeap());
  guiKhung(ackChar, 5, json);
}

void guiTelemetry() {
  if (!ketNoi || !streamEnabled) return;
  if (millis() - mau.timestampMs > cauHinh.sampleWatchdogMs) mau.hopLe = false;
  if (millis() - lucBaro > 150) coKhiAp = false;

  char imu[200], baro[160], json[1024];
  if (mau.hopLe) {
    snprintf(imu, sizeof(imu),
      "%.3f,\"accelYMs2\":%.3f,\"accelZMs2\":%.3f,\"gyroXDps\":%.2f,\"gyroYDps\":%.2f,\"gyroZDps\":%.2f",
      mau.ax, mau.ay, mau.az, mau.gx, mau.gy, mau.gz);
  } else {
    snprintf(imu, sizeof(imu),
      "null,\"accelYMs2\":null,\"accelZMs2\":null,\"gyroXDps\":null,\"gyroYDps\":null,\"gyroZDps\":null");
  }

  if (coKhiAp && coMoc) {
    snprintf(baro, sizeof(baro), "%.1f,\"temperatureC\":%.2f,\"altitudeDeltaM\":%.3f",
             apSuat, nhietDo, doCaoTuongDoi);
  } else if (coKhiAp) {
    snprintf(baro, sizeof(baro), "%.1f,\"temperatureC\":%.2f,\"altitudeDeltaM\":null",
             apSuat, nhietDo);
  } else {
    snprintf(baro, sizeof(baro), "null,\"temperatureC\":null,\"altitudeDeltaM\":null");
  }

  char mag[24], omega[24];
  if (mau.hopLe) {
    snprintf(mag, sizeof(mag), "%.3f", mau.doLonGiaToc);
    snprintf(omega, sizeof(omega), "%.2f", mau.vanTocGoc);
  } else {
    strcpy(mag, "null");
    strcpy(omega, "null");
  }

  snprintf(json, sizeof(json),
    "{\"protocolVersion\":1,\"deviceId\":\"%s\",\"sequenceNumber\":%lu,\"timestampMs\":%lu,"
    "\"accelXMs2\":%s,\"pressurePa\":%s,\"batteryPercent\":%d,\"batteryVoltageMv\":null,"
    "\"isCharging\":%s,\"sosButtonPressed\":false,\"sensorQuality\":%d,"
    "\"accelerationMagnitudeMs2\":%s,\"angularSpeedDps\":%s,\"espState\":\"%s\",\"testMode\":%s}",
    tenBle, (unsigned long)++soThuTu, (unsigned long)mau.timestampMs, imu, baro,
    phanTramPin, dangSacPin ? "true" : "false",
    mau.hopLe ? (coKhiAp ? 100 : 80) : 0, mag, omega,
    dangNghiNgo ? "CANDIDATE" : "NORMAL",
    cauHinh.cheDoThuNghiem ? "true" : "false");

  guiKhung(streamChar, 1, json);
}

// ============================================================================
// PARSER JSON THU CON (Zero-allocation, an toan bo nho)
// ============================================================================
static bool docSo(const char *json, const char *key, double &value) {
  char ten[64]; snprintf(ten, sizeof(ten), "\"%s\"", key);
  const char *p = strstr(json, ten);
  if (!p || strstr(p + strlen(ten), ten)) return false;
  p += strlen(ten); while (isspace((unsigned char)*p)) ++p;
  if (*p++ != ':') return false;
  while (isspace((unsigned char)*p)) ++p;
  char *end = nullptr; value = strtod(p, &end);
  return end != p && isfinite(value) && (*end == ',' || *end == '}' || isspace((unsigned char)*end));
}

static bool docBool(const char *json, const char *key, bool &value) {
  char ten[64]; snprintf(ten, sizeof(ten), "\"%s\"", key);
  const char *p = strstr(json, ten);
  if (!p || strstr(p + strlen(ten), ten)) return false;
  p += strlen(ten); while (isspace((unsigned char)*p)) ++p;
  if (*p++ != ':') return false;
  while (isspace((unsigned char)*p)) ++p;
  if (strncmp(p, "true", 4) == 0) { value = true; return true; }
  if (strncmp(p, "false", 5) == 0) { value = false; return true; }
  return false;
}

static bool docChuoi(const char *json, const char *key, char *out, size_t cap) {
  char ten[64]; snprintf(ten, sizeof(ten), "\"%s\"", key);
  const char *p = strstr(json, ten);
  if (!p || strstr(p + strlen(ten), ten)) return false;
  p += strlen(ten); while (isspace((unsigned char)*p)) ++p;
  if (*p++ != ':') return false;
  while (isspace((unsigned char)*p)) ++p;
  if (*p++ != '"') return false;
  const char *end = strchr(p, '"');
  if (!end || (size_t)(end - p) >= cap) return false;
  memcpy(out, p, end - p); out[end - p] = 0; return true;
}

static void apDungProfile(const char *json, bool &coThayDoi) {
  double v;
  bool testMode = cauHinh.cheDoThuNghiem;
  if (strstr(json, "\"cheDoThuNghiem\"") != nullptr && docBool(json, "cheDoThuNghiem", testMode)) {
    coThayDoi = true;
  }
  cauHinh.cheDoThuNghiem = testMode;

  if (docSo(json, "impactAccelerationMs2", v) && v >= 10 && v <= 120)
    { cauHinh.impact = (float)v; coThayDoi = true; }
  if (docSo(json, "freeFallThresholdMs2", v) && v >= 0.1 && v <= 9)
    { cauHinh.lowG = (float)(v / G); coThayDoi = true; }
  if (docSo(json, "gyroTurnThresholdDps", v) && v >= 10 && v <= 1800)
    { cauHinh.gyro = (float)v; coThayDoi = true; }
  if (docSo(json, "altitudeDropMinM", v) && v >= -20 && v <= 0)
    { cauHinh.deltaH = (float)v; coThayDoi = true; }
  if (docSo(json, "postImpactWindowMs", v) && v >= 200 && v <= 10000 && floor(v) == v)
    { cauHinh.windowMs = (uint32_t)v; coThayDoi = true; }
  if (docSo(json, "postImpactStillnessDurationMs", v) && v >= 100 && v <= cauHinh.windowMs && floor(v) == v)
    { cauHinh.stillMs = (uint32_t)v; coThayDoi = true; }
  if (docSo(json, "stillnessTargetAccelerationMs2", v) && v >= 7 && v <= 12)
    { cauHinh.stillTarget = (float)v; coThayDoi = true; }
  if (docSo(json, "stillnessToleranceMs2", v) && v >= 0.1 && v <= 3)
    { cauHinh.stillTolerance = (float)v; coThayDoi = true; }
  if (docSo(json, "minimumStillnessSamples", v) && v >= 2 && v <= 100 && floor(v) == v)
    { cauHinh.minStillSamples = (uint32_t)v; coThayDoi = true; }
  if (docSo(json, "maximumSampleGapMs", v) && v >= 20 && v <= 2000 && floor(v) == v)
    { cauHinh.maxSampleGapMs = (uint32_t)v; coThayDoi = true; }
  if (docSo(json, "freeFallMinDurationMs", v) && v >= 10 && v <= 1000 && floor(v) == v)
    { cauHinh.lowDurationMs = (uint32_t)v; coThayDoi = true; }
  if (docSo(json, "pressureEvidenceMinRisePa", v) && v >= 0 && v <= 500)
    { cauHinh.pressureRisePa = (float)v; coThayDoi = true; }
  if (docSo(json, "pressureWindowMs", v) && v >= 100 && v <= 30000 && floor(v) == v)
    { cauHinh.pressureWindowMs = (uint32_t)v; coThayDoi = true; }
  if (docSo(json, "sampleWatchdogMs", v) && v >= 20 && v <= 2000 && floor(v) == v)
    { cauHinh.sampleWatchdogMs = (uint32_t)v; coThayDoi = true; }
}

static void resetPhatHien() {
  dangNghiNgo = daCoLowG = daBaoLowG = daBaoBatDong = false;
  lucBatDong = soMauBatDong = lucMauHopLe = lucApSuatGoc = 0;
}

void xuLyLenhDienThoai(const char *json) {
  Serial.printf("[BLE<-APP] %s\n", json);
  strcpy(ackCommandId, "profile");
  char commandType[48] = "";
  bool coCommand = docChuoi(json, "commandType", commandType, sizeof(commandType));

  if (coCommand) {
    docChuoi(json, "commandId", ackCommandId, sizeof(ackCommandId));
    if (strcmp(commandType, "START_STREAM") == 0) {
      streamEnabled = true; guiAck(true, "Stream active"); return;
    }
    if (strcmp(commandType, "STOP_STREAM") == 0) {
      streamEnabled = false; guiAck(true, "Stream stopped"); return;
    }
    if (strcmp(commandType, "SET_TEST_MODE") == 0 || strcmp(commandType, "THU_NGHIEM") == 0) {
      cauHinh = TEST; resetPhatHien(); guiAck(true, "Test mode"); return;
    }
    if (strcmp(commandType, "SET_DEFAULT_PROFILE") == 0) {
      cauHinh = NORMAL; resetPhatHien(); guiAck(true, "Default profile"); return;
    }
    if (strcmp(commandType, "GET_STATUS") == 0 || strcmp(commandType, "PING") == 0) {
      guiTrangThai(); guiAck(true, "PONG"); return;
    }
    if (strcmp(commandType, "CANCEL_ALERT") == 0) {
      resetPhatHien(); guiEvent("SOS_CANCELLED", 100); guiAck(true, "Cleared"); return;
    }
    if (strcmp(commandType, "SET_PROFILE") != 0) {
      guiAck(false, "Unknown command"); return;
    }
  }

  bool coThayDoi = false;
  apDungProfile(json, coThayDoi);
  if (!coThayDoi) { guiAck(false, "Khong co thong so hop le"); return; }
  Serial.printf("[CONFIG] Applied: %s\n", cauHinh.cheDoThuNghiem ? "TEST" : "NORMAL");
  resetPhatHien();
  guiAck(true, "Applied successfully");
}

// ============================================================================
// BLE SERVER CALLBACKS & INITIALIZATION (FIX CCCD 0x2902 DESCRIPTORS)
// ============================================================================
class KetNoiCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    ketNoi = true;
    streamEnabled = true;
    Serial.printf("[BLE] Connected | Client MTU=%u\n", pServer->getPeerMTU(pServer->getConnId()));
  }
  void onDisconnect(BLEServer *) override {
    ketNoi = false;
    quangBaCho = true;
    Serial.println("[BLE] Disconnected -> Re-advertising scheduled");
  }
};

class LenhCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) override {
    String input = c->getValue();
    if (input.length() == 0 || input.length() >= DO_DAI_LENH) return;

    portENTER_CRITICAL(&khoaHangDoi);
    uint8_t ghiMoi = (dauGhi + 1) % SO_SLOT_LENH;
    if (ghiMoi != dauDoc) {
      memcpy(lenhCho[dauGhi], input.c_str(), input.length());
      lenhCho[dauGhi][input.length()] = 0;
      dauGhi = ghiMoi;
    } else {
      Serial.println("[BLE] Canh bao: Hang doi lenh day");
    }
    portEXIT_CRITICAL(&khoaHangDoi);
  }
};

void khoiTaoBLE() {
  uint64_t mac = ESP.getEfuseMac();
  snprintf(tenBle, sizeof(tenBle), "FALLSAFE-%04X", (unsigned)(mac & 0xFFFF));

  BLEDevice::init(tenBle);
  BLEDevice::setMTU(517); // De xuat MTU cuc dai toi Client
  mayChu = BLEDevice::createServer();
  mayChu->setCallbacks(new KetNoiCallbacks());

  BLEService *service = mayChu->createService(UUID_SERVICE);

  // Tao Characteristic kem Descriptors 0x2902 (BAT BUOC CHO NOTIFY HOAT DONG)
  streamChar = service->createCharacteristic(UUID_STREAM, BLECharacteristic::PROPERTY_NOTIFY);
  streamChar->addDescriptor(new BLE2902());

  eventChar  = service->createCharacteristic(UUID_EVENT,  BLECharacteristic::PROPERTY_NOTIFY);
  eventChar->addDescriptor(new BLE2902());

  ackChar    = service->createCharacteristic(UUID_ACK,    BLECharacteristic::PROPERTY_NOTIFY);
  ackChar->addDescriptor(new BLE2902());

  // Ho tro ca WRITE co phan hoi va WRITE_NR (Write Without Response)
  BLECharacteristic *cmdChar = service->createCharacteristic(
    UUID_COMMAND,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  cmdChar->setCallbacks(new LenhCallbacks());

  service->start();

  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(UUID_SERVICE);
  adv->setScanResponse(true);
  adv->setMinPreferred(0x06); // Ho tro ket noi on dinh tren iOS
  adv->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.printf("[BLE] READY | Name: %s | MTU: 517\n", tenBle);
}

// ============================================================================
// THUAT TOAN PHAT HIEN NGA (Multi-stage Fall Detection)
// ============================================================================
void kiemTraSuKien() {
  if (!mau.hopLe) {
    dangNghiNgo = daCoLowG = daBaoLowG = daBaoBatDong = false;
    lucMauHopLe = lucBatDong = soMauBatDong = 0;
    return;
  }

  uint32_t now = mau.timestampMs;
  if (lucMauHopLe && now - lucMauHopLe > cauHinh.sampleWatchdogMs) {
    dangNghiNgo = daCoLowG = daBaoLowG = daBaoBatDong = false;
    lucBatDong = soMauBatDong = 0;
  }
  if (lucMauHopLe && now - lucMauHopLe > cauHinh.maxSampleGapMs) {
    lucBatDong = soMauBatDong = 0;
  }
  lucMauHopLe = now;

  float g = mau.doLonGiaToc / G;

  // Giai doan 1: Roi tu do (Free-fall / Low-G)
  if (daCoLowG && now - lucLowG > cauHinh.windowMs) {
    daCoLowG = daBaoLowG = false;
  }
  if (g < cauHinh.lowG) {
    if (!daCoLowG) { daCoLowG = true; lucLowG = now; }
    if (!daBaoLowG && now - lucLowG >= cauHinh.lowDurationMs) {
      daBaoLowG = true;
      Serial.println("[EVENT] LOW_G Detected");
    }
  } else if (!daBaoLowG) {
    daCoLowG = false;
  }

  // Giai doan 2: Va cham manh (Impact)
  if (mau.doLonGiaToc >= cauHinh.impact) {
    if (!dangNghiNgo) {
      int doTinCay = 40 + (daBaoLowG ? 20 : 0) +
        (mau.vanTocGoc >= cauHinh.gyro ? 20 : 0) +
        (coKhiAp && coMoc && doCaoTuongDoi <= cauHinh.deltaH ? 10 : 0);
      Serial.printf("[EVENT] IMPACT DETECTED | LowG=%s, Gyro=%.1f, dH=%.2fm\n",
                    daBaoLowG ? "YES" : "NO", mau.vanTocGoc, doCaoTuongDoi);
      guiEvent("IMPACT_DETECTED", doTinCay);
    }
    dangNghiNgo = true;
    lucImpact = now;
    lucBatDong = 0;
    daBaoBatDong = false;
    soMauBatDong = 0;
    if (coKhiAp && coMoc) {
      apSuatGoc = apSuat;
      lucApSuatGoc = now;
    } else {
      lucApSuatGoc = 0;
    }
  }

  if (!dangNghiNgo) return;

  // Het cua so theo doi ma khong bat dong -> Bo qua
  if (now - lucImpact > cauHinh.windowMs) {
    dangNghiNgo = daCoLowG = daBaoLowG = false;
    return;
  }

  // Giai doan 3: Bat dong sau va cham (Post-impact Stillness)
  if (fabsf(mau.doLonGiaToc - cauHinh.stillTarget) <= cauHinh.stillTolerance &&
      mau.vanTocGoc < fminf(15.0f, cauHinh.gyro * 0.2f)) {
    if (!soMauBatDong) lucBatDong = now;
    ++soMauBatDong;
    if (!daBaoBatDong && soMauBatDong >= cauHinh.minStillSamples &&
        now - lucBatDong >= cauHinh.stillMs) {
      daBaoBatDong = true;
      Serial.println("[EVENT] INACTIVITY_DETECTED (Fall Confirmed)");
      bool apSuatHoTro = lucApSuatGoc && coKhiAp && coMoc &&
        (now - lucApSuatGoc <= cauHinh.pressureWindowMs) &&
        (apSuat - apSuatGoc >= cauHinh.pressureRisePa);
      guiEvent("INACTIVITY_DETECTED", 65 + (apSuatHoTro ? 15 : 0));
    }
  } else {
    lucBatDong = 0;
    soMauBatDong = 0;
  }
}

void capNhatLED(uint32_t now) {
  if (dangNghiNgo) {
    digitalWrite(CHAN_LED_BOARD, (now % 200 < 100) ? HIGH : LOW); // Nhap nhay nhanh khi co va cham
  } else if (!mpuOk || !gy63Ok) {
    digitalWrite(CHAN_LED_BOARD, (now % 1000 < 500) ? HIGH : LOW); // Nhap nhay deu khi thieu cam bien
  } else {
    digitalWrite(CHAN_LED_BOARD, (now % 2000 < 80) ? HIGH : LOW); // Nhip tim 2s khi he thong hoan hao
  }
}

void inTrangThaiSerial(uint32_t now) {
  if (now - lucIn < 1000) return;
  lucIn = now;

  if (mau.hopLe) {
    Serial.printf("[MPU] a=%.2f g | w=%.1f dps | ", mau.doLonGiaToc / G, mau.vanTocGoc);
  } else {
    Serial.print("[MPU] ERROR | ");
  }

  if (coKhiAp && coMoc) {
    Serial.printf("[GY63] P=%.2f hPa | dH=%.2f m | ", apSuat / 100.0f, doCaoTuongDoi);
  } else {
    Serial.printf("[GY63] %s | ", gy63Ok ? "CALIBRATING" : "DISCONNECTED");
  }

  Serial.printf("[BLE] %s | Pin: %d%%\n", ketNoi ? "CONNECTED" : "ADV", phanTramPin);
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=======================================================");
  Serial.println("[BOOT] NCKH27PA V5 - ESP32-S3 Super Mini Fall Detection");
  Serial.println("[BOOT] Pure BLE (No WiFi) | I2C0 (8/9) | I2C1 (7/6)");
  Serial.println("=======================================================");

  pinMode(CHAN_LED_BOARD, OUTPUT);
  digitalWrite(CHAN_LED_BOARD, LOW);

  // 1. Khoi tao BLE Stack va GATT Services truoc
  khoiTaoBLE();

  // 2. Khoi phuc va khoi tao Bus I2C0 cho MPU-6050 va IP5306
  phucHoiBusI2C(CHAN_I2C0_SDA, CHAN_I2C0_SCL);
  Wire.begin(CHAN_I2C0_SDA, CHAN_I2C0_SCL, 100000);
  Wire.setTimeOut(25); // Timeout ngan phong tranh treo bus

  // 3. Khoi phuc va khoi tao Bus I2C1 doc lap cho GY-63 (MS5611)
  phucHoiBusI2C(CHAN_I2C1_SDA, CHAN_I2C1_SCL);
  Wire1.begin(CHAN_I2C1_SDA, CHAN_I2C1_SCL, 100000);
  Wire1.setTimeOut(25);

  // 4. Kiem tra va khoi tao phan cung
  coIp5306 = docIP5306();
  mpuOk    = khoiTaoMPU();
  gy63Ok   = khoiTaoGY63();

  Serial.printf("[INIT] IP5306: %s | Pin: %d%%\n", coIp5306 ? "FOUND" : "NOT FOUND (Default 100%)", phanTramPin);
  Serial.printf("[INIT] MPU6050: %s (Addr: 0x%02X)\n", mpuOk ? "ONLINE" : "OFFLINE", s_diaChiMpu);
  Serial.printf("[INIT] MS5611:  %s\n", gy63Ok ? "ONLINE" : "OFFLINE");

  mocLayMauUs = micros();
  lucBaro     = millis();
  lucThuLai   = millis();
}

void xuLyHangDoiLenh() {
  while (dauDoc != dauGhi) {
    char buff[DO_DAI_LENH];
    portENTER_CRITICAL(&khoaHangDoi);
    memcpy(buff, lenhCho[dauDoc], DO_DAI_LENH);
    dauDoc = (dauDoc + 1) % SO_SLOT_LENH;
    portEXIT_CRITICAL(&khoaHangDoi);
    xuLyLenhDienThoai(buff);
  }
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  uint32_t now = millis();

  // Quang ba lai khi bi mat ket noi BLE
  if (quangBaCho) {
    quangBaCho = false;
    delay(100);
    BLEDevice::startAdvertising();
    Serial.println("[BLE] Resumed Advertising");
  }

  // Xu ly cac lenh nhan duoc tu Dien thoai qua BLE
  xuLyHangDoiLenh();

  // Co che tu dong thu ket noi lai cam bien moi 5 giay (Khong scan cheo bus)
  if (now - lucThuLai >= 5000) {
    lucThuLai = now;
    if (!mpuOk) {
      Serial.print("[RETRY] MPU6050: ");
      mpuOk = khoiTaoMPU();
    }
    if (!gy63Ok) {
      Serial.print("[RETRY] GY-63 MS5611: ");
      gy63Ok = khoiTaoGY63();
    }
    if (!coIp5306) {
      docIP5306();
    }
  }

  // Doc cam bien ap suat khi quyen MS5611 (~25 Hz)
  now = millis();
  docGY63(now);

  // Lay mau IMU MPU-6050 chinh xac 100 Hz bang micros
  uint32_t us = micros();
  if ((int32_t)(us - mocLayMauUs) >= 0) {
    mocLayMauUs += 10000;
    if ((int32_t)(us - mocLayMauUs) >= 0) mocLayMauUs = us + 10000;

    if (docMPUVaoMau()) {
      loiMpu = 0;
    } else if (mpuOk && ++loiMpu >= 5) {
      Serial.println("[MPU] Error timeout -> offline");
      mpuOk = false;
    }
    kiemTraSuKien();
  }

  // Gui Telemetry dinh ky qua BLE (~25 Hz)
  if (now - lucBle >= 40) {
    lucBle = now;
    guiTelemetry();
  }

  // Cap nhat pin dinh ky moi 5 giay
  if (now - lucPin >= 5000) {
    lucPin = now;
    if (coIp5306) docIP5306();
  }

  // Den LED bao trang thai & In log chan doan Serial
  capNhatLED(now);
  inTrangThaiSerial(now);
}