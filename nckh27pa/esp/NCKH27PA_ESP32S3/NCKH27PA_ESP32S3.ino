// NCKH27PA - ESP32-S3 Super Mini; Arduino IDE: ESP32S3 Dev Module.
// Day da chot: MPU SDA=8 SCL=9 (Wire), GY63 SDA=7 SCL=6 (Wire1).
// ESP chi gui du lieu va goi y; Android moi quyet dinh countdown/SOS.
#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h> // Electronic Cats 1.4.5: cung thu vien voi ban mau da doc duoc.
#include <MS5611.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <math.h>
#include <ctype.h>

// Kieu du lieu khai bao som de Arduino tu tao prototype dung.
struct CauHinh {
  float lowG, impact, gyro, deltaH;
  uint32_t windowMs, stillMs;
  float stillTarget, stillTolerance, pressureRisePa;
  uint32_t minStillSamples, maxSampleGapMs, lowDurationMs;
  uint32_t pressureWindowMs, sampleWatchdogMs;
  bool cheDoThuNghiem;
};
struct Mau {
  float ax, ay, az, gx, gy, gz, doLonGiaToc, vanTocGoc;
  uint32_t timestampMs;
  bool hopLe;
};

static const char *UUID_SERVICE = "7d2a0001-6f45-4c2b-9a1e-38a8f5c10001";
static const char *UUID_STREAM = "7d2a0002-6f45-4c2b-9a1e-38a8f5c10001";
static const char *UUID_EVENT = "7d2a0003-6f45-4c2b-9a1e-38a8f5c10001";
static const char *UUID_COMMAND = "7d2a0005-6f45-4c2b-9a1e-38a8f5c10001";
static const char *UUID_ACK = "7d2a0006-6f45-4c2b-9a1e-38a8f5c10001";
static const float G = 9.80665f;
static const CauHinh NORMAL = {0.50f, 25.0f, 120.0f, -0.40f, 3000, 1000,
  9.81f, 1.0f, 12.0f, 6, 250, 80, 5000, 100, false};
static const CauHinh TEST = {0.75f, 15.0f, 60.0f, -0.15f, 4000, 500,
  9.81f, 1.0f, 6.0f, 4, 250, 30, 4000, 100, true};
static CauHinh cauHinh = NORMAL;
static MPU6050 mpuA(0x68, &Wire), mpuB(0x69, &Wire);
static MS5611 gy63a(0x77, &Wire1), gy63b(0x76, &Wire1);
static MS5611 *gy63 = &gy63a;
static uint8_t diaChiMpu = 0, diaChiGy63 = 0;
static bool mpuOk = false, gy63Ok = false, coMoc = false, coKhiAp = false, ketNoi = false;
static uint8_t loiMpu = 0, loiGy63 = 0;
static float apSuat = 0, nhietDo = 0, apSuatMoc = 0, doCaoTuongDoi = 0;
static float tongMoc = 0;
static uint8_t soMauMoc = 0;
static uint32_t lucBaro = 0, lucThuLai = 0, lucIn = 0, lucBle = 0;
static uint32_t lucLowG = 0, lucImpact = 0, lucBatDong = 0;
static uint32_t lucMauHopLe = 0, lucApSuatGoc = 0;
static uint32_t soMauBatDong = 0;
static float apSuatGoc = 0;
static uint32_t soThuTu = 0, soKhung = 0, mocLayMauUs = 0;
static bool dangNghiNgo = false, daCoLowG = false, daBaoLowG = false, daBaoBatDong = false;
static Mau mau = {};
static char tenBle[24];
static char lenhCho[768] = {};
static bool coLenhCho = false;
static bool lenhQuaDai = false;
static portMUX_TYPE khoaLenh = portMUX_INITIALIZER_UNLOCKED;
static BLECharacteristic *streamChar = nullptr, *eventChar = nullptr, *ackChar = nullptr;
static BLEServer *mayChu = nullptr;
static bool streamEnabled = true;
static volatile bool quangBaCho = false;
static char ackCommandId[65] = "profile";

