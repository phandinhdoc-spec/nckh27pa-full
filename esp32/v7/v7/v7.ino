/*
 * FallSafe v7.1 -- ESP32-S3 Super Mini -- Arduino-ESP32 3.3.12
 * Mo file trong thu muc v7/ bang Arduino IDE; chon ESP32S3 Dev Module,
 * USB CDC On Boot: Enabled, Partition Scheme: Huge APP (3MB No OTA).
 * Serial 115200. Khong can cai thu vien cam bien ben ngoai.
 * Wire va BLE la thanh phan cua ESP32 core; cam bien doc thanh ghi truc tiep.
 * I2C0 MPU6050: SDA8/SCL9; I2C1 MS5611: SDA6/SCL7; 100kHz.
 * Khong co nut, buzzer, GPS, mach do pin hay giao tiep IP5306.
 * De yen thiet bi khoang 4 giay khi khoi dong de lay P0.
 *
 * GIAO THUC BLE v1 (de xuat vi esp.md khong cung cap giao thuc Android):
 * Name FALLSAFE-xxxx. UUID prefix: 8d7a000N-6f4b-4c8e-9f31-52fa11a00001
 * N=1 service; N=2 telemetry READ/NOTIFY; N=3 event READ/NOTIFY;
 * N=4 command WRITE (with response); N=5 result READ/NOTIFY.
 * Bat CCCD 0x2902 cho N=2,3,5. Tat ca thong diep <=20 byte, MTU23 du.
 * Telemetry little-endian, 2 frame/100ms, cung seq uint16 tai byte 2:
 * A: [0]=0xA1,[1]=flags,[2..3]=seq,[4..7]=uptime ms,
 *    [8..13]=ax,ay,az int16 x100 m/s2,[14..19]=gx,gy,gz int16 x10 deg/s.
 * B: [0]=0xB1,[1]=FSM(0 MONITORING,1 SUSPECTED,2 VERIFYING,3 ALERT),
 *    [2..3]=seq,[4..7]=P uint32 Pa,[8..9]=T int16 x100 C,
 *    [10..13]=height int32 mm so voi P0,[14..17]=delta int32 mm so voi
 *    do cao truoc cu nga,[18..19]=eventId uint16.
 * flags: bit0 IMU fresh, bit1 baro fresh, bit2 P0 ready, bit3 BLE connected,
 *        bit4 ALERT, bit5 sampling gaps ever observed. Invalid fields = 0;
 *        app MUST inspect flags. B height/delta require bits1+2.
 * Event ASCII FALL_CONFIRMED:<id>, repeated each 1s until ACK:<id>.
 * ACK chi xac nhan nhan tin, KHONG huy ALERT. Khi reconnect gui lai ke ca
 * da ACK. App deduplicate theo device + session + id, KHONG dem nguoc lai.
 * CANCEL:<id> = huy bao gia; RESUME:<id> = ket thuc xu ly, giam sat lai.
 * Sau CANCEL/RESUME co 5s cho, ALERT khong tu mat khi nguoi dung cu dong.
 * GET BOOT => BOOT:<8 hex>; doc moi lan ket noi de nhan dien session.
 * Khong luu event qua mat dien; boot ID ngau nhien moi sau reboot.
 * Command ASCII <=20 byte, khong newline. Result OK:<command> (toi da20)
 * hoac ERR:...; WRITE response chi la transport, phai doc result.
 * SET FF 4.9 | SET IMP 25 | SET ROT 120 | SET STILL 1.0 | SET DROP 0.40
 * SET FFMS 80 | SET IDLEMS 1000. DROP la do lon duong (delta <= -DROP).
 * GET FF (hoac ten khac) => FF=4.900. DEFAULT khoi phuc nguong.
 * Chi doi profile khi MONITORING, khong trong pha roi/cooldown. Profile RAM,
 * reset ve mac dinh sau reboot. REZERO lay lai P0 khi MONITORING.
 * Retry command neu ERR:BUSY; mot command dang cho tai mot thoi diem.
 * Android phai tu dem nguoc 10s, huy, goi/SMS/GPS; firmware khong lam viec do.
 * BLE prototype chua bonding/authentication: can them truoc trien khai that.
 *
 * FSM theo esp.md: a<4.9 lien tuc >=80ms; sau do a>=25 va omega>120;
 * |a-9.81|<=1 lien tuc >=1000ms va delta h<=-0.40m => FALL_CONFIRMED.
 * Cua so impact 1500ms, verification 5000ms; rot trong 150ms truoc impact.
 * Baro bat buoc khoe de xac nhan. Loi cam bien => khong xac nhan moi,
 * giu ALERT da co, bao flags va thu khoi tao lai moi 5s.
 * Nguong nay can kiem thu tren du lieu thuc; khong bao phu moi kieu nga.
 */
