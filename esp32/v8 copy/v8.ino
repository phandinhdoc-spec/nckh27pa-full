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

// ESP32-S3 Super Mini, theo esp.md
TwoWire &mpuBus = Wire;   // SDA 8, SCL 9
TwoWire &gyBus = Wire1;   // SDA 6, SCL 7
constexpr uint8_t MPU_ADDR = 0x68;
constexpr uint8_t GY63_ADDR = 0x77;
// Android loc quang ba va tim GATT service theo UUID nay.
constexpr const char *BLE_SERVICE_UUID = "7d2a0001-6f45-4c2b-9a1e-38a8f5c10001";
constexpr const char *BLE_TELEMETRY_UUID = "7d2a0002-6f45-4c2b-9a1e-38a8f5c10001";
constexpr const char *BLE_EVENT_UUID = "7d2a0003-6f45-4c2b-9a1e-38a8f5c10001";
constexpr const char *BLE_REQUEST_UUID = "7d2a0005-6f45-4c2b-9a1e-38a8f5c10001";
constexpr const char *BLE_ACK_UUID = "7d2a0006-6f45-4c2b-9a1e-38a8f5c10001";
std::atomic<bool> bleConnected{false};
std::atomic<bool> eventSubscribed{false};
std::atomic<bool> sosRestartRequested{false};
std::atomic<bool> sosNotifyFailed{false};
std::atomic<bool> restartAdvertising{false};
std::atomic<bool> telemetryRequested{false};
std::atomic<uint16_t> sosAckReceivedId{0};
std::atomic<uint16_t> blePeerMtu{23};
BLECharacteristic *telemetryCharacteristic = nullptr;
BLECharacteristic *eventCharacteristic = nullptr;
BLECharacteristic *ackCharacteristic = nullptr;
std::atomic<bool> profilePending{false};
char profileInput[512] = {};
size_t profileInputLength = 0;
Preferences profileStorage;
uint32_t telemetrySequence = 0;
char telemetryPayload[512] = {};
size_t telemetryLength = 0;
size_t telemetryOffset = 0;
uint16_t telemetryFrameIndex = 0;
uint16_t telemetryFrameCount = 0;
char sosPayload[96] = {};
size_t sosLength = 0;
size_t sosOffset = 0;
uint16_t sosFrameIndex = 0;
uint16_t sosFrameCount = 0;
uint16_t sosEventId = 0;
bool sosIsTest = false;
bool sosAwaitingAck = false;
uint32_t sosLastSendMs = 0;
uint32_t sosQueuedAtMs = 0;
constexpr uint32_t SOS_ACK_DEADLINE_MS = 5000;

struct Gy63Sample {
  uint32_t timeMs;
  float pressurePa;
  float temperatureC;
  float altitudeDeltaM; // So voi ap suat o mau hop le dau tien sau khoi dong.
  bool valid;
};
Gy63Sample latestGy63 = {};
uint16_t gy63Prom[8] = {};
float gy63BaselinePa = 0;
bool gy63Ready = false;
uint32_t gy63SampleCount = 0;
enum class Gy63Phase : uint8_t { IDLE, WAIT_TEMPERATURE, WAIT_PRESSURE };
Gy63Phase gy63Phase = Gy63Phase::IDLE;
uint32_t gy63ConversionUs = 0;
uint32_t gy63NextSampleMs = 0;
uint32_t gy63RawTemperature = 0;
float gy63FilteredPa = 0;
double gy63WarmupSumPa = 0;
uint8_t gy63WarmupSamples = 0;

// Luu mau moi nhat trong RAM va lich su CSV trong flash.
struct MpuSample {
  uint32_t timeMs;
  float ax, ay, az; // m/s^2
  float gx, gy, gz; // rad/s
  bool valid;
};

Adafruit_MPU6050 mpu;
MpuSample latestMpu = {};
bool mpuReady = false;
uint32_t mpuSampleCount = 0;
bool mpuStorageReady = false;
bool mpuStorageFull = false;
FallDetector fallDetector;
constexpr const char *MPU_LOG_PATH = "/mpu.csv";
void queueSosEvent(uint16_t eventId, bool test = false);
bool ping(TwoWire &bus, uint8_t addr);

