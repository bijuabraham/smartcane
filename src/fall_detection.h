#ifndef FALL_DETECTION_H
#define FALL_DETECTION_H

#include <Arduino.h>
#include "sensors.h"

// ===================================================================
// Fall Detection State
// ===================================================================

enum FallState {
  FALL_IDLE,
  FALL_POTENTIAL,
  FALL_CONFIRMED
};

// ===================================================================
// Fall Detection Functions
// ===================================================================

void fall_detection_init();
void fall_detection_update(const IMUData& imu);
bool fall_detection_check(float& ax, float& ay, float& az);
void fall_detection_reset();

#endif // FALL_DETECTION_H