#include <Arduino.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <atomic>
#include <esp_random.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// PURE_LOGIC_BEGIN -- same implementation exercised by host tests.
namespace FallSafe {
enum State : uint8_t { MONITORING, SUSPECTED, VERIFYING, ALERT };
struct Profile {
  float ff=4.9f, impact=25.0f, rotation=120.0f, still=1.0f, drop=0.40f;
  uint32_t ffMs=80, idleMs=1000;
};
struct Detector {
  Profile p;
  State state=MONITORING;
  bool low=false, quiet=false, baselineReady=false, rotated=false, cooling=false;
  uint32_t lowAt=0, impactAt=0, quietAt=0, rotationAt=0, resumedAt=0;
  uint16_t eventId=0;
  float baseline=0, before=0, delta=0;
  void resetCandidate() {
    if (state!=ALERT) state=MONITORING;
    low=quiet=rotated=false; baselineReady=false; delta=0;
  }
  bool resume(uint16_t id, uint32_t now) {
    if (state!=ALERT || id!=eventId) return false;
    state=MONITORING; resetCandidate(); cooling=true; resumedAt=now; return true;
  }
  void step(uint32_t now, float a, float w, float h, bool valid) {
    if (state==ALERT) return;
    if (!valid) { resetCandidate(); return; }
    if (cooling) {
      if (uint32_t(now-resumedAt)<5000) return;
      cooling=false;
    }
    if (!baselineReady) { baseline=before=h; baselineReady=true; }
    if (w>p.rotation) { rotationAt=now; rotated=true; }
    if (state==MONITORING) {
      if (a<p.ff) {
        if (!low) { low=true; lowAt=now; before=baseline; }
        if (uint32_t(now-lowAt)>=p.ffMs) state=SUSPECTED;
      } else {
        low=false;
        baseline += 0.02f*(h-baseline); // ~0.5s reference; freeze on first low-g
      }
    }
    delta=h-before;
    if (state==SUSPECTED) {
      if (uint32_t(now-lowAt)>1500) { resetCandidate(); return; }
      if (a>=p.impact && rotated && uint32_t(now-rotationAt)<=150) {
        state=VERIFYING; impactAt=now; quiet=false;
      }
    } else if (state==VERIFYING) {
      if (uint32_t(now-impactAt)>5000) { resetCandidate(); return; }
      if (fabsf(a-9.81f)<=p.still) {
        if (!quiet) { quiet=true; quietAt=now; }
        if (uint32_t(now-quietAt)>=p.idleMs && delta<=-p.drop) {
          state=ALERT; if (++eventId==0) ++eventId;
        }
      } else quiet=false;
    }
  }
};
uint8_t crc4(const uint16_t *prom) {
  uint16_t r=0;
  for (unsigned i=0;i<16;++i) {
    uint16_t word=prom[i/2]; if (i/2==7) word &= 0xFF00;
    r ^= (i&1) ? (word&0xFF) : (word>>8);
    for (unsigned j=0;j<8;++j) r=(r&0x8000)?uint16_t((r<<1)^0x3000):uint16_t(r<<1);
  }
  return (r>>12)&15;
}
bool compensate(const uint16_t *c, uint32_t d1, uint32_t d2, float &pa, float &tc) {
  if (!d1 || !d2 || d1>=0xFFFFFF || d2>=0xFFFFFF) return false;
  int64_t dt=int64_t(d2)-int64_t(c[5])*256;
  int64_t t=2000+dt*c[6]/8388608;
  int64_t off=int64_t(c[2])*65536+int64_t(c[4])*dt/128;
  int64_t sens=int64_t(c[1])*32768+int64_t(c[3])*dt/256;
  if (t<2000) {
    int64_t q=(t-2000)*(t-2000), off2=5*q/2, sens2=5*q/4;
    if (t<-1500) { q=(t+1500)*(t+1500); off2+=7*q; sens2+=11*q/2; }
    t-=dt*dt/2147483648LL; off-=off2; sens-=sens2;
  }
  pa=float((int64_t(d1)*sens/2097152-off)/32768); // 0.01mbar = 1Pa
  tc=float(t)/100.0f;
  return pa>=1000 && pa<=120000 && tc>=-40 && tc<=85;
}
} // namespace FallSafe
// PURE_LOGIC_END