void recoverMpuBus() {
  // Neu MPU giu SDA thap sau mot giao dich loi, giai phong bus truoc khi thu lai.
  mpuBus.end();
  pinMode(8, INPUT_PULLUP);
  pinMode(9, OUTPUT_OPEN_DRAIN);
  digitalWrite(9, HIGH);
  for (uint8_t i = 0; i < 9 && digitalRead(8) == LOW; ++i) {
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
  mpuBus.begin(8, 9, 100000);
  mpuBus.setTimeOut(5);
}

void startMpuReader() {
  static uint32_t nextRetryMs = 0;
  uint32_t now = millis();
  if (nextRetryMs != 0 && int32_t(now - nextRetryMs) < 0) return;
  nextRetryMs = now + 5000;
  if (!ping(mpuBus, MPU_ADDR)) recoverMpuBus();
  mpuReady = mpu.begin(MPU_ADDR, &mpuBus);
  if (mpuReady) {
    // 25 m/s^2 vuot qua thang +/-2g mac dinh; lay mau 100 Hz.
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
    mpu.setSampleRateDivisor(9);
  }
  Serial.println(mpuReady ? "MPU-6050: san sang doc 6 truc" : "MPU-6050: khong khoi tao duoc");
}

void startMpuStorage() {
  // Khong tu dong format: giu nguyen du lieu flash neu mount that bai.
  mpuStorageReady = LittleFS.begin(false);
  if (!mpuStorageReady) {
    Serial.println("LittleFS chua san sang. Gui FORMAT_MPU qua Serial de tao he thong tep.");
  }
}

void printMpuLog() {
  if (!mpuStorageReady) return;
  File file = LittleFS.open(MPU_LOG_PATH, "r");
  if (!file) {
    Serial.println("Chua co du lieu MPU.");
    return;
  }
  Serial.println("BEGIN_MPU_CSV");
  while (file.available()) Serial.write(file.read());
  file.close();
  Serial.println("END_MPU_CSV");
}

void handleMpuSerialCommand() {
  if (!Serial.available()) return;
  String command = Serial.readStringUntil('\n');
  command.trim();
  if (command == "DUMP_MPU") {
    printMpuLog();
  } else if (command == "TEST_SOS") {
    if (sosLength != 0) {
      Serial.println("TEST_SOS: dang cho SOS truoc do");
    } else {
      static uint16_t testEventId = 40000;
      if (++testEventId == 0) testEventId = 40000;
      queueSosEvent(testEventId, true);
      Serial.printf("TEST_SOS: da tao canh bao gia, id=%u\n", testEventId);
    }
  } else if (command == "TEST_SOS_REAL") {
    if (!bleConnected.load() || !eventSubscribed.load()) {
      Serial.println("TEST_SOS_REAL: Android chua dang ky nhan event BLE");
    } else if (sosLength != 0) {
      Serial.println("TEST_SOS_REAL: dang cho SOS truoc do");
    } else {
      static uint16_t testEventId = 50000;
      if (++testEventId == 0) testEventId = 50000;
      queueSosEvent(testEventId);
      Serial.printf("TEST_SOS_REAL: da tao SOS that, id=%u\n", testEventId);
    }
  } else if (command == "FORMAT_MPU" && !mpuStorageReady) {
    if (LittleFS.format()) {
      startMpuStorage();
      Serial.println(mpuStorageReady ? "LittleFS da san sang" : "LittleFS mount that bai");
    } else {
      Serial.println("LittleFS format that bai");
    }
  }
}

void readAndStoreMpu() {
  if (!mpuReady) startMpuReader();
  if (!mpuReady || !ping(mpuBus, MPU_ADDR)) {
    latestMpu.valid = false;
    Serial.println("MPU-6050: khong co mau moi");
    return;
  }

  sensors_event_t acceleration, rotation, temperature;
  if (!mpu.getEvent(&acceleration, &rotation, &temperature)) {
    latestMpu.valid = false;
    Serial.println("MPU-6050: doc that bai");
    return;
  }
  latestMpu = {millis(), acceleration.acceleration.x, acceleration.acceleration.y,
               acceleration.acceleration.z, rotation.gyro.x, rotation.gyro.y,
               rotation.gyro.z, true};
  Serial.printf("MPU ax=%.3f ay=%.3f az=%.3f m/s^2 | gx=%.3f gy=%.3f gz=%.3f rad/s\n",
                latestMpu.ax, latestMpu.ay, latestMpu.az,
                latestMpu.gx, latestMpu.gy, latestMpu.gz);

  if (!mpuStorageReady || mpuStorageFull) return;
  File file = LittleFS.open(MPU_LOG_PATH, "a");
  if (!file) {
    Serial.println("MPU-6050: khong mo duoc tep CSV");
    return;
  }
  if (file.size() >= LittleFS.totalBytes() * 3 / 4) {
    mpuStorageFull = true;
    file.close();
    Serial.println("MPU-6050: tep CSV da dat gioi han 75% dung luong LittleFS");
    return;
  }
  if (file.size() == 0) file.println("time_ms,ax_m_s2,ay_m_s2,az_m_s2,gx_rad_s,gy_rad_s,gz_rad_s");
  file.printf("%lu,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
              (unsigned long)latestMpu.timeMs, latestMpu.ax, latestMpu.ay,
              latestMpu.az, latestMpu.gx, latestMpu.gy, latestMpu.gz);
  file.close();
}

bool sampleMpuForFall(float &accelerationMs2, float &rotationDps) {
  static uint8_t errors = 0;
  if (!mpuReady) return false;
  uint8_t bytes[14];
  mpuBus.beginTransmission(MPU_ADDR);
  mpuBus.write(0x3B); // ACCEL_XOUT_H; doc lien tuc 6 truc va nhiet do.
  if (mpuBus.endTransmission(false) != 0 ||
      mpuBus.requestFrom(MPU_ADDR, (uint8_t)sizeof(bytes)) != sizeof(bytes)) {
    if (++errors >= 3) mpuReady = false;
    while (mpuBus.available()) mpuBus.read();
    latestMpu.valid = false;
    return false;
  }
  errors = 0;
  for (uint8_t i = 0; i < sizeof(bytes); ++i) bytes[i] = mpuBus.read();
  auto axis = [&](uint8_t index) -> int16_t {
    return int16_t((uint16_t(bytes[index]) << 8) | bytes[index + 1]);
  };
  constexpr float accelScale = 9.80665f / 2048.0f; // +/-16g
  constexpr float gyroScale = 1.0f / 16.4f;         // +/-2000 deg/s
  float ax = axis(0) * accelScale, ay = axis(2) * accelScale, az = axis(4) * accelScale;
  float gx = axis(8) * gyroScale, gy = axis(10) * gyroScale, gz = axis(12) * gyroScale;
  accelerationMs2 = sqrtf(ax * ax + ay * ay + az * az);
  rotationDps = sqrtf(gx * gx + gy * gy + gz * gz);
  latestMpu = {millis(), ax, ay, az, gx * 0.017453293f, gy * 0.017453293f,
               gz * 0.017453293f, true};
  ++mpuSampleCount;
  return true;
}

class BleServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *) override {
    blePeerMtu.store(23);
    eventSubscribed.store(false);
    bleConnected.store(true);
  }

  void onDisconnect(BLEServer *) override {
    bleConnected.store(false);
    eventSubscribed.store(false);
    telemetryRequested.store(false);
    sosRestartRequested.store(true);
    restartAdvertising.store(true);
  }