// Khoi tao truc tiep nhu ban mau; khong tu phat xung GPIO truoc Wire.begin().
// Chi khoi dong lai bus khi cam bien loi, giu nguyen hai cap day thuc te.
bool khoiDongBus(TwoWire &bus, uint8_t sda, uint8_t scl, uint32_t hz) {
  bus.end();
  bool ready = bus.begin(sda, scl, hz);
  bus.setTimeOut(25); // Giong ban mau; loi bus van co gioi han cho.
  return ready;
}
static uint8_t scanAddress = 0, scanFound = 0;
void quetGY63TungBuoc(uint32_t now) {
  static uint32_t last = 0;
  if (!scanAddress || now - last < 50) return;
  last = now;
  if (gy63Ok) { scanAddress = 0; return; }
  Wire1.beginTransmission(scanAddress);
  if (Wire1.endTransmission() == 0) {
    ++scanFound; Serial.printf("[GY63][I2C] ACK 0x%02X\n", scanAddress);
  }
  if (++scanAddress > 0x77) {
    scanAddress = 0;
    if (!scanFound) Serial.println("[GY63][I2C] KHONG CO dia chi nao ACK (kiem tra 3V3, GND, SDA/SCL, dien tro keo len)");
  }
}
// Hai bus tach biet; khong dung GPIO 8/9 cho LED vi trung day I2C.
bool khoiTaoMPU() {
  if (!khoiDongBus(Wire, 8, 9, 100000)) return mpuOk = false;
  for (uint8_t addr : {uint8_t(0x68), uint8_t(0x69)}) {
    Wire.beginTransmission(addr);
    uint8_t ack = Wire.endTransmission();
    Serial.printf("[MPU][I2C] SDA=8 SCL=9 addr=0x%02X status=%u\n", addr, ack);
    if (ack != 0) continue;
    MPU6050 &mpu = addr == 0x68 ? mpuA : mpuB;
    mpu.initialize(ACCEL_FS::A16G, GYRO_FS::G2000DPS);
    if (mpu.testConnection()) {
      diaChiMpu = addr;
      mpu.setClockSource(MPU6050_CLOCK_PLL_XGYRO);
      mpu.setSleepEnabled(false);
      mpu.setRate(9); // 100 Hz voi DLPF bat, cung cau hinh ban mau.
      mpu.setDLPFMode(MPU6050_DLPF_BW_188);
      // Read both adjacent config registers with checked transport too: the
      // library range getters also do not expose a failed I2C transaction.
      Wire.beginTransmission(addr); Wire.write(MPU6050_RA_GYRO_CONFIG);
      if (Wire.endTransmission(false) != 0 || Wire.requestFrom(addr, uint8_t(2)) != 2) continue;
      uint8_t gyroRange = Wire.read(), accelRange = Wire.read();
      if ((gyroRange & 0x18) != 0x18 || (accelRange & 0x18) != 0x18) continue;
      loiMpu = 0;
      return mpuOk = true;
    }
  }
  diaChiMpu = 0;
  return mpuOk = false;
}
bool khoiTaoGY63() {
  if (!khoiDongBus(Wire1, 7, 6, 100000)) return gy63Ok = false;
  bool coAck = false;
  for (MS5611 *sensor : {&gy63a, &gy63b}) {
    // ACK cho biet day/nguon/bus co tra loi; begin/reset con kiem tra PROM.
    if (!sensor->isConnected()) continue;
    coAck = true;
    // Thu vien tu doc PROM/bu nhiet; mathMode=1 sua loi he so 2 tren module GY63.
    if (sensor->begin() && sensor->reset(1)) {
      sensor->setOversampling(OSR_LOW); // Giong ban mau, giam thoi gian cho chuyen doi.
      gy63 = sensor;
      diaChiGy63 = sensor == &gy63a ? 0x77 : 0x76;
      loiGy63 = 0;
      coKhiAp = false;
      coMoc = false;
      soMauMoc = 0;
      tongMoc = 0;
      return gy63Ok = true;
    }
  }
  diaChiGy63 = 0;
  coKhiAp = coMoc = false;
  // Chi quet het bus luc dau va moi phut: tim dia chi la, khong can tro IMU/BLE.
  static uint32_t lucChanDoan = 0;
  uint32_t now = millis();
  if (!lucChanDoan || now - lucChanDoan >= 60000) {
    lucChanDoan = now ? now : 1;
    if (coAck) {
      Serial.println("[GY63][I2C] 0x76/0x77 ACK nhung ROM/PROM loi; kiem tra nguon va chip MS5611");
    } else {
      Serial.println("[GY63][I2C] 0x76/0x77 khong ACK | Wire1 SDA=7 SCL=6 @100kHz");
      scanAddress = 0x08; scanFound = 0; // One bounded probe per loop slot, not 112 in a row.
    }
  }
  return gy63Ok = false;
}

// Khong gui mau cu: moi chu ky tao mau moi, loi duoc bieu dien bang null/quality=0.
void docCamBien() {
  mau.hopLe = false;
  mau.timestampMs = millis();
  if (!mpuOk) return;
  // getMotion6() khong tra ve loi bus. Dung I2Cdev cua cung thu vien,
  // chi chap nhan du 14 byte de mat ket noi khong thanh du lieu te nga gia.
  uint8_t bytes[14];
  bool ok = I2Cdev::readBytes(diaChiMpu, MPU6050_RA_ACCEL_XOUT_H,
                            14, bytes, 25, &Wire) == 14;
  if (!ok) {
    while (Wire.available()) Wire.read();
    if (++loiMpu >= 3) { mpuOk = false; Serial.println("[MPU] ERROR - cho ket noi lai"); }
    return;
  }

  loiMpu = 0;
  // Signed big-endian data, ranges configured and verified during init.
  mau.ax = int16_t((uint16_t(bytes[0]) << 8) | bytes[1]) * (G / 2048.0f);
  mau.ay = int16_t((uint16_t(bytes[2]) << 8) | bytes[3]) * (G / 2048.0f);
  mau.az = int16_t((uint16_t(bytes[4]) << 8) | bytes[5]) * (G / 2048.0f);
  mau.gx = int16_t((uint16_t(bytes[8]) << 8) | bytes[9]) / 16.4f;
  mau.gy = int16_t((uint16_t(bytes[10]) << 8) | bytes[11]) / 16.4f;
  mau.gz = int16_t((uint16_t(bytes[12]) << 8) | bytes[13]) / 16.4f;
  mau.doLonGiaToc = sqrtf(mau.ax*mau.ax + mau.ay*mau.ay + mau.az*mau.az);
  mau.vanTocGoc = sqrtf(mau.gx*mau.gx + mau.gy*mau.gy + mau.gz*mau.gz);
  mau.hopLe = isfinite(mau.doLonGiaToc) && isfinite(mau.vanTocGoc);
}
void docGY63(uint32_t now) {
  if (!gy63Ok || now - lucBaro < 40) return; // ~25 Hz; OSR_LOW giam thoi gian chan.
  lucBaro = now;
  if (gy63->read() != MS5611_READ_OK) {
    coKhiAp = false;
    if (++loiGy63 >= 3) { gy63Ok = false; coMoc = false; Serial.println("[GY63] ERROR - cho ket noi lai"); }
    return;
  }
  float p = gy63->getPressurePascal(), temp = gy63->getTemperature();
  if (!isfinite(p) || p < 30000 || p > 110000 || !isfinite(temp)) {
    coKhiAp = false;
    if (++loiGy63 >= 3) { gy63Ok = false; coMoc = false; Serial.println("[GY63] ERROR - ap suat khong hop le"); }
    return;
  }
  loiGy63 = 0; coKhiAp = true; apSuat = p; nhietDo = temp;
  if (!coMoc) {
    tongMoc += p;
    if (++soMauMoc >= 30) {
      apSuatMoc = tongMoc / soMauMoc;
      coMoc = true;
      Serial.printf("[GY63] Moc ap suat: %.1f Pa (%u mau)\n", apSuatMoc, soMauMoc);
    }
  }
  if (coMoc) doCaoTuongDoi = 44330.77f * (1 - powf(p / apSuatMoc, 0.190263f));
}

