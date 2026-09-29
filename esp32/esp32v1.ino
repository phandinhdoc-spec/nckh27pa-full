/*
 * ESP32-S3 Super Mini - esp32v1
 * Mo file nay trong Arduino IDE; board: ESP32S3 Dev Module.
 * USB CDC On Boot: Enabled; Serial Monitor: 115200 baud.
 * Library Manager: Adafruit MPU6050 (+ BusIO, Unified Sensor),
 *                 MS5611 by Rob Tillaart (kiem tra voi 0.5.2).
 * BLE/Wire co san trong esp32 by Espressif Systems (kiem tra voi 3.3.12).
 *
 * Theo esp/v1, esp/esp2609 va esp/mycode trong project:
 *   1 MPU6050: SDA=8, SCL=9, dia chi 0x68 HOAC 0x69.
 *   1 MS5611 : SDA=7, SCL=6, dia chi 0x77 HOAC 0x76.
 *   1 IP5306 : chung SDA=8/SCL=9, dia chi 0x75 (neu ban co I2C).
 *   LED board: GPIO21. Cac dia chi du phong KHONG phai cam bien thu hai.
 *   Nguon/logic cam bien 3.3V, chung GND. Khong co nut/coi trong ban nay.
 * Cac ban test/hocsinh cu co pinmap khac; khong tron cac pinmap.
 *
 * BLE ten NCKH27PA-v1, dich vu Nordic UART (UUID ben duoi).
 * Dung BLE client: connect, subscribe TX Notify; RX Write nhan:
 *   PING -> PONG; ZERO -> lay lai moc cao do; START/STOP -> bat/tat stream.
 * Moi lenh la mot lan Write (co the them CR/LF). Serial cung nhan cac lenh nay.
 * TX la CSV ASCII, ghep cac notification den ky tu '\n' thanh mot dong.
 * Moi notification toi da 20 byte, hoat dong ca voi MTU mac dinh 23.
 * Dong D: ms, ax/ay/az (m/s2), gx/gy/gz (rad/s), P (Pa), T (C), h (m),
 *         mpu_ok, baro_ok, ip5306_ack. 'nan' nghia la khong co mau hop le.
 * Cao do tuong doi dung trung binh 20 mau ap suat dau; giu yen khi ZERO.
 * Chi doc cam bien va truyen du lieu, chua co thuat toan ket luan te nga.
 * Giao thuc UART nay khong thay the giao thuc app cu trong cac sketch khac.
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MS5611.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <atomic>
#include <math.h>

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
#error "Chon ESP32S3 Dev Module: pinmap nay danh cho ESP32-S3."
#endif

constexpr int MPU_SDA = 8, MPU_SCL = 9;
constexpr int BARO_SDA = 7, BARO_SCL = 6;
constexpr int LED_PIN = 21;
constexpr uint8_t IP5306_ADDR = 0x75;
constexpr uint32_t I2C_HZ = 100000;
constexpr char SERVICE_UUID[] = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
constexpr char RX_UUID[] = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
constexpr char TX_UUID[] = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

Adafruit_MPU6050 mpu;
MS5611 baro77(0x77, &Wire1), baro76(0x76, &Wire1);
MS5611 *baro = nullptr;
uint8_t mpuAddress = 0;
bool bus0 = false, bus1 = false, ipPresent = false, streaming = true;
float baseline = NAN;
double pressureSum = 0;
uint8_t baselineCount = 0;
uint32_t lastSample = 0, lastRetry = 0;
BLECharacteristic *tx = nullptr;
BLE2902 *txSubscription = nullptr;
std::atomic<bool> connected{false}, restartAdvertising{false};
QueueHandle_t commandQueue = nullptr;
struct Command { char text[24]; };

// Callback BLE chi dua lenh vao queue; I2C va notification chay trong loop.
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *) override { connected.store(true); }
  void onDisconnect(BLEServer *) override {
    connected.store(false);
    restartAdvertising.store(true);
  }
};

class RxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    String value = characteristic->getValue();
    value.trim();
    Command cmd{};
    if (value.length() >= sizeof(cmd.text)) return;
    value.toCharArray(cmd.text, sizeof(cmd.text));
    xQueueSend(commandQueue, &cmd, 0);
  }
};

bool acknowledges(TwoWire &bus, uint8_t address) {
  bus.beginTransmission(address);
  return bus.endTransmission() == 0;
}

void scanBus(TwoWire &bus, const char *name) {
  unsigned count = 0;
  Serial.printf("# %s:", name);
  for (uint8_t address = 8; address < 120; ++address) {
    if (acknowledges(bus, address)) {
      Serial.printf(" 0x%02X", address);
      ++count;
    }
  }
  Serial.printf(" (%u dia chi ACK)\n", count);
}

void resetBaseline() {
  baseline = NAN;
  pressureSum = 0;
  baselineCount = 0;
}

void detectSensors() {
  if (!bus0) bus0 = Wire.begin(MPU_SDA, MPU_SCL, I2C_HZ);
  if (!bus1) bus1 = Wire1.begin(BARO_SDA, BARO_SCL, I2C_HZ);
  Wire.setTimeOut(30);
  Wire1.setTimeOut(30);
  if (bus0 && !mpuAddress) {
    for (uint8_t address : {0x68, 0x69}) {
      if (acknowledges(Wire, address) && mpu.begin(address, &Wire)) {
        mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
        mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        mpuAddress = address;
        Serial.printf("# MPU6050 OK 0x%02X\n", address);
        break;
      }
    }
  }
  if (bus1 && !baro) {
    for (MS5611 *candidate : {&baro77, &baro76}) {
      if (candidate->begin()) {
        baro = candidate;
        baro->setOversampling(OSR_HIGH);
        // Dung mathMode mac dinh cua MS5611; khong tu dong nhan/chia ap suat.
        resetBaseline();
        Serial.printf("# MS5611 OK 0x%02X\n", baro->getAddress());
        break;
      }
    }
  }
  // Chi bao ACK cua IP5306, khong suy dien % pin tu code cu chua xac minh.
  ipPresent = bus0 && acknowledges(Wire, IP5306_ADDR);
  Serial.printf("# MPU=%s MS5611=%s IP5306_ACK=%d\n",
                mpuAddress ? "OK" : "MISSING", baro ? "OK" : "MISSING", ipPresent);
}

void sendLine(const char *line) {
  Serial.println(line);
  if (!connected.load() || !txSubscription->getNotifications()) return;
  String message = String(line) + '\n';
  for (size_t offset = 0; offset < message.length(); offset += 20) {
    if (!connected.load()) break;
    size_t count = message.length() - offset;
    if (count > 20) count = 20;
    tx->setValue(reinterpret_cast<const uint8_t *>(message.c_str() + offset), count);
    tx->notify();
    delay(5);  // Nhuong CPU/stack BLE, han che don notification.
  }
}

void handleCommand(char *text) {
  String command(text);
  command.trim();
  command.toUpperCase();
  if (command == "PING") sendLine("# PONG");
  else if (command == "ZERO") { resetBaseline(); sendLine("# ZERO: collecting 20 samples"); }
  else if (command == "START") { streaming = true; sendLine("# START OK"); }
  else if (command == "STOP") { streaming = false; sendLine("# STOP OK"); }
  else sendLine("# ERROR: use PING, ZERO, START, STOP");
}

void sampleSensors() {
  sensors_event_t a{}, g{}, t{};
  bool imuOK = false, baroOK = false;
  // Adafruit getEvent() khong phan anh day du loi doc I2C: kiem tra ACK
  // truoc/sau va gia tri huu han; van can thu tren phan cung khi day chap chon.
  if (mpuAddress) {
    imuOK = acknowledges(Wire, mpuAddress) && mpu.getEvent(&a, &g, &t)
         && acknowledges(Wire, mpuAddress)
         && isfinite(a.acceleration.x) && isfinite(a.acceleration.y)
         && isfinite(a.acceleration.z) && isfinite(g.gyro.x)
         && isfinite(g.gyro.y) && isfinite(g.gyro.z);
    if (!imuOK) mpuAddress = 0;
  }
  float pressure = NAN, temperature = NAN, height = NAN;
  if (baro) {
    if (baro->read() == MS5611_READ_OK) {
      pressure = baro->getPressurePascal();
      temperature = baro->getTemperature();
      baroOK = isfinite(pressure) && pressure >= 1000 && pressure <= 120000
            && isfinite(temperature) && temperature >= -40 && temperature <= 85;
    }
    if (!baroOK) {
      baro = nullptr;
      pressure = temperature = NAN;
      resetBaseline();
    } else {
      if (baselineCount < 20) {
        pressureSum += pressure;
        if (++baselineCount == 20) baseline = pressureSum / baselineCount;
      }
      if (isfinite(baseline)) height = 44330.0f * (1.0f - powf(pressure / baseline, 0.190295f));
    }
  }
  if (!streaming) return;
  char line[240];
  snprintf(line, sizeof(line),
           "D,%lu,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.1f,%.2f,%.2f,%d,%d,%d",
           (unsigned long)millis(), imuOK ? a.acceleration.x : NAN,
           imuOK ? a.acceleration.y : NAN, imuOK ? a.acceleration.z : NAN,
           imuOK ? g.gyro.x : NAN, imuOK ? g.gyro.y : NAN, imuOK ? g.gyro.z : NAN,
           pressure, temperature, height, imuOK, baroOK, ipPresent);
  sendLine(line);
}

void setup() {
  Serial.begin(115200);
  // Khong doi Serial: van khoi dong BLE khi cap nguon bang pin.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  commandQueue = xQueueCreate(8, sizeof(Command));
  if (!commandQueue) { Serial.println("# ERROR: command queue allocation"); return; }
  BLEDevice::init("NCKH27PA-v1");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());
  BLEService *service = server->createService(SERVICE_UUID);
  tx = service->createCharacteristic(TX_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  txSubscription = new BLE2902();
  tx->addDescriptor(txSubscription);
  BLECharacteristic *rx = service->createCharacteristic(RX_UUID, BLECharacteristic::PROPERTY_WRITE);
  rx->setCallbacks(new RxCallbacks());
  service->start();
  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->start();
  detectSensors();
  if (bus0) scanBus(Wire, "I2C0 SDA8 SCL9: MPU6050 + IP5306");
  if (bus1) scanBus(Wire1, "I2C1 SDA7 SCL6: MS5611");
  Serial.println("# D,ms,ax,ay,az,gx,gy,gz,pressure_Pa,temp_C,height_m,mpu_ok,baro_ok,ip5306_ack");
  lastRetry = millis();
}

void loop() {
  if (!commandQueue) { delay(100); return; }
  if (restartAdvertising.exchange(false)) {
    // Chi cho mot central. Xoa trang thai subscribe cua ket noi cu.
    txSubscription->setNotifications(false);
    BLEDevice::startAdvertising();
  }
  Command command{};
  if (xQueueReceive(commandQueue, &command, 0) == pdTRUE) handleCommand(command.text);
  static char serialLine[24];
  static size_t used = 0;
  static bool overflow = false;
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      if (used && !overflow) { serialLine[used] = '\0'; handleCommand(serialLine); }
      used = 0;
      overflow = false;
    } else if (used < sizeof(serialLine) - 1) serialLine[used++] = ch;
    else overflow = true;
  }
  uint32_t now = millis();
  if (now - lastRetry >= 5000) { lastRetry = now; detectSensors(); }
  now = millis();
  if (now - lastSample >= 100) { lastSample = now; sampleSensors(); }
  digitalWrite(LED_PIN, connected.load() ? HIGH : ((millis() / 500) % 2));
  delay(1);
}