#if defined(CONFIG_NIMBLE_ENABLED)
  void onMtuChanged(BLEServer *, ble_gap_conn_desc *, uint16_t mtu) override {
    blePeerMtu.store(mtu);
  }
#elif defined(CONFIG_BLUEDROID_ENABLED)
  void onMtuChanged(BLEServer *, esp_ble_gatts_cb_param_t *param) override {
    blePeerMtu.store(param->mtu.mtu);
  }
#endif
};

class EventDescriptorCallbacks : public BLEDescriptorCallbacks {
  void onWrite(BLEDescriptor *descriptor) override {
    const uint8_t *value = descriptor->getValue();
    eventSubscribed.store(descriptor->getLength() >= 2 && (value[0] & 1) != 0);
  }
};

class EventCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onStatus(BLECharacteristic *, Status status, uint32_t) override {
    if (status != SUCCESS_NOTIFY) {
      sosNotifyFailed.store(true);
      if (status == ERROR_NOTIFY_DISABLED || status == ERROR_NO_SUBSCRIBER)
        eventSubscribed.store(false);
    }
  }
#if defined(CONFIG_NIMBLE_ENABLED)
  void onSubscribe(BLECharacteristic *, ble_gap_conn_desc *, uint16_t value) override {
    eventSubscribed.store((value & 1) != 0);
  }
#endif
};

class BleRequestCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    // BLE callback chi dat co; viec dong goi/gap I2C chay trong loop().
    String value = characteristic->getValue();
    if (value == "GET_TELEMETRY") {
      telemetryRequested.store(true);
    } else if (value.startsWith("SOS_ACK:")) {
      long id = value.substring(8).toInt();
      if (id > 0 && id <= 65535) sosAckReceivedId.store(uint16_t(id));
    } else if (!profilePending.load() && value.length() > 0 && value.length() < sizeof(profileInput) && value[0] == '{') {
      memcpy(profileInput, value.c_str(), value.length());
      profileInput[value.length()] = 0;
      profileInputLength = value.length();
      profilePending.store(true);
    }
  }
};

void startBLE() {
  char name[20];
  snprintf(name, sizeof(name), "FALLSAFE-%04X", (unsigned)(ESP.getEfuseMac() & 0xFFFF));

  BLEDevice::init(name);
  BLEDevice::setMTU(512);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new BleServerCallbacks());
  BLEService *service = server->createService(BLE_SERVICE_UUID);
  telemetryCharacteristic = service->createCharacteristic(BLE_TELEMETRY_UUID,
                                                          BLECharacteristic::PROPERTY_NOTIFY);
  telemetryCharacteristic->addDescriptor(new BLE2902());
  eventCharacteristic = service->createCharacteristic(BLE_EVENT_UUID,
                                                      BLECharacteristic::PROPERTY_NOTIFY);
  eventCharacteristic->setCallbacks(new EventCharacteristicCallbacks());
  BLE2902 *eventDescriptor = new BLE2902();
  eventDescriptor->setCallbacks(new EventDescriptorCallbacks());
  eventCharacteristic->addDescriptor(eventDescriptor);
  BLECharacteristic *requestCharacteristic = service->createCharacteristic(
      BLE_REQUEST_UUID, BLECharacteristic::PROPERTY_WRITE);
  requestCharacteristic->setCallbacks(new BleRequestCallbacks());
  ackCharacteristic = service->createCharacteristic(BLE_ACK_UUID,
                                                    BLECharacteristic::PROPERTY_NOTIFY);
  ackCharacteristic->addDescriptor(new BLE2902());
  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(BLE_SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.printf("BLE dang quang ba: %s\n", name);
}

bool ping(TwoWire &bus, uint8_t addr) {
  bus.beginTransmission(addr);
  return bus.endTransmission() == 0;
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
  if (!ping(mpuBus, MPU_ADDR)) {
    Serial.println("MPU-6050: khong phan hoi tai 0x68");
    Serial.print("I2C MPU GPIO8/9 tim thay:");
    bool found = false;
    for (uint8_t addr = 0x08; addr <= 0x77; ++addr) {
      if (ping(mpuBus, addr)) {
        Serial.printf(" 0x%02X", addr);
        found = true;
      }
    }
    if (!found) Serial.print(" khong co thiet bi");
    Serial.println();
    if (ping(gyBus, 0x68) || ping(gyBus, 0x69))
      Serial.println("MPU co the dang noi nham bus GPIO6/7");
    mpuBus.end();
    mpuBus.begin(9, 8, 100000); // Chi kiem tra SDA/SCL bi dao, sau do tra ve so do chuan.
    if (ping(mpuBus, 0x68) || ping(mpuBus, 0x69))
      Serial.println("MPU co the dang dao SDA/SCL GPIO8/9");
    mpuBus.end();
    mpuBus.begin(8, 9, 100000);
    mpuBus.setTimeOut(5);
    return;
  }
  uint8_t id;
  if (!readRegister(mpuBus, MPU_ADDR, 0x75, id)) {
    Serial.println("MPU-6050: loi doc WHO_AM_I");
    return;
  }
  Serial.printf("MPU-6050: dia chi 0x%02X, WHO_AM_I=0x%02X %s\n",
                MPU_ADDR, id, (id == 0x68) ? "OK" : "(kiem tra lai chip)");
}