namespace Device {
constexpr uint32_t I2C_HZ=100000, PERIOD_MS=10, BARO_STALE_MS=150;
constexpr char SERVICE[]="8d7a0001-6f4b-4c8e-9f31-52fa11a00001";
constexpr char TELEMETRY[]="8d7a0002-6f4b-4c8e-9f31-52fa11a00001";
constexpr char EVENT[]="8d7a0003-6f4b-4c8e-9f31-52fa11a00001";
constexpr char COMMAND[]="8d7a0004-6f4b-4c8e-9f31-52fa11a00001";
constexpr char RESULT[]="8d7a0005-6f4b-4c8e-9f31-52fa11a00001";
struct Motion { float ax=0,ay=0,az=0,gx=0,gy=0,gz=0,a=0,w=0; };
struct Snapshot {
  Motion m; float pressure=0,temp=0,height=0,delta=0;
  uint32_t ms=0,gaps=0; uint16_t id=0; uint8_t state=0,flags=0; bool acked=false;
};
struct Message { char text[21]; };
uint32_t bootId=0; // set once before sensor task starts
QueueHandle_t snapshots=nullptr, commands=nullptr, results=nullptr;
std::atomic<bool> connected(false), restartAdvertising(false), resetEventAck(false);
BLECharacteristic *telemetry=nullptr,*events=nullptr,*result=nullptr;
BLE2902 *telemetryCCCD=nullptr,*eventCCCD=nullptr,*resultCCCD=nullptr;

bool readBytes(TwoWire &bus, uint8_t addr, uint8_t reg, uint8_t *out, size_t n) {
  bus.beginTransmission(addr); bus.write(reg);
  if (bus.endTransmission(false)!=0) return false;
  size_t received=bus.requestFrom(addr,n,true);
  if (received!=n) { while(bus.available()) bus.read(); return false; }
  for (size_t i=0;i<n;++i) out[i]=bus.read();
  return true;
}
bool writeReg(TwoWire &bus,uint8_t addr,uint8_t reg,uint8_t value) {
  bus.beginTransmission(addr); bus.write(reg); bus.write(value); return bus.endTransmission()==0;
}
bool command(TwoWire &bus,uint8_t addr,uint8_t cmd) {
  bus.beginTransmission(addr); bus.write(cmd); return bus.endTransmission()==0;
}
struct MPU {
  uint8_t addr=0; bool ready=false;
  bool begin() {
    ready=false;
    for (uint8_t candidate : {uint8_t(0x68),uint8_t(0x69)}) {
      uint8_t who=0;
      if (!readBytes(Wire,candidate,0x75,&who,1) || who!=0x68) continue;
      addr=candidate;
      if (!writeReg(Wire,addr,0x6B,0x80)) continue;
      vTaskDelay(pdMS_TO_TICKS(100));
      // PLL X gyro; all axes on; DLPF ~44Hz; 1kHz/(9+1)=100Hz;
      // gyro +/-2000deg/s (16.4LSB/deg/s), accel +/-16g (2048LSB/g).
      if (!writeReg(Wire,addr,0x6B,0x01) || !writeReg(Wire,addr,0x6C,0x00) ||
          !writeReg(Wire,addr,0x1A,0x03) || !writeReg(Wire,addr,0x19,9) ||
          !writeReg(Wire,addr,0x1B,0x18) || !writeReg(Wire,addr,0x1C,0x18) ||
          !writeReg(Wire,addr,0x38,0x01)) continue;
      uint8_t cfg[4];
      if (!readBytes(Wire,addr,0x19,cfg,4) || cfg[0]!=9 || cfg[1]!=3 || cfg[2]!=0x18 || cfg[3]!=0x18) continue;
      ready=true; break;
    }
    Serial.printf("MPU6050: %s (0x%02X)\n",ready?"WHO_AM_I/config OK":"FAILED",addr);
    return ready;
  }
  // 1=new sample, 0=no new sample, -1=I2C failure. Never reuse stale sample.
  int read(Motion &m) {
    uint8_t b[15];
    if (!readBytes(Wire,addr,0x3A,b,sizeof(b))) return -1;
    if (!(b[0]&1)) return 0;
    auto raw=[&](int i)->int16_t { return int16_t((uint16_t(b[i])<<8)|b[i+1]); };
    m.ax=raw(1)*(9.81f/2048); m.ay=raw(3)*(9.81f/2048); m.az=raw(5)*(9.81f/2048);
    m.gx=raw(9)/16.4f; m.gy=raw(11)/16.4f; m.gz=raw(13)/16.4f;
    m.a=sqrtf(m.ax*m.ax+m.ay*m.ay+m.az*m.az);
    m.w=sqrtf(m.gx*m.gx+m.gy*m.gy+m.gz*m.gz);
    return 1;
  }
};
struct Barometer {
  uint8_t addr=0; uint16_t c[8]={}; bool ready=false,phaseD2=true,haveSample=false,zeroReady=false;
  uint32_t startedUs=0,lastMs=0,warmupAt=0,d2=0;
  uint16_t zeroCount=0; double zeroSum=0;
  float pressure=0,temp=0,p0=0,filtered=0,height=0;
  void rezero() { zeroReady=false; zeroCount=0; zeroSum=0; height=0; warmupAt=millis(); }
  // MS5611 uses command -> STOP -> read, independently of MPU repeated START.
  bool send(uint8_t cmd) {
    Wire1.beginTransmission(addr); Wire1.write(cmd);
    uint8_t error=Wire1.endTransmission(true);
    if (error) Serial.printf("[MS5611] addr=0x%02X cmd=0x%02X I2C_error=%u\n",addr,cmd,error);
    return error==0;
  }
  bool readData(uint8_t cmd,uint8_t *out,size_t n) {
    if (!send(cmd)) return false;
    size_t received=Wire1.requestFrom(addr,n,true);
    if (received!=n) {
      Serial.printf("[MS5611] addr=0x%02X cmd=0x%02X SHORT_READ got=%u need=%u\n",
                    addr,cmd,unsigned(received),unsigned(n));
      while (Wire1.available()) Wire1.read();
      return false;
    }
    for (size_t i=0;i<n;++i) out[i]=Wire1.read();
    return true;
  }
  bool start(uint8_t cmd) {
    if (!send(cmd)) { ready=false; return false; }
    startedUs=micros(); return true;
  }
  bool begin() {
    ready=false; haveSample=false; rezero();
    Serial.println("[MS5611] INIT bus1 SDA=6 SCL=7 100kHz, command STOP/read");
    for (uint8_t candidate : {uint8_t(0x77),uint8_t(0x76)}) {
      addr=candidate;
      Wire1.beginTransmission(addr);
      uint8_t ack=Wire1.endTransmission(true);
      Serial.printf("[MS5611] PROBE 0x%02X ACK=%s code=%u\n",addr,ack==0?"YES":"NO",ack);
      if (ack) continue;
      if (!send(0x1E)) { Serial.println("[MS5611] RESET_FAILED"); continue; }
      // PROM reload requires >=2.8ms; allow margin for RTOS tick boundaries.
      vTaskDelay(pdMS_TO_TICKS(10));
      bool ok=true;
      for (int i=0;i<8;++i) {
        uint8_t b[2];
        if (!readData(0xA0+2*i,b,2)) { ok=false; break; }
        c[i]=(uint16_t(b[0])<<8)|b[1];
        Serial.printf("[MS5611] PROM[%d]=0x%04X (%u)\n",i,c[i],c[i]);
        if (i>=1 && i<=6 && (c[i]==0 || c[i]==0xFFFF)) {
          Serial.printf("[MS5611] INVALID_COEFFICIENT C%d\n",i); ok=false;
        }
      }
      if (!ok) { Serial.println("[MS5611] PROM_INVALID_OR_UNREADABLE"); continue; }
      uint8_t expected=c[7]&15,actual=FallSafe::crc4(c);
      Serial.printf("[MS5611] CRC stored=%u calculated=%u\n",expected,actual);
      if (actual!=expected) { Serial.println("[MS5611] CRC_MISMATCH (not bypassed)"); continue; }
      phaseD2=true;
      if (!start(0x58)) { Serial.println("[MS5611] CONVERSION_START_FAILED"); continue; }
      ready=true;
      Serial.printf("MS5611: PROM CRC OK (0x%02X)\n",addr);
      return true;
    }
    addr=0; // Do not report the last probed address as the detected device.
    Serial.println("MS5611: INIT FAILED; see PROBE/PROM/CRC above; retry in 5s");
    return false;
  }
  void poll() {
    if (!ready || uint32_t(micros()-startedUs)<10000) return; // max conversion 9.04ms
    uint8_t b[3];
    if (!readData(0x00,b,3)) { ready=false; return; }
    uint32_t adc=(uint32_t(b[0])<<16)|(uint32_t(b[1])<<8)|b[2];
    if (!adc || adc==0xFFFFFF) {
      Serial.printf("[MS5611] INVALID_ADC %s=0x%06lX\n",phaseD2?"D2":"D1",(unsigned long)adc);
      ready=false; return;
    }
    if (phaseD2) { d2=adc; phaseD2=false; start(0x48); return; }
    if (!FallSafe::compensate(c,adc,d2,pressure,temp)) {
      Serial.printf("[MS5611] RANGE_ERROR D1=%lu D2=%lu P=%.2f T=%.2f\n",
        (unsigned long)adc,(unsigned long)d2,pressure,temp);
      ready=false; return;
    }
    lastMs=millis();
    if (!haveSample) filtered=pressure;
    else filtered+=0.20f*(pressure-filtered);
    haveSample=true;
    if (!zeroReady && uint32_t(lastMs-warmupAt)>=1000) {
      zeroSum+=pressure;
      if (++zeroCount>=100) {
        p0=float(zeroSum/zeroCount); zeroReady=true;
        Serial.printf("P0 READY: %.2f Pa\n",p0);
      }
    }
    if (zeroReady) height=44330.0f*(1.0f-powf(filtered/p0,0.19029495f));
    phaseD2=true; start(0x58);
  }
  bool fresh(uint32_t now) const { return ready && haveSample && uint32_t(now-lastMs)<=BARO_STALE_MS; }
};

void reply(const char *s) {
  Message m={}; snprintf(m.text,sizeof(m.text),"%s",s);
  xQueueSend(results,&m,0); // queue depth > command depth; loop drains before next send
}
bool parseNumber(const char *s, float &v) {
  char *end=nullptr; v=strtof(s,&end);
  return end!=s && *end=='\0' && isfinite(v);
}
void handleCommand(const char *text, FallSafe::Detector &d, Barometer &bar, bool &acked) {
  if (!strncmp(text,"ACK:",4) || !strncmp(text,"CANCEL:",7) || !strncmp(text,"RESUME:",7)) {
    bool ack=!strncmp(text,"ACK:",4); const char *s=text+(ack?4:7);
    if (!*s) { reply("ERR:ID"); return; }
    uint32_t id=0;
    for (;*s;++s) {
      if (*s<'0'||*s>'9') { reply("ERR:ID"); return; }
      id=id*10+(*s-'0'); if (id>65535) { reply("ERR:ID"); return; }
    }
    if (d.state!=FallSafe::ALERT || id!=d.eventId) { reply("ERR:ID"); return; }
    if (ack) acked=true; else { d.resume(uint16_t(id),millis()); acked=false; }
    reply(ack?"OK:ACK":"OK:RESUMED"); return;
  }
  if (!strcmp(text,"GET BOOT")) {
    char response[21]; snprintf(response,sizeof(response),"BOOT:%08lX",(unsigned long)bootId);
    reply(response); return;
  }
  char key[9]={},number[16]={},extra=0; bool get=!strncmp(text,"GET ",4);
  if (get || !strncmp(text,"SET ",4)) {
    int count=sscanf(text+4,"%8s %15s %c",key,number,&extra);
    if ((get && count!=1)||(!get && count!=2)) { reply("ERR:SYNTAX"); return; }
    float *target=nullptr,min=0,max=0; uint32_t *timeTarget=nullptr;
    if (!strcmp(key,"FF")) { target=&d.p.ff; min=1; max=8; }
    else if (!strcmp(key,"IMP")) { target=&d.p.impact; min=15; max=100; }
    else if (!strcmp(key,"ROT")) { target=&d.p.rotation; min=30; max=1000; }
    else if (!strcmp(key,"STILL")) { target=&d.p.still; min=0.2f; max=3; }
    else if (!strcmp(key,"DROP")) { target=&d.p.drop; min=0.1f; max=2; }
    else if (!strcmp(key,"FFMS")) { timeTarget=&d.p.ffMs; min=20; max=500; }
    else if (!strcmp(key,"IDLEMS")) { timeTarget=&d.p.idleMs; min=300; max=3000; }
    else { reply("ERR:KEY"); return; }
    char response[21];
    if (get) { snprintf(response,sizeof(response),"%s=%.3f",key,target?*target:float(*timeTarget)); reply(response); return; }
    if (d.state!=FallSafe::MONITORING || d.low || d.cooling) { reply("ERR:STATE"); return; }
    float v;
    if (!parseNumber(number,v)||v<min||v>max||(timeTarget && floorf(v)!=v)) { reply("ERR:RANGE"); return; }
    if (target) *target=v; else *timeTarget=uint32_t(v);
    d.resetCandidate(); snprintf(response,sizeof(response),"OK:SET %s",key); reply(response); return;
  }
  if (!strcmp(text,"DEFAULT") || !strcmp(text,"REZERO")) {
    if (d.state!=FallSafe::MONITORING || d.low || d.cooling) { reply("ERR:STATE"); return; }
    if (!strcmp(text,"DEFAULT")) d.p=FallSafe::Profile{};
    else bar.rezero();
    d.resetCandidate(); reply("OK"); return;
  }
  reply("ERR:COMMAND");
}

void sensorTask(void *) {
  MPU imu; Barometer bar; Motion motion; FallSafe::Detector detector;
  bool bus0=Wire.begin(8,9,I2C_HZ),bus1=Wire1.begin(6,7,I2C_HZ);
  Wire.setTimeOut(5); Wire1.setTimeOut(5);
  if (bus0) imu.begin(); if (bus1) bar.begin();
  TickType_t wake=xTaskGetTickCount();
  uint32_t retryAt=millis(),lastImuMs=millis(),lastStepMs=0,gaps=0;
  bool haveImu=false,acked=false; uint16_t previousEvent=0;
  for (;;) {
    uint32_t now=millis();
    Message cmd;
    if (resetEventAck.exchange(false)) acked=false;
    // At most one config per 10ms cycle, ownership stays entirely in this task.
    if (xQueueReceive(commands,&cmd,0)==pdTRUE) handleCommand(cmd.text,detector,bar,acked);
    if (uint32_t(now-retryAt)>=5000 && (!imu.ready || !bar.ready)) {
      // Mark discontinuity before any slow recovery; stale telemetry is invalid.
      detector.resetCandidate(); haveImu=false; lastStepMs=0; ++gaps;
      if (!imu.ready) {
        Wire.end(); bus0=Wire.begin(8,9,I2C_HZ); Wire.setTimeOut(5);
        if (bus0) imu.begin();
      }
      if (!bar.ready) {
        Wire1.end(); bus1=Wire1.begin(6,7,I2C_HZ); Wire1.setTimeOut(5);
        if (bus1) bar.begin();
      }
      retryAt=millis(); now=retryAt; lastImuMs=now; wake=xTaskGetTickCount();
    }
    int sample=imu.ready?imu.read(motion):-1;
    now=millis();
    if (sample<0) { imu.ready=false; haveImu=false; detector.resetCandidate(); }
    if (sample==1) { lastImuMs=now; haveImu=true; }
    if (imu.ready && uint32_t(now-lastImuMs)>30) { imu.ready=false; haveImu=false; }
    bar.poll(); now=millis();
    bool barGood=bar.fresh(now),imuGood=imu.ready && haveImu && uint32_t(now-lastImuMs)<=30;
    bool valid=barGood && imuGood && bar.zeroReady;
    if (!valid) detector.resetCandidate();
    if (sample==1) {
      if (lastStepMs && uint32_t(now-lastStepMs)>25) { ++gaps; detector.resetCandidate(); }
      lastStepMs=now;
      detector.step(now,motion.a,motion.w,bar.height,valid);
    }
    if (detector.eventId!=previousEvent) { previousEvent=detector.eventId; acked=false; }
    Snapshot s; s.m=motion; s.ms=now; s.gaps=gaps;
    s.pressure=bar.pressure; s.temp=bar.temp; s.height=bar.height; s.delta=detector.delta;
    s.id=detector.eventId; s.state=detector.state; s.acked=acked;
    s.flags=(imuGood?1:0)|(barGood?2:0)|(bar.zeroReady?4:0)|
            (connected.load()?8:0)|(detector.state==FallSafe::ALERT?16:0)|(gaps?32:0);
    xQueueOverwrite(snapshots,&s);
    // Do not run burst catch-up iterations after a missed deadline.
    if (xTaskGetTickCount()-wake>=pdMS_TO_TICKS(PERIOD_MS)) wake=xTaskGetTickCount();
    vTaskDelayUntil(&wake,pdMS_TO_TICKS(PERIOD_MS));
  }
}
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *) override { resetEventAck.store(true); connected.store(true); }
  void onDisconnect(BLEServer *) override { connected.store(false); restartAdvertising.store(true); }
};
class CommandCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    auto value=characteristic->getValue();
    Message msg={};
    if (!value.length() || value.length()>20) { reply("ERR:LENGTH"); return; }
    for (size_t i=0;i<value.length();++i) {
      if (value[i]<32 || value[i]>126) { reply("ERR:ASCII"); return; }
      msg.text[i]=value[i];
    }
    if (xQueueSend(commands,&msg,0)!=pdTRUE) reply("ERR:BUSY");
  }
};
void beginBLE() {
  char name[20]; snprintf(name,sizeof(name),"FALLSAFE-%04X",unsigned((ESP.getEfuseMac()>>32)&0xFFFF));
  BLEDevice::init(name); bootId=esp_random();
  BLEServer *server=BLEDevice::createServer(); server->setCallbacks(new ServerCallbacks());
  BLEService *service=server->createService(BLEUUID(SERVICE),24);
  telemetry=service->createCharacteristic(TELEMETRY,BLECharacteristic::PROPERTY_READ|BLECharacteristic::PROPERTY_NOTIFY);
  events=service->createCharacteristic(EVENT,BLECharacteristic::PROPERTY_READ|BLECharacteristic::PROPERTY_NOTIFY);
  result=service->createCharacteristic(RESULT,BLECharacteristic::PROPERTY_READ|BLECharacteristic::PROPERTY_NOTIFY);
  auto *cmd=service->createCharacteristic(COMMAND,BLECharacteristic::PROPERTY_WRITE);
  cmd->setCallbacks(new CommandCallbacks());
  telemetryCCCD=new BLE2902(); eventCCCD=new BLE2902(); resultCCCD=new BLE2902();
  telemetry->addDescriptor(telemetryCCCD); events->addDescriptor(eventCCCD); result->addDescriptor(resultCCCD);
  events->setValue("READY"); result->setValue("PROTOCOL:1");
  service->start();
  auto *adv=BLEDevice::getAdvertising(); adv->addServiceUUID(SERVICE); adv->setScanResponse(true);
  BLEDevice::startAdvertising(); Serial.printf("BLE: %s\n",name);
}
void put16(uint8_t *b,uint16_t n) { b[0]=n; b[1]=n>>8; }
void put32(uint8_t *b,uint32_t n) { for (int i=0;i<4;++i) b[i]=n>>(8*i); }
int16_t scale16(float f,float scale) { return int16_t(lroundf(fmaxf(-32768,fminf(32767,f*scale)))); }
void publishTelemetry(const Snapshot &s,uint16_t seq) {
  uint8_t b[20]={}; b[0]=0xA1; b[1]=s.flags; put16(b+2,seq); put32(b+4,s.ms);
  float v[6]={s.m.ax,s.m.ay,s.m.az,s.m.gx,s.m.gy,s.m.gz};
  for (int i=0;i<6;++i) put16(b+8+2*i,(s.flags&1)?uint16_t(scale16(v[i],i<3?100:10)):0);
  telemetry->setValue(b,20); if (connected.load() && telemetryCCCD->getNotifications()) telemetry->notify();
  memset(b,0,20); b[0]=0xB1; b[1]=s.state; put16(b+2,seq);
  if (s.flags&2) { put32(b+4,uint32_t(lroundf(s.pressure))); put16(b+8,uint16_t(scale16(s.temp,100))); }
  if ((s.flags&6)==6) { put32(b+10,uint32_t(int32_t(lroundf(s.height*1000)))); put32(b+14,uint32_t(int32_t(lroundf(s.delta*1000)))); }
  put16(b+18,s.id); telemetry->setValue(b,20);
  if (connected.load() && telemetryCCCD->getNotifications()) telemetry->notify();
}
} // namespace Device