// IF-003: Android nhan khung 16 byte, chia theo MTU-19, sau do parse JSON.
void guiKhung(BLECharacteristic *characteristic, uint8_t kind, const char *json) {
  if (!ketNoi || !characteristic) return;
  size_t length = strlen(json);
  if (!length || length > 1024) return;
  uint16_t mtu = mayChu->getPeerMTU(mayChu->getConnId());
  if (mtu < 23) mtu = 23;
  if (mtu > 517) mtu = 517;
  // Khong phat hang tram fragment khi Android chua thuong luong MTU.
  // Ung dung hien tai yeu cau MTU lon truoc khi dang ky notification.
  if (mtu < 128) {
    static uint32_t lucBaoMtu = 0;
    if (millis() - lucBaoMtu >= 3000) {
      lucBaoMtu = millis();
      Serial.printf("[BLE] Cho MTU >=128, hien tai=%u; chua gui packet\n", mtu);
    }
    return;
  }
  size_t chunk = mtu - 19;
  uint16_t count = (length + chunk - 1) / chunk;
  uint32_t id = ++soKhung;
  if (!id) id = ++soKhung;
  for (uint16_t i = 0; i < count; ++i) {
    size_t offset = i * chunk, n = min(chunk, length - offset);
    uint8_t frame[514] = {0x46, 0x53, 1, kind};
    for (int b = 0; b < 4; ++b) frame[4+b] = id >> (8*b);
    frame[8] = i; frame[9] = i >> 8;
    frame[10] = count; frame[11] = count >> 8;
    frame[12] = length; frame[13] = length >> 8;
    frame[14] = offset; frame[15] = offset >> 8;
    memcpy(frame + 16, json + offset, n);
    characteristic->setValue(frame, 16+n);
    characteristic->notify();
  }
}
void guiAck(bool ok, const char *lyDo) {
  char json[320];
  snprintf(json, sizeof(json), "{\"protocolVersion\":1,\"commandId\":\"%s\",\"deviceId\":\"%s\",\"timestampMs\":%lu,\"commandStatus\":\"%s\",\"errorCode\":null,\"message\":\"%s\"}",
           ackCommandId, tenBle, (unsigned long)millis(), ok ? "COMPLETED" : "REJECTED", lyDo);
  guiKhung(ackChar, 5, json);
}
void guiEvent(const char *type, int confidence) {
  char json[360];
  snprintf(json, sizeof(json), "{\"protocolVersion\":1,\"eventId\":\"evt-%lu\",\"deviceId\":\"%s\",\"sequenceNumber\":%lu,\"timestampMs\":%lu,\"eventType\":\"%s\",\"eventSeverity\":\"WARNING\",\"sosButtonPressed\":false,\"eventConfidence\":%d,\"peakAccelerationMs2\":null,\"orientationChangeDeg\":null,\"altitudeDeltaM\":null,\"inactivityDurationMs\":null,\"checksum\":null}",
           (unsigned long)millis(), tenBle, (unsigned long)++soThuTu, (unsigned long)millis(), type, confidence);
  guiKhung(eventChar, 2, json);
}
// Profile ghi tren 0005: cac truong bat buoc phai duoc kiem tra truoc khi ap dung.
// Parser gioi han cho JSON so/phim bool phang, khong nhan prefix trung ten hay NaN.
bool docSo(const char *json, const char *key, double &value) {
  char ten[64]; snprintf(ten, sizeof(ten), "\"%s\"", key);
  const char *p = strstr(json, ten);
  if (!p || strstr(p + strlen(ten), ten)) return false;
  p += strlen(ten); while (isspace((unsigned char)*p)) ++p;
  if (*p++ != ':') return false;
  while (isspace((unsigned char)*p)) ++p;
  char *end = nullptr;
  value = strtod(p, &end);
  return end != p && isfinite(value) && (*end == ',' || *end == '}' || isspace((unsigned char)*end));
}
bool docBool(const char *json, const char *key, bool &value) {
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
// Profile Android la object JSON phang gom 14 so (va tuy chon TEST bool).
// Kiem tra ca dau/cuoi, key la, trung key: tranh ACK cho chuoi JSON hong.
bool profileJsonHopLe(const char *json, bool command) {
  static const char *keys[] = {
    "impactAccelerationMs2", "stillnessTargetAccelerationMs2", "stillnessToleranceMs2",
    "postImpactWindowMs", "postImpactStillnessDurationMs", "minimumStillnessSamples",
    "maximumSampleGapMs", "freeFallThresholdMs2", "freeFallMinDurationMs",
    "gyroTurnThresholdDps", "pressureEvidenceMinRisePa", "pressureWindowMs",
    "altitudeDropMinM", "sampleWatchdogMs", "cheDoThuNghiem",
    "protocolVersion", "commandId", "timestampMs", "commandType"
  };
  bool seen[19] = {};
  const char *p = json;
  while (isspace((unsigned char)*p)) ++p;
  if (*p++ != '{') return false;
  int count = 0;
  while (true) {
    while (isspace((unsigned char)*p)) ++p;
    if (*p++ != '"') return false;
    const char *start = p;
    while (*p && *p != '"') {
      if (*p == '\\' || (unsigned char)*p < 32) return false;
      ++p;
    }
    if (*p != '"') return false;
    size_t len = p - start;
    int key = -1;
    for (int i = 0; i < 19; ++i)
      if (strlen(keys[i]) == len && strncmp(start, keys[i], len) == 0) key = i;
    if (key < 0 || seen[key] || (command ? key < 15 : key >= 15)) return false;
    seen[key] = true; ++count; ++p;
    while (isspace((unsigned char)*p)) ++p;
    if (*p++ != ':') return false;
    while (isspace((unsigned char)*p)) ++p;
    if (key == 14) {
      if (strncmp(p, "true", 4) == 0) p += 4;
      else if (strncmp(p, "false", 5) == 0) p += 5;
      else return false;
    } else if (key == 16 || key == 18) {
      if (*p++ != '"') return false;
      const char *text = p;
      while (isalnum((unsigned char)*p) || *p == '-' || *p == '_') ++p;
      if (*p != '"' || p == text || p - text > 64) return false;
      ++p;
    } else {
      // Strict JSON numbers; strtod alone also accepts hex, +1 and .5.
      const char *number = p;
      if (*p == '-') ++p;
      if (*p == '0') ++p;
      else { if (*p < '1' || *p > '9') return false; while (isdigit((unsigned char)*p)) ++p; }
      if (*p == '.') { ++p; if (!isdigit((unsigned char)*p)) return false; while (isdigit((unsigned char)*p)) ++p; }
      if (*p == 'e' || *p == 'E') {
        ++p; if (*p == '+' || *p == '-') ++p;
        if (!isdigit((unsigned char)*p)) return false;
        while (isdigit((unsigned char)*p)) ++p;
      }
      char *end;
      double value = strtod(number, &end);
      if (end != p || !isfinite(value)) return false;
    }
    while (isspace((unsigned char)*p)) ++p;
    if (*p == '}') { ++p; break; }
    if (*p++ != ',') return false;
  }
  while (isspace((unsigned char)*p)) ++p;
  if (*p != 0) return false;
  if (command) return count == 4;
  if (count == 1 && seen[14]) return true;
  for (int i = 0; i < 14; ++i) if (!seen[i]) return false;
  return count == 14 || count == 15;
}
bool docChuoi(const char *json, const char *key, char *out, size_t capacity) {
  char name[40]; snprintf(name, sizeof(name), "\"%s\"", key);
  const char *p = strstr(json, name);
  if (!p) return false;
  p += strlen(name); while (isspace((unsigned char)*p)) ++p;
  if (*p++ != ':') return false;
  while (isspace((unsigned char)*p)) ++p;
  if (*p++ != '"') return false;
  const char *end = strchr(p, '"');
  if (!end || size_t(end - p) >= capacity) return false;
  memcpy(out, p, end-p); out[end-p] = 0; return true;
}
void nhanCauHinhAndroid(const char *json) {
  Serial.println("[CONFIG] Received from Android");
  strcpy(ackCommandId, "profile");
  if (strstr(json, "\"commandType\"")) {
    double version, timestamp; char type[65];
    if (!profileJsonHopLe(json, true) || !docSo(json, "protocolVersion", version) || version != 1 ||
        !docSo(json, "timestampMs", timestamp) || timestamp < 0 || floor(timestamp) != timestamp ||
        !docChuoi(json, "commandId", ackCommandId, sizeof(ackCommandId)) ||
        !docChuoi(json, "commandType", type, sizeof(type))) { guiAck(false, "Invalid command"); return; }
    if (!strcmp(type, "START_STREAM")) streamEnabled = true;
    else if (!strcmp(type, "STOP_STREAM")) streamEnabled = false;
    else { guiAck(false, "Unknown command"); return; }
    guiAck(true, streamEnabled ? "Stream active" : "Stream stopped"); return;
  }
  if (!profileJsonHopLe(json, false)) { guiAck(false, "Malformed profile JSON"); return; }
  // Ho tro profile hien tai cua BleProfilePayload, hoac lenh chi doi TEST mode.
  bool testMode = cauHinh.cheDoThuNghiem;
  bool doiMode = strstr(json, "\"cheDoThuNghiem\"") != nullptr;
  if (doiMode && !docBool(json, "cheDoThuNghiem", testMode)) { guiAck(false, "Invalid test mode"); return; }
  bool profile = strstr(json, "\"impactAccelerationMs2\"") != nullptr;
  if (!profile && !doiMode) { guiAck(false, "Unknown config"); return; }

  CauHinh moi = testMode ? TEST : NORMAL;
  if (profile) {
    double impact, lowG, gyro, deltaH, window, still;
    double stillTarget, stillTolerance, minSamples, maxGap, lowDuration;
    double pressureRise, pressureWindow, watchdog;
    if (!docSo(json, "impactAccelerationMs2", impact) ||
        !docSo(json, "freeFallThresholdMs2", lowG) ||
        !docSo(json, "gyroTurnThresholdDps", gyro) ||
        !docSo(json, "altitudeDropMinM", deltaH) ||
        !docSo(json, "postImpactWindowMs", window) ||
        !docSo(json, "postImpactStillnessDurationMs", still) ||
        !docSo(json, "stillnessTargetAccelerationMs2", stillTarget) ||
        !docSo(json, "stillnessToleranceMs2", stillTolerance) ||
        !docSo(json, "minimumStillnessSamples", minSamples) ||
        !docSo(json, "maximumSampleGapMs", maxGap) ||
        !docSo(json, "freeFallMinDurationMs", lowDuration) ||
        !docSo(json, "pressureEvidenceMinRisePa", pressureRise) ||
        !docSo(json, "pressureWindowMs", pressureWindow) ||
        !docSo(json, "sampleWatchdogMs", watchdog) ||
        impact < 10 || impact > 120 || lowG < 0.1 || lowG > 9 ||
        gyro < 10 || gyro > 1800 || deltaH < -20 || deltaH > 0 ||
        window < 200 || window > 10000 || still < 100 || still > window ||
        floor(window) != window || floor(still) != still) {
      guiAck(false, "Invalid profile"); return;
    }
    if (stillTarget < 7 || stillTarget > 12 || stillTolerance < 0.1 || stillTolerance > 3 ||
        minSamples < 2 || minSamples > 100 || floor(minSamples) != minSamples ||
        maxGap < 20 || maxGap > 2000 || floor(maxGap) != maxGap ||
        lowDuration < 10 || lowDuration > 1000 || floor(lowDuration) != lowDuration ||
        pressureRise < 0 || pressureRise > 500 ||
        pressureWindow < 100 || pressureWindow > 30000 || floor(pressureWindow) != pressureWindow ||
        watchdog < 20 || watchdog > 2000 || floor(watchdog) != watchdog ||
        still > window) {
      guiAck(false, "Invalid profile bounds"); return;
    }
    moi.impact = impact; moi.lowG = lowG / G; moi.gyro = gyro;
    moi.deltaH = deltaH; moi.windowMs = window; moi.stillMs = still;
    moi.stillTarget = stillTarget; moi.stillTolerance = stillTolerance;
    moi.minStillSamples = minSamples; moi.maxSampleGapMs = maxGap;
    moi.lowDurationMs = lowDuration; moi.pressureRisePa = pressureRise;
    moi.pressureWindowMs = pressureWindow; moi.sampleWatchdogMs = watchdog;
  }
  moi.cheDoThuNghiem = testMode;
  cauHinh = moi;
  dangNghiNgo = daCoLowG = daBaoLowG = daBaoBatDong = false;
  lucBatDong = soMauBatDong = lucMauHopLe = 0;
  Serial.printf("[CONFIG] Applied successfully | %s\n", testMode ? "TEST" : "NORMAL");
  guiAck(true, "Applied successfully");
}
class KetNoiCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *) override {
    ketNoi = true; streamEnabled = true;
    Serial.println("[BLE] connected | telemetry enabled");
  }
  void onDisconnect(BLEServer *) override {
    ketNoi = false; quangBaCho = true;
    Serial.println("[BLE] disconnected | cho quang ba lai trong loop");
  }
};
class LenhCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) override {
    String input = c->getValue();
    portENTER_CRITICAL(&khoaLenh);
    if (!coLenhCho && !lenhQuaDai) {
      if (input.length() && input.length() < sizeof(lenhCho) &&
          !memchr(input.c_str(), 0, input.length())) {
        memcpy(lenhCho, input.c_str(), input.length()); lenhCho[input.length()] = 0;
        coLenhCho = true;
      } else lenhQuaDai = true;
    }
    portEXIT_CRITICAL(&khoaLenh);
  }
};
void khoiTaoBLE() {
  uint64_t mac = ESP.getEfuseMac();
  snprintf(tenBle, sizeof(tenBle), "FALLSAFE-%04X", (unsigned)(mac & 0xffff));
  BLEDevice::init(tenBle); BLEDevice::setMTU(517);
  mayChu = BLEDevice::createServer(); mayChu->setCallbacks(new KetNoiCallbacks());
  BLEService *service = mayChu->createService(UUID_SERVICE);
  streamChar = service->createCharacteristic(UUID_STREAM, BLECharacteristic::PROPERTY_NOTIFY);
  eventChar = service->createCharacteristic(UUID_EVENT, BLECharacteristic::PROPERTY_NOTIFY);
  ackChar = service->createCharacteristic(UUID_ACK, BLECharacteristic::PROPERTY_NOTIFY);
  service->createCharacteristic(UUID_COMMAND, BLECharacteristic::PROPERTY_WRITE)->setCallbacks(new LenhCallbacks());
  // NimBLE adds CCCD automatically; Bluedroid requires explicit descriptors.
#if defined(CONFIG_BLUEDROID_ENABLED)
  streamChar->addDescriptor(new BLE2902());
  eventChar->addDescriptor(new BLE2902());
  ackChar->addDescriptor(new BLE2902());
#endif
  service->start();
  BLEAdvertising *adv = BLEDevice::getAdvertising(); adv->addServiceUUID(UUID_SERVICE);
  adv->setScanResponse(true); BLEDevice::startAdvertising();
  Serial.printf("[BLE] READY | %s\n", tenBle);
}
void guiTelemetry() {
  if (!ketNoi || !streamEnabled) return;
  if (millis() - mau.timestampMs > cauHinh.sampleWatchdogMs) mau.hopLe = false;
  if (millis() - lucBaro > 120) coKhiAp = false;
  char imu[200], baro[120], json[1024];
  if (mau.hopLe) snprintf(imu, sizeof(imu), "%.3f,\"accelYMs2\":%.3f,\"accelZMs2\":%.3f,\"gyroXDps\":%.2f,\"gyroYDps\":%.2f,\"gyroZDps\":%.2f",
     mau.ax, mau.ay, mau.az, mau.gx, mau.gy, mau.gz);
  else snprintf(imu, sizeof(imu), "null,\"accelYMs2\":null,\"accelZMs2\":null,\"gyroXDps\":null,\"gyroYDps\":null,\"gyroZDps\":null");
  if (coKhiAp && coMoc) snprintf(baro, sizeof(baro), "%.1f,\"temperatureC\":%.2f,\"altitudeDeltaM\":%.3f", apSuat, nhietDo, doCaoTuongDoi);
  else if (coKhiAp) snprintf(baro, sizeof(baro), "%.1f,\"temperatureC\":%.2f,\"altitudeDeltaM\":null", apSuat, nhietDo);
  else snprintf(baro, sizeof(baro), "null,\"temperatureC\":null,\"altitudeDeltaM\":null");
  // Cac truong bat buoc cua Esp32PacketDecoder duoc giu nguyen; so do lon la extension.
  char mag[24], omega[24];
  if (mau.hopLe) {
    snprintf(mag, sizeof(mag), "%.3f", mau.doLonGiaToc);
    snprintf(omega, sizeof(omega), "%.2f", mau.vanTocGoc);
  } else {
    strcpy(mag, "null"); strcpy(omega, "null");
  }
  int n = snprintf(json, sizeof(json), "{\"protocolVersion\":1,\"deviceId\":\"%s\",\"sequenceNumber\":%lu,\"timestampMs\":%lu,\"accelXMs2\":%s,\"pressurePa\":%s,\"batteryPercent\":-1,\"batteryVoltageMv\":null,\"isCharging\":false,\"sosButtonPressed\":false,\"sensorQuality\":%d,\"accelerationMagnitudeMs2\":%s,\"angularSpeedDps\":%s,\"espState\":\"%s\",\"testMode\":%s}",
    tenBle, (unsigned long)++soThuTu, (unsigned long)mau.timestampMs, imu, baro,
    mau.hopLe ? (coKhiAp ? 100 : 80) : 0,
    mag, omega,
    dangNghiNgo ? "CANDIDATE" : "NORMAL", cauHinh.cheDoThuNghiem ? "true" : "false");
  if (n > 0 && n < (int)sizeof(json)) guiKhung(streamChar, 1, json);
}
void kiemTraSuKien() {
  if (!mau.hopLe) {
    dangNghiNgo = daCoLowG = daBaoLowG = daBaoBatDong = false;
    lucMauHopLe = lucBatDong = soMauBatDong = 0;
    return;
  }
  uint32_t now = mau.timestampMs;
  // Mau den qua tre thi khong duoc ghep vao cua so bat dong cu.
  if (lucMauHopLe && now - lucMauHopLe > cauHinh.sampleWatchdogMs) {
    dangNghiNgo = daCoLowG = daBaoLowG = daBaoBatDong = false;
    lucBatDong = soMauBatDong = 0;
  }
  if (lucMauHopLe && now - lucMauHopLe > cauHinh.maxSampleGapMs) {
    lucBatDong = soMauBatDong = 0;
  }
  lucMauHopLe = now;
  float g = mau.doLonGiaToc / G;
  if (daCoLowG && now - lucLowG > cauHinh.windowMs) daCoLowG = daBaoLowG = false;
  if (g < cauHinh.lowG) {
    if (!daCoLowG) { daCoLowG = true; lucLowG = now; }
    if (!daBaoLowG && now - lucLowG >= cauHinh.lowDurationMs) {
      daBaoLowG = true; Serial.println("[EVENT] LOW_G");
    }
  } else if (!daBaoLowG) {
    daCoLowG = false; // Roi qua ngan thi khong tinh la low-G.
  }
  if (mau.doLonGiaToc >= cauHinh.impact) {
    if (!dangNghiNgo) {
      // Chung cu phu chi tang uu tien; khong duoc dung ap suat mot minh.
      int doTinCay = 40 + (daBaoLowG ? 20 : 0) +
        (mau.vanTocGoc >= cauHinh.gyro ? 20 : 0) +
        (coKhiAp && coMoc && doCaoTuongDoi <= cauHinh.deltaH ? 10 : 0);
      Serial.printf("[EVENT] IMPACT | lowG=%s gyro=%.0f dH=%s\n",
        daBaoLowG ? "yes" : "no", mau.vanTocGoc,
        coKhiAp && coMoc ? "available" : "unknown");
      guiEvent("IMPACT_DETECTED", doTinCay);
    }
    dangNghiNgo = true; lucImpact = now; lucBatDong = 0; daBaoBatDong = false;
    soMauBatDong = 0;
    if (coKhiAp && coMoc) { apSuatGoc = apSuat; lucApSuatGoc = now; }
    else lucApSuatGoc = 0;
  }
  if (!dangNghiNgo) return;
  if (now - lucImpact > cauHinh.windowMs) { dangNghiNgo = daCoLowG = daBaoLowG = false; return; }
  // Low-G, gyro va deltaH la bang chung bo sung; khong tu ket luan SOS.
  if (fabsf(mau.doLonGiaToc - cauHinh.stillTarget) <= cauHinh.stillTolerance &&
      mau.vanTocGoc < min(15.0f, cauHinh.gyro * 0.2f)) {
    if (!soMauBatDong) lucBatDong = now;
    ++soMauBatDong;
    if (!daBaoBatDong && soMauBatDong >= cauHinh.minStillSamples &&
        now - lucBatDong >= cauHinh.stillMs) {
      daBaoBatDong = true; Serial.println("[EVENT] POST_IMPACT_STILL");
      // Ap suat tang khi ha thap, chi la chung cu phu sau va cham MPU.
      bool apSuatHoTro = lucApSuatGoc && coKhiAp && coMoc &&
        now - lucApSuatGoc <= cauHinh.pressureWindowMs &&
        apSuat - apSuatGoc >= cauHinh.pressureRisePa;
      guiEvent("INACTIVITY_DETECTED", 60 + (apSuatHoTro ? 10 : 0));
    }
  } else { lucBatDong = 0; soMauBatDong = 0; }
}
void inTrangThaiSerial(uint32_t now) {
  if (now - lucIn < 1000) return; lucIn = now;
  if (mau.hopLe) Serial.printf("[MPU] a=%.2fg | gyro=%.1f deg/s\n", mau.doLonGiaToc / G, mau.vanTocGoc);
  else Serial.println("[MPU] -- (khong co mau hop le)");
  if (coKhiAp && coMoc) Serial.printf("[GY63] P=%.2f hPa | dH=%.2f m\n", apSuat/100, doCaoTuongDoi);
  else Serial.println("[GY63] -- (chua co mau/moc)");
  Serial.printf("[STATE] %s | %s | BLE %s\n", dangNghiNgo ? "CANDIDATE" : "NORMAL",
    cauHinh.cheDoThuNghiem ? "TEST" : "NORMAL", ketNoi ? "connected" : "advertising");
}
void setup() {
  Serial.begin(115200);
  Serial.println("[BOOT] ESP32 khoi dong | BLE_ONLY_I2C100_V2");
  Serial.println("[I2C] MPU SDA=8 SCL=9 | GY63 SDA=7 SCL=6 | 100kHz");
  // Khoi tao BLE truoc khi lay moc khi ap, de dien thoai thay duoc thiet bi ngay.
  khoiTaoBLE();
  bool imuReady = khoiTaoMPU(), baroReady = khoiTaoGY63();
  Serial.printf("[MPU6050] %s | addr=0x%02X | SDA=8 SCL=9\n", imuReady ? "OK" : "ERROR", diaChiMpu);
  Serial.printf("[GY63] %s | addr=0x%02X | SDA=7 SCL=6\n", baroReady ? "OK" : "ERROR", diaChiGy63);
  Serial.println("[MODE] NORMAL (TEST chi bat qua BLE)");
  mocLayMauUs = micros(); lucBaro = millis(); lucThuLai = millis();
}
void loop() {
  uint32_t now = millis();
  if (quangBaCho) {
    quangBaCho = false;
    if (!ketNoi) BLEDevice::startAdvertising();
  }
  char local[sizeof(lenhCho)] = {};
  bool coLenh = false, quaDai = false;
  portENTER_CRITICAL(&khoaLenh);
  if (coLenhCho) { strcpy(local, lenhCho); coLenh = true; coLenhCho = false; }
  if (lenhQuaDai) { quaDai = true; lenhQuaDai = false; }
  portEXIT_CRITICAL(&khoaLenh);
  if (quaDai) guiAck(false, "Command too long or busy");
  if (coLenh) nhanCauHinhAndroid(local);
  if (now - lucThuLai >= 5000) {
    lucThuLai = now;
    if (!mpuOk) Serial.printf("[MPU6050] retry: %s\n", khoiTaoMPU() ? "OK" : "ERROR");
    if (!gy63Ok) Serial.printf("[GY63] retry: %s\n", khoiTaoGY63() ? "OK" : "ERROR");
  }
  now = millis(); // Init may block; do not schedule or assess freshness with its old time.
  docGY63(now);
  quetGY63TungBuoc(now);
  uint32_t us = micros();
  if ((int32_t)(us - mocLayMauUs) >= 0) {
    mocLayMauUs += 10000; // 100 Hz IMU, 25 Hz BLE; khong phat bu cac slot tre.
    if ((int32_t)(us - mocLayMauUs) >= 0) mocLayMauUs = us + 10000;
    docCamBien(); kiemTraSuKien();
  }
  if (now - lucBle >= 40) { lucBle = now; guiTelemetry(); }
  inTrangThaiSerial(now);
}