void testGY63() {
  if (!ping(gyBus, GY63_ADDR)) {
    Serial.println("GY-63/MS5611: khong phan hoi tai 0x77");
    return;
  }
  gyBus.beginTransmission(GY63_ADDR);
  gyBus.write(0x1E); // Lenh reset MS5611
  if (gyBus.endTransmission() != 0) {
    Serial.println("GY-63/MS5611: loi reset");
    return;
  }
  delay(4);
  uint8_t hi, lo;
  gyBus.beginTransmission(GY63_ADDR);
  gyBus.write(0xA2); // PROM C1, he so hieu chuan ap suat
  if (gyBus.endTransmission(false) != 0 || gyBus.requestFrom(GY63_ADDR, (uint8_t)2) != 2) {
    Serial.println("GY-63/MS5611: loi doc PROM");
    return;
  }
  hi = gyBus.read();
  lo = gyBus.read();
  uint16_t c1 = ((uint16_t)hi << 8) | lo;
  Serial.printf("GY-63/MS5611: dia chi 0x%02X, PROM C1=%u %s\n",
                GY63_ADDR, c1, (c1 != 0 && c1 != 0xFFFF) ? "OK" : "(gia tri bat thuong)");
}

bool gy63Command(uint8_t command) {
  gyBus.beginTransmission(GY63_ADDR);
  gyBus.write(command);
  return gyBus.endTransmission() == 0;
}

bool gy63ReadBytes(uint8_t command, uint8_t *out, uint8_t count) {
  gyBus.beginTransmission(GY63_ADDR);
  gyBus.write(command);
  if (gyBus.endTransmission(false) != 0) return false;
  if (gyBus.requestFrom(GY63_ADDR, count) != count) {
    while (gyBus.available()) gyBus.read();
    return false;
  }
  for (uint8_t i = 0; i < count; ++i) out[i] = gyBus.read();
  return true;
}

uint8_t gy63PromCrc(const uint16_t *prom) {
  uint16_t remainder = 0;
  for (uint8_t i = 0; i < 16; ++i) {
    uint16_t word = prom[i / 2];
    if (i / 2 == 7) word &= 0xFF00; // Bo 4 bit CRC luu trong PROM[7].
    remainder ^= (i & 1) ? (word & 0xFF) : (word >> 8);
    for (uint8_t bit = 0; bit < 8; ++bit) {
      remainder = (remainder & 0x8000) ? uint16_t((remainder << 1) ^ 0x3000)
                                         : uint16_t(remainder << 1);
    }
  }
  return (remainder >> 12) & 0x0F;
}

void startGy63Reader() {
  gy63Ready = false;
  latestGy63.valid = false;
  gy63Phase = Gy63Phase::IDLE;
  gy63NextSampleMs = millis() + 5000; // Ca loi som cung khong duoc retry moi vong lap.
  if (!gy63Command(0x1E)) return; // Reset, nap lai he so hieu chuan.
  delay(4);
  for (uint8_t i = 0; i < 8; ++i) {
    uint8_t bytes[2];
    if (!gy63ReadBytes(0xA0 + 2 * i, bytes, 2)) return;
    gy63Prom[i] = (uint16_t(bytes[0]) << 8) | bytes[1];
    if (i >= 1 && i <= 6 && (gy63Prom[i] == 0 || gy63Prom[i] == 0xFFFF)) return;
  }
  gy63Ready = gy63PromCrc(gy63Prom) == (gy63Prom[7] & 0x0F);
  if (!gy63Ready && (gy63Prom[7] & 0x0F) == 0) {
    // Module dang dung de trong CRC; chi chap nhan neu ca 8 tu PROM doc lai y het.
    gy63Ready = true;
    for (uint8_t i = 0; i < 8; ++i) {
      uint8_t bytes[2];
      if (!gy63ReadBytes(0xA0 + 2 * i, bytes, 2) ||
          gy63Prom[i] != ((uint16_t(bytes[0]) << 8) | bytes[1])) {
        gy63Ready = false;
        break;
      }
    }
    if (gy63Ready) Serial.println("GY-63: CRC PROM=0, da doi chieu he so 2 lan");
  } else if (gy63Ready) {
    Serial.println("GY-63: he so hieu chuan CRC OK");
  }
  if (!gy63Ready) Serial.println("GY-63: he so hieu chuan khong hop le");
  if (gy63Ready) {
    gy63BaselinePa = 0;
    gy63FilteredPa = 0;
    gy63WarmupSumPa = 0;
    gy63WarmupSamples = 0;
    gy63NextSampleMs = millis();
  } else {
    gy63NextSampleMs = millis() + 5000;
  }
}

bool gy63ReadAdc(uint32_t &adc) {
  uint8_t bytes[3];
  if (!gy63ReadBytes(0x00, bytes, 3)) return false;
  adc = (uint32_t(bytes[0]) << 16) | (uint32_t(bytes[1]) << 8) | bytes[2];
  return adc != 0 && adc != 0xFFFFFF;
}

void readGy63() {
  uint32_t now = millis();
  if (!gy63Ready && int32_t(now - gy63NextSampleMs) >= 0) startGy63Reader();
  if (!gy63Ready) {
    latestGy63.valid = false;
    return;
  }

  if (gy63Phase == Gy63Phase::IDLE) {
    if (int32_t(now - gy63NextSampleMs) < 0) return;
    if (gy63Command(0x58)) { // D2, OSR 4096.
      gy63ConversionUs = micros();
      gy63Phase = Gy63Phase::WAIT_TEMPERATURE;
      return;
    }
  } else if (uint32_t(micros() - gy63ConversionUs) < 10000) {
    return; // Moi phep do can toi da 9.04 ms, khong chan lay mau MPU.
  } else if (gy63Phase == Gy63Phase::WAIT_TEMPERATURE) {
    if (gy63ReadAdc(gy63RawTemperature) && gy63Command(0x48)) {
      gy63ConversionUs = micros();
      gy63Phase = Gy63Phase::WAIT_PRESSURE;
      return;
    }
  } else {
    uint32_t d1;
    if (gy63ReadAdc(d1)) {
      gy63Phase = Gy63Phase::IDLE;
      gy63NextSampleMs = millis() + 20; // ~25 cap mau ap suat/giay.
      uint32_t d2 = gy63RawTemperature;
      int64_t dt = int64_t(d2) - int64_t(gy63Prom[5]) * 256;
      int64_t temperature = 2000 + dt * gy63Prom[6] / 8388608;
      int64_t offset = int64_t(gy63Prom[2]) * 65536 + int64_t(gy63Prom[4]) * dt / 128;
      int64_t sensitivity = int64_t(gy63Prom[1]) * 32768 + int64_t(gy63Prom[3]) * dt / 256;
      if (temperature < 2000) {
        int64_t square = (temperature - 2000) * (temperature - 2000);
        int64_t offset2 = 5 * square / 2;
        int64_t sensitivity2 = 5 * square / 4;
        if (temperature < -1500) {
          square = (temperature + 1500) * (temperature + 1500);
          offset2 += 7 * square;
          sensitivity2 += 11 * square / 2;
        }
        temperature -= dt * dt / 2147483648LL;
        offset -= offset2;
        sensitivity -= sensitivity2;
      }
      float pressurePa = float((int64_t(d1) * sensitivity / 2097152 - offset) / 32768);
      float temperatureC = float(temperature) / 100.0f;
      if (pressurePa < 1000 || pressurePa > 120000 || temperatureC < -40 || temperatureC > 85) {
        latestGy63.valid = false;
        return;
      }
      ++gy63SampleCount;
      if (gy63WarmupSamples < 25) {
        gy63WarmupSumPa += pressurePa;
        if (++gy63WarmupSamples == 25) {
          gy63BaselinePa = float(gy63WarmupSumPa / 25);
          gy63FilteredPa = pressurePa;
          Serial.printf("GY-63 P0=%.1f Pa (25 mau)\n", gy63BaselinePa);
        } else {
          latestGy63.valid = false;
          return;
        }
      }
      gy63FilteredPa += 0.20f * (pressurePa - gy63FilteredPa);
      float deltaM = 44330.0f * (1.0f - powf(gy63FilteredPa / gy63BaselinePa, 0.19029495f));
      latestGy63 = {millis(), pressurePa, temperatureC, deltaM, true};
      return;
    }
  }

  // Loi I2C/ADC: mau cu khong duoc dung de xac nhan nga.
  gy63Phase = Gy63Phase::IDLE;
  gy63Ready = false;
  gy63NextSampleMs = millis() + 5000;
  latestGy63.valid = false;
  Serial.println("GY-63: doc ADC that bai, thu lai sau 5s");
}

const char *fallStateName(FallState state) {
  switch (state) {
    case FallState::MONITORING: return "MONITORING";
    case FallState::SUSPECTED: return "SUSPECTED";
    case FallState::VERIFYING: return "VERIFYING";
    case FallState::ALERT: return "ALERT";
  }
  return "UNKNOWN";
}

void queueSosEvent(uint16_t eventId, bool test) {
  int count = snprintf(sosPayload, sizeof(sosPayload),
      "{\"eventId\":\"%u\",\"deviceId\":\"FALLSAFE-%04X\",\"eventType\":\"SOS_PRESSED\",\"test\":%s}",
      eventId, (unsigned)(ESP.getEfuseMac() & 0xFFFF), test ? "true" : "false");
  if (count <= 0 || size_t(count) >= sizeof(sosPayload)) return;
  sosIsTest = test;
  sosEventId = eventId;
  sosLength = size_t(count);
  sosOffset = 0;
  sosFrameIndex = 0;
  sosAwaitingAck = false;
  sosLastSendMs = 0;
  sosQueuedAtMs = millis();
  telemetryLength = 0; // Bo goi telemetry dang do de SOS khong xep sau no.
}