void setup() {
  Serial.begin(115200);
  uint32_t start=millis(); while (!Serial && uint32_t(millis()-start)<3000) delay(10);
  Serial.println("\nFallSafe v7.1: raw MPU6050 + MS5611 / dual I2C / BLE / MS5611 diagnostics");
  Device::snapshots=xQueueCreate(1,sizeof(Device::Snapshot));
  Device::commands=xQueueCreate(4,sizeof(Device::Message));
  Device::results=xQueueCreate(16,sizeof(Device::Message));
  if (!Device::snapshots || !Device::commands || !Device::results) {
    Serial.println("FATAL: queue allocation"); while(true) delay(1000);
  }
  Device::beginBLE();
  if (xTaskCreatePinnedToCore(Device::sensorTask,"sensors",6144,nullptr,3,nullptr,1)!=pdPASS) {
    Device::events->setValue("ERROR:SENSOR_TASK");
    Serial.println("FATAL: sensor task"); while(true) delay(1000);
  }
}
void loop() {
  using namespace Device;
  static Snapshot s; static uint32_t telemetryAt=0,eventAt=0,logAt=0;
  static uint16_t seq=0,lastId=0; static bool wasConnected=false,wasAlert=false;
  static bool repeatAfterConnect=false;
  bool link=connected.load();
  if (restartAdvertising.exchange(false)) {
    // Clear stale CCCD from unbonded prior connection; Android must subscribe again.
    telemetryCCCD->setNotifications(false); eventCCCD->setNotifications(false); resultCCCD->setNotifications(false);
    BLEDevice::startAdvertising(); wasConnected=false;
  }
  if (link && !wasConnected) { repeatAfterConnect=true; eventAt=millis()-1000; }
  wasConnected=link;
  xQueuePeek(snapshots,&s,0);
  uint32_t now=millis();
  bool alert=s.state==FallSafe::ALERT;
  if (alert) {
    char msg[21]; snprintf(msg,sizeof(msg),"FALL_CONFIRMED:%u",s.id);
    events->setValue(msg);
    bool newEvent=!wasAlert || lastId!=s.id;
    if (newEvent) Serial.println(msg);
    if (link && eventCCCD->getNotifications() &&
        (newEvent || ((!s.acked || repeatAfterConnect) && uint32_t(now-eventAt)>=1000))) {
      events->notify(); eventAt=now; repeatAfterConnect=false;
    }
  } else {
    events->setValue((s.flags&7)==7?"READY":"SENSOR_NOT_READY");
    if (wasAlert && link && eventCCCD->getNotifications()) events->notify();
  }
  wasAlert=alert; lastId=s.id;
  Message response;
  while (xQueueReceive(results,&response,0)==pdTRUE) {
    result->setValue(response.text);
    if (link && resultCCCD->getNotifications()) result->notify();
  }
  if (uint32_t(now-telemetryAt)>=100) { telemetryAt=now; publishTelemetry(s,++seq); }
  if (uint32_t(now-logAt)>=1000) {
    logAt=now;
    Serial.printf("state=%u flags=0x%02X a=%.2f w=%.1f P=%.0f T=%.2f h=%.2f dh=%.2f gaps=%lu BLE=%u\n",
      s.state,s.flags,s.m.a,s.m.w,s.pressure,s.temp,s.height,s.delta,(unsigned long)s.gaps,unsigned(link));
  }
  delay(2);
}
