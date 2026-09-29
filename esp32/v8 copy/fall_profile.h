#pragma once

#include <math.h>
#include <stdint.h>

struct FallProfile {
  float impactAccelerationMs2 = 25.0f;
  float stillnessTargetAccelerationMs2 = 9.81f;
  float stillnessToleranceMs2 = 1.0f;
  uint32_t postImpactWindowMs = 3000;
  uint32_t postImpactStillnessDurationMs = 1000;
  uint16_t minimumStillnessSamples = 6;
  uint32_t maximumSampleGapMs = 250;
  float freeFallThresholdMs2 = 4.9f;
  uint32_t freeFallMinDurationMs = 80;
  float gyroTurnThresholdDps = 120.0f;
  float pressureEvidenceMinRisePa = 12.0f;
  uint32_t pressureWindowMs = 5000;
  float altitudeDropMinM = -0.40f;
  uint32_t sampleWatchdogMs = 100;

  bool valid() const {
    return isfinite(impactAccelerationMs2) && impactAccelerationMs2 >= 1 && impactAccelerationMs2 <= 100 &&
           isfinite(stillnessTargetAccelerationMs2) && stillnessTargetAccelerationMs2 >= 0 && stillnessTargetAccelerationMs2 <= 20 &&
           isfinite(stillnessToleranceMs2) && stillnessToleranceMs2 >= 0.1f && stillnessToleranceMs2 <= 10 &&
           postImpactWindowMs >= 500 && postImpactWindowMs <= 10000 &&
           postImpactStillnessDurationMs >= 100 && postImpactStillnessDurationMs <= postImpactWindowMs &&
           minimumStillnessSamples >= 2 && minimumStillnessSamples <= 100 &&
           maximumSampleGapMs >= 10 && maximumSampleGapMs <= 2000 &&
           isfinite(freeFallThresholdMs2) && freeFallThresholdMs2 >= 0.1f && freeFallThresholdMs2 <= 20 &&
           freeFallMinDurationMs >= 20 && freeFallMinDurationMs <= 1000 &&
           isfinite(gyroTurnThresholdDps) && gyroTurnThresholdDps >= 1 && gyroTurnThresholdDps <= 2000 &&
           isfinite(pressureEvidenceMinRisePa) && pressureEvidenceMinRisePa >= 1 && pressureEvidenceMinRisePa <= 1000 &&
           pressureWindowMs >= 100 && pressureWindowMs <= 10000 &&
           isfinite(altitudeDropMinM) && altitudeDropMinM >= -10 && altitudeDropMinM <= -0.01f &&
           sampleWatchdogMs >= 20 && sampleWatchdogMs <= 2000;
  }
};
