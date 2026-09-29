#pragma once

#include <math.h>
#include <stdint.h>
#include "fall_profile.h"

// Phan quyet thuan C++: moi lan goi la mot mau MPU cach nhau khoang 10 ms.
enum class FallState : uint8_t { MONITORING, SUSPECTED, VERIFYING, ALERT };

struct FallDetector {
  FallProfile profile;
  FallState state = FallState::MONITORING;
  uint16_t eventId = 0;
  uint32_t sampleGaps = 0;
  uint32_t lastSampleMs = 0;
  uint32_t lowAtMs = 0;
  uint32_t rotationAtMs = 0;
  uint32_t impactAtMs = 0;
  uint32_t stillAtMs = 0;
  float referenceHeightM = 0;
  float beforeFallHeightM = 0;
  float dropM = 0;
  float beforeFallPressurePa = NAN;
  uint16_t stillnessSamples = 0;
  bool haveSample = false;
  bool haveReference = false;
  bool lowActive = false;
  bool rotationSeen = false;
  bool stillActive = false;

  void resetCandidate() {
    if (state != FallState::ALERT) state = FallState::MONITORING;
    haveReference = false;
    lowActive = rotationSeen = stillActive = false;
    dropM = 0;
    beforeFallPressurePa = NAN;
    stillnessSamples = 0;
  }

  void applyProfile(const FallProfile &updated) {
    profile = updated;
    resetCandidate();
    haveSample = false;
  }

  // Tra ve true dung mot lan khi vua xac nhan nga.
  bool step(uint32_t nowMs, float accelerationMs2, float rotationDps,
            float heightM, bool sensorsValid, float pressurePa = NAN) {
    if (state == FallState::ALERT) return false;
    if (!sensorsValid || !isfinite(accelerationMs2) || !isfinite(rotationDps) ||
        !isfinite(heightM)) {
      // Một lần đọc hụt không xóa bằng chứng rơi/va đập. Chỉ hủy khi mất mẫu
      // lâu hơn giới hạn của bộ thông số; lastSampleMs là mốc mẫu hợp lệ gần nhất.
      if (haveSample && uint32_t(nowMs - lastSampleMs) > profile.maximumSampleGapMs) {
        ++sampleGaps;
        resetCandidate();
        haveSample = false;
      }
      return false;
    }
    if (haveSample && uint32_t(nowMs - lastSampleMs) > profile.maximumSampleGapMs) {
      ++sampleGaps;
      resetCandidate();
    }
    lastSampleMs = nowMs;
    haveSample = true;

    if (!haveReference) {
      referenceHeightM = heightM;
      haveReference = true;
    }
    if (rotationDps > profile.gyroTurnThresholdDps) {
      rotationSeen = true;
      rotationAtMs = nowMs;
    }

    if (state == FallState::MONITORING) {
      if (accelerationMs2 < profile.freeFallThresholdMs2) {
        if (!lowActive) {
          lowActive = true;
          lowAtMs = nowMs;
          beforeFallHeightM = referenceHeightM;
          beforeFallPressurePa = pressurePa;
        }
        if (uint32_t(nowMs - lowAtMs) >= profile.freeFallMinDurationMs) state = FallState::SUSPECTED;
      } else {
        lowActive = false;
        referenceHeightM += 0.02f * (heightM - referenceHeightM);
      }
    }

    if (state == FallState::SUSPECTED) {
      if (uint32_t(nowMs - lowAtMs) > 1500) {
        resetCandidate();
        return false;
      }
      if (accelerationMs2 >= profile.impactAccelerationMs2 && rotationSeen &&
          uint32_t(nowMs - rotationAtMs) <= 150) {
        state = FallState::VERIFYING;
        impactAtMs = nowMs;
        stillActive = false;
        stillnessSamples = 0;
      }
    } else if (state == FallState::VERIFYING) {
      if (uint32_t(nowMs - impactAtMs) > profile.postImpactWindowMs) {
        resetCandidate();
        return false;
      }
      dropM = heightM - beforeFallHeightM;
      if (fabsf(accelerationMs2 - profile.stillnessTargetAccelerationMs2) <= profile.stillnessToleranceMs2) {
        if (!stillActive) {
          stillActive = true;
          stillAtMs = nowMs;
          stillnessSamples = 0;
        }
        if (stillnessSamples < UINT16_MAX) ++stillnessSamples;
        bool pressureEvidence = isfinite(pressurePa) && isfinite(beforeFallPressurePa) &&
                                uint32_t(nowMs - lowAtMs) <= profile.pressureWindowMs &&
                                pressurePa - beforeFallPressurePa >= profile.pressureEvidenceMinRisePa;
        if (uint32_t(nowMs - stillAtMs) >= profile.postImpactStillnessDurationMs &&
            stillnessSamples >= profile.minimumStillnessSamples &&
            (dropM <= profile.altitudeDropMinM || pressureEvidence)) {
          state = FallState::ALERT;
          if (++eventId == 0) ++eventId;
          return true;
        }
      } else {
        stillActive = false;
        stillnessSamples = 0;
      }
    }
    return false;
  }
};