// Chi dua mot frame moi lan goi; notify() khong co nghia Android da nhan.
// Cho Android ghi SOS_ACK:<eventId> qua request characteristic, roi moi xoa SOS.
bool sendPendingSosStep(uint32_t now) {
  static uint32_t lastFrameMs = 0;
  if (sosLength != 0 && sosAckReceivedId.exchange(0) == sosEventId) {
    Serial.printf("SOS Android da nhan: eventId=%u\n", sosEventId);
    sosLength = 0;
    sosAwaitingAck = false;
    return false;
  }
  if (sosLength != 0 && uint32_t(now - sosQueuedAtMs) >= SOS_ACK_DEADLINE_MS) {
    Serial.printf("SOS khong co Android ACK sau %lu ms, bo hang doi: eventId=%u\n",
                  (unsigned long)SOS_ACK_DEADLINE_MS, sosEventId);
    sosLength = 0;
    sosAwaitingAck = false;
    return false;
  }
  if (sosRestartRequested.exchange(false)) {
    sosOffset = 0;
    sosFrameIndex = 0;
    sosAwaitingAck = false;
  }
  if (sosLength == 0 || !bleConnected.load() || !eventSubscribed.load() ||
      !eventCharacteristic) return false;
  if (sosAwaitingAck) {
    if (uint32_t(now - sosLastSendMs) < 1000) return false;
    sosOffset = 0;
    sosFrameIndex = 0;
    sosAwaitingAck = false;
    Serial.printf("SOS chua co ACK, gui lai: eventId=%u\n", sosEventId);
  }
  if (lastFrameMs != 0 && uint32_t(now - lastFrameMs) < 10) return true;

  uint16_t mtu = blePeerMtu.load();
  if (mtu < 23) mtu = 23;
  if (sosOffset == 0) {
    sosFrameCount = sosLength <= mtu - 3 ? 0 :
                    (sosLength + (mtu - 19) - 1) / (mtu - 19);
  }
  if (sosFrameCount == 0) {
    sosNotifyFailed.store(false);
    eventCharacteristic->setValue((uint8_t *)sosPayload, sosLength);
    eventCharacteristic->notify();
    if (!sosNotifyFailed.load() && bleConnected.load() && eventSubscribed.load()) sosAwaitingAck = true;
  } else {
    size_t length = min(size_t(mtu - 19), sosLength - sosOffset);
    uint8_t frame[112] = {0x46, 0x53, 1, 2}; // IF-003, kind=event.
    uint32_t id = sosEventId;
    frame[4] = id; frame[5] = id >> 8; frame[6] = id >> 16; frame[7] = id >> 24;
    frame[8] = sosFrameIndex; frame[9] = sosFrameIndex >> 8;
    frame[10] = sosFrameCount; frame[11] = sosFrameCount >> 8;
    frame[12] = sosLength; frame[13] = sosLength >> 8;
    frame[14] = sosOffset; frame[15] = sosOffset >> 8;
    memcpy(frame + 16, sosPayload + sosOffset, length);
    sosNotifyFailed.store(false);
    eventCharacteristic->setValue(frame, 16 + length);
    eventCharacteristic->notify();
    if (!sosNotifyFailed.load() && bleConnected.load() && eventSubscribed.load()) {
      sosOffset += length;
      ++sosFrameIndex;
      if (sosOffset == sosLength) sosAwaitingAck = true;
    }
  }
  lastFrameMs = now;
  if (sosAwaitingAck) {
    sosLastSendMs = now;
    Serial.printf("SOS BLE da xep gui, cho Android ACK: eventId=%u test=%u\n", sosEventId, sosIsTest);
  }
  return true;
}

void updateFallDetection(uint32_t now) {
  float accelerationMs2 = 0, rotationDps = 0;
  bool imuValid = sampleMpuForFall(accelerationMs2, rotationDps);
  bool mpuFresh = latestMpu.valid && uint32_t(now - latestMpu.timeMs) <= fallDetector.profile.sampleWatchdogMs;
  bool baroValid = latestGy63.valid && uint32_t(now - latestGy63.timeMs) <= fallDetector.profile.sampleWatchdogMs;
  FallState previous = fallDetector.state;
  bool confirmed = fallDetector.step(now, accelerationMs2, rotationDps,
                                     latestGy63.altitudeDeltaM, imuValid && mpuFresh && baroValid,
                                     latestGy63.pressurePa);
  if (confirmed) {
    queueSosEvent(fallDetector.eventId);
  } else if (fallDetector.state != previous) {
    Serial.printf("FALL_STATE: %s\n", fallStateName(fallDetector.state));
  }
}

bool parseFallProfile(const char *json, size_t length, FallProfile &out) {
  const char *end = nullptr;
  cJSON *root = cJSON_ParseWithLengthOpts(json, length + 1, &end, true);
  if (!root) return false;
  const char *names[14] = {
      "impactAccelerationMs2", "stillnessTargetAccelerationMs2", "stillnessToleranceMs2",
      "postImpactWindowMs", "postImpactStillnessDurationMs", "minimumStillnessSamples",
      "maximumSampleGapMs", "freeFallThresholdMs2", "freeFallMinDurationMs",
      "gyroTurnThresholdDps", "pressureEvidenceMinRisePa", "pressureWindowMs",
      "altitudeDropMinM", "sampleWatchdogMs"};
  double values[14] = {};
  bool valid = cJSON_IsObject(root) && end == json + length;
  if (valid) {
    int count = 0;
    for (const cJSON *item = root->child; item; item = item->next) ++count;
    valid = count == 14;
  }
  for (uint8_t i = 0; valid && i < 14; ++i) {
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(root, names[i]);
    valid = cJSON_IsNumber(item) && isfinite(item->valuedouble);
    if (valid) values[i] = item->valuedouble;
  }
  for (uint8_t i : {3, 4, 5, 6, 8, 11, 13}) {
    if (valid && (values[i] < 0 || values[i] > 10000 || floor(values[i]) != values[i])) valid = false;
  }
  if (valid) {
    out.impactAccelerationMs2 = values[0];
    out.stillnessTargetAccelerationMs2 = values[1];
    out.stillnessToleranceMs2 = values[2];
    out.postImpactWindowMs = values[3];
    out.postImpactStillnessDurationMs = values[4];
    out.minimumStillnessSamples = values[5];
    out.maximumSampleGapMs = values[6];
    out.freeFallThresholdMs2 = values[7];
    out.freeFallMinDurationMs = values[8];
    out.gyroTurnThresholdDps = values[9];
    out.pressureEvidenceMinRisePa = values[10];
    out.pressureWindowMs = values[11];
    out.altitudeDropMinM = values[12];
    out.sampleWatchdogMs = values[13];
    valid = out.valid();
  }
  cJSON_Delete(root);
  return valid;
}

void sendProfileAck(const char *status, const char *code, const char *message) {
  if (!bleConnected.load() || !ackCharacteristic) return;
  char ack[240];
  int count = snprintf(ack, sizeof(ack),
      "{\"protocolVersion\":1,\"commandId\":\"profile\",\"deviceId\":\"FALLSAFE-%04X\","
      "\"timestampMs\":%lu,\"commandStatus\":\"%s\",\"errorCode\":%s,\"message\":\"%s\"}",
      (unsigned)(ESP.getEfuseMac() & 0xFFFF), (unsigned long)millis(), status, code, message);
  if (count > 0 && size_t(count) < sizeof(ack) && count <= blePeerMtu.load() - 3) {
    ackCharacteristic->setValue((uint8_t *)ack, count);
    ackCharacteristic->notify();
  }
}

void loadFallProfile() {
  if (!profileStorage.begin("fallsafe", false)) {
    Serial.println("FallProfile: NVS khong san sang, dung mac dinh");
    return;
  }
  if (profileStorage.getBytesLength("profile") != sizeof(FallProfile)) return;
  FallProfile saved;
  if (profileStorage.getBytes("profile", &saved, sizeof(saved)) == sizeof(saved) && saved.valid()) {
    fallDetector.applyProfile(saved);
    Serial.println("FallProfile: da nap tu NVS");
  }
}

void handleProfileRequest() {
  if (!profilePending.load()) return;
  if (!bleConnected.load()) {
    profilePending.store(false);
    return;
  }
  FallProfile candidate;
  if (!parseFallProfile(profileInput, profileInputLength, candidate)) {
    sendProfileAck("REJECTED", "\"INVALID_PROFILE\"", "Invalid profile");
  } else if (profileStorage.putBytes("profile", &candidate, sizeof(candidate)) != sizeof(candidate)) {
    sendProfileAck("FAILED", "\"STORAGE_ERROR\"", "Could not save profile");
  } else {
    fallDetector.applyProfile(candidate);
    sendProfileAck("COMPLETED", "null", "Applied and saved");
    Serial.println("FallProfile: da cap nhat va luu NVS");
  }
  profilePending.store(false);
}

void prepareTelemetry(uint32_t now) {
  bool mpuValid = latestMpu.valid && uint32_t(now - latestMpu.timeMs) <= 150;
  bool gyValid = latestGy63.valid && uint32_t(now - latestGy63.timeMs) <= 200;
  char pressure[24] = "null", temperature[24] = "null", height[24] = "null";
  if (gyValid) {
    snprintf(pressure, sizeof(pressure), "%.1f", latestGy63.pressurePa);
    snprintf(temperature, sizeof(temperature), "%.2f", latestGy63.temperatureC);
    snprintf(height, sizeof(height), "%.3f", latestGy63.altitudeDeltaM);
  }
  char id[20];
  snprintf(id, sizeof(id), "FALLSAFE-%04X", (unsigned)(ESP.getEfuseMac() & 0xFFFF));
  uint32_t sequence = ++telemetrySequence;
  int count;
  if (mpuValid) {
    constexpr float toDps = 57.2957795f;
    count = snprintf(telemetryPayload, sizeof(telemetryPayload),
        "{\"protocolVersion\":1,\"deviceId\":\"%s\",\"sequenceNumber\":%lu,\"timestampMs\":%lu,"
        "\"accelXMs2\":%.3f,\"accelYMs2\":%.3f,\"accelZMs2\":%.3f,"
        "\"gyroXDps\":%.2f,\"gyroYDps\":%.2f,\"gyroZDps\":%.2f,"
        "\"pressurePa\":%s,\"temperatureC\":%s,\"altitudeDeltaM\":%s,"
        "\"batteryPercent\":-1,\"batteryVoltageMv\":null,\"isCharging\":false,"
        "\"sosButtonPressed\":false,\"sensorQuality\":%u}",
        id, (unsigned long)sequence, (unsigned long)now,
        latestMpu.ax, latestMpu.ay, latestMpu.az,
        latestMpu.gx * toDps, latestMpu.gy * toDps, latestMpu.gz * toDps,
        pressure, temperature, height, gyValid ? 100u : 50u);
  } else {
    count = snprintf(telemetryPayload, sizeof(telemetryPayload),
        "{\"protocolVersion\":1,\"deviceId\":\"%s\",\"sequenceNumber\":%lu,\"timestampMs\":%lu,"
        "\"accelXMs2\":null,\"accelYMs2\":null,\"accelZMs2\":null,"
        "\"gyroXDps\":null,\"gyroYDps\":null,\"gyroZDps\":null,"
        "\"pressurePa\":%s,\"temperatureC\":%s,\"altitudeDeltaM\":%s,"
        "\"batteryPercent\":-1,\"batteryVoltageMv\":null,\"isCharging\":false,"
        "\"sosButtonPressed\":false,\"sensorQuality\":0}",
        id, (unsigned long)sequence, (unsigned long)now, pressure, temperature, height);
  }
  if (count <= 0 || size_t(count) >= sizeof(telemetryPayload)) {
    telemetryLength = 0;
    Serial.println("BLE: telemetry vuot bo dem");
    return;
  }
  telemetryLength = size_t(count);
  telemetryOffset = 0;
  telemetryFrameIndex = 0;
  uint16_t mtu = blePeerMtu.load();
  if (mtu < 23) mtu = 23;
  if (telemetryLength <= mtu - 3) {
    telemetryFrameCount = 0; // Mot notification JSON.
  } else {
    uint16_t chunkSize = mtu - 19; // 16-byte IF-003 header + ATT overhead.
    telemetryFrameCount = (telemetryLength + chunkSize - 1) / chunkSize;
  }
}

