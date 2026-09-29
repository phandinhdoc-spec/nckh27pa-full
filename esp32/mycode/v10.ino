#include <Arduino.h>
#include <Wire.h>
#include <math.h>

TwoWire &busMPU = Wire;
TwoWire &busGY = Wire1;

constexpr uint8_t DIA_CHI_MPU = 0x68;
constexpr unit8_t DIA_CHI_GY = 0x77;