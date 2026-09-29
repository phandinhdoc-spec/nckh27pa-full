#include "../fall_detector.h"

#include <assert.h>

static void stepRange(FallDetector &d, uint32_t first, uint32_t last,
                      float acceleration, float rotation, float height,
                      bool valid = true) {
  for (uint32_t t = first; t <= last; t += 10)
    d.step(t, acceleration, rotation, height, valid);
}

int main() {
  FallDetector fall;
  stepRange(fall, 10, 100, 9.81f, 0, 0);
  stepRange(fall, 110, 190, 2.0f, 0, 0);
  assert(fall.state == FallState::SUSPECTED);
  fall.step(200, 30.0f, 150.0f, -0.2f, true);
  assert(fall.state == FallState::VERIFYING);
  stepRange(fall, 210, 1200, 9.81f, 0, -0.55f);
  assert(fall.state == FallState::VERIFYING);
  assert(fall.step(1210, 9.81f, 0, -0.55f, true));
  assert(fall.state == FallState::ALERT && fall.eventId == 1);
  assert(!fall.step(1220, 9.81f, 0, -0.55f, true));

  FallDetector shortFall;
  stepRange(shortFall, 10, 70, 2.0f, 0, 0);
  shortFall.step(80, 30.0f, 150.0f, -0.6f, true);
  assert(shortFall.state == FallState::MONITORING);

  FallDetector noRotation;
  stepRange(noRotation, 10, 100, 2.0f, 0, 0);
  noRotation.step(110, 30.0f, 0, -0.6f, true);
  assert(noRotation.state == FallState::SUSPECTED);

  FallDetector noDrop;
  stepRange(noDrop, 10, 100, 2.0f, 0, 0);
  noDrop.step(110, 30.0f, 150.0f, 0, true);
  stepRange(noDrop, 120, 1200, 9.81f, 0, -0.2f);
  assert(noDrop.state == FallState::VERIFYING);

  FallDetector gap;
  FallProfile strictGap;
  strictGap.maximumSampleGapMs = 25;
  gap.applyProfile(strictGap);
  stepRange(gap, 10, 100, 2.0f, 0, 0);
  gap.step(140, 30.0f, 150.0f, -0.6f, true);
  assert(gap.state == FallState::MONITORING);

  FallDetector missingBaro;
  stepRange(missingBaro, 10, 100, 2.0f, 0, 0);
  missingBaro.step(110, 30.0f, 150.0f, -0.6f, false);
  assert(missingBaro.state == FallState::MONITORING);

  FallProfile custom;
  custom.impactAccelerationMs2 = 40.0f;
  assert(custom.valid());
  FallDetector tuned;
  tuned.applyProfile(custom);
  stepRange(tuned, 10, 100, 2.0f, 0, 0);
  tuned.step(110, 30.0f, 150.0f, -0.6f, true);
  assert(tuned.state == FallState::SUSPECTED);
  tuned.step(120, 45.0f, 150.0f, -0.6f, true);
  assert(tuned.state == FallState::VERIFYING);

  FallDetector pressureFall;
  for (uint32_t t = 10; t <= 100; t += 10)
    pressureFall.step(t, 2.0f, 0, 0, true, 100000.0f);
  pressureFall.step(110, 30.0f, 150.0f, 0, true, 100000.0f);
  bool pressureConfirmed = false;
  for (uint32_t t = 120; t <= 1120; t += 10)
    pressureConfirmed |= pressureFall.step(t, 9.81f, 0, 0, true, 100015.0f);
  assert(pressureConfirmed && pressureFall.state == FallState::ALERT);

  FallProfile invalid = custom;
  invalid.postImpactStillnessDurationMs = invalid.postImpactWindowMs + 1;
  assert(!invalid.valid());
}