void sendTelemetryStep(uint32_t now) {
  static uint32_t lastFrameMs = 0;
  if (!bleConnected.load()) {
    telemetryLength = 0;
    telemetryRequested.store(false);
    return;
  }
  if (telemetryLength == 0 && telemetryRequested.exchange(false)) prepareTelemetry(now);
  if (telemetryLength == 0 || !telemetryCharacteristic ||
      (telemetryFrameIndex > 0 && uint32_t(now - lastFrameMs) < 10)) return;

  if (telemetryFrameCount == 0) {
    telemetryCharacteristic->setValue((uint8_t *)telemetryPayload, telemetryLength);
    telemetryCharacteristic->notify();
    telemetryLength = 0;
  } else {
    uint16_t chunkSize = blePeerMtu.load() - 19;
    size_t length = min(size_t(chunkSize), telemetryLength - telemetryOffset);
    uint8_t frame[512] = {0x46, 0x53, 1, 1}; // IF-003, kind=telemetry.
    uint32_t id = telemetrySequence;
    frame[4] = id; frame[5] = id >> 8; frame[6] = id >> 16; frame[7] = id >> 24;
    frame[8] = telemetryFrameIndex; frame[9] = telemetryFrameIndex >> 8;
    frame[10] = telemetryFrameCount; frame[11] = telemetryFrameCount >> 8;
    frame[12] = telemetryLength; frame[13] = telemetryLength >> 8;
    frame[14] = telemetryOffset; frame[15] = telemetryOffset >> 8;
    memcpy(frame + 16, telemetryPayload + telemetryOffset, length);
    telemetryCharacteristic->setValue(frame, 16 + length);
    telemetryCharacteristic->notify();
    telemetryOffset += length;
    ++telemetryFrameIndex;
    if (telemetryOffset == telemetryLength) telemetryLength = 0;
  }
  lastFrameMs = now;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  mpuBus.begin(8, 9, 100000);
  gyBus.begin(6, 7, 100000);
  mpuBus.setTimeOut(5);
  gyBus.setTimeOut(5);
  Serial.println("Kiem tra I2C ESP32-S3");
  loadFallProfile();
  startMpuReader();
  testMPU();
  testGY63();
  startGy63Reader();
  startMpuStorage();
  startBLE();
}

void loop() {
  static uint32_t lastMpuRead = 0;
  static uint32_t nextFallSampleMs = 0;
  static uint32_t previousMpuSamples = 0, previousGy63Samples = 0;
  if (restartAdvertising.exchange(false)) {
    BLEDevice::startAdvertising();
    Serial.println("BLE da quang ba lai sau ngat ket noi");
  }
  uint32_t now = millis();
  if (int32_t(now - nextFallSampleMs) >= 0) {
    nextFallSampleMs = now + 10; // Khong chay bu cac mau khi bi tre.
    updateFallDetection(now);
  }
  readGy63();
  // MPU va GY-63 van duoc doc khi SOS dang truyen hoac cho ACK.
  bool sosPending = sendPendingSosStep(millis());
  if (!sosPending) {
    handleMpuSerialCommand();
    handleProfileRequest();
    sendTelemetryStep(now);
  }
  if (uint32_t(now - lastMpuRead) >= 3000) {
    uint32_t elapsedMs = now - lastMpuRead;
    lastMpuRead = now;
    if (!sosPending && fallDetector.state == FallState::MONITORING) readAndStoreMpu();
    if (latestGy63.valid) {
      Serial.printf("GY-63 P=%.1f Pa | T=%.2f C | dH=%+.3f m\n",
                    latestGy63.pressurePa, latestGy63.temperatureC,
                    latestGy63.altitudeDeltaM);
    } else {
      Serial.println("GY-63: chua co mau hop le");
    }
    Serial.printf("FALL: %s | sample_gaps=%lu\n", fallStateName(fallDetector.state),
                  (unsigned long)fallDetector.sampleGaps);
    Serial.printf("SAMPLE_RATE MPU=%lu Hz GY63=%lu Hz\n",
                  (unsigned long)((mpuSampleCount - previousMpuSamples) * 1000 / elapsedMs),
                  (unsigned long)((gy63SampleCount - previousGy63Samples) * 1000 / elapsedMs));
    previousMpuSamples = mpuSampleCount;
    previousGy63Samples = gy63SampleCount;
    Serial.printf("BLE: %s | SOS: %s\n\n",
                  bleConnected.load() ? "da ket noi" : "dang quang ba",
                  sosLength == 0 ? "khong cho" : (sosAwaitingAck ? "cho Android ACK" : "dang gui"));
  }
  delay(1);
}
