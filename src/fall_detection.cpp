#include "fall_detection.h"
#include "config.h"

// ===================================================================
// Fall Detection State Variables
// ===================================================================

static FallState fall_state = FALL_IDLE;
static unsigned long potential_fall_time = 0;
static float fall_ax = 0, fall_ay = 0, fall_az = 0;

// ===================================================================
// Fall Detection Initialization
// ===================================================================

void fall_detection_init() {
  fall_state = FALL_IDLE;
  potential_fall_time = 0;
  Serial.println("Fall Detection: Initialized");
}

// ===================================================================
// Fall Detection Update
// ===================================================================

void fall_detection_update(const IMUData& imu) {
  if (!imu.valid) return;
  
  float accel_magnitude = sqrt(imu.ax * imu.ax + imu.ay * imu.ay + imu.az * imu.az);
  
  switch (fall_state) {
    case FALL_IDLE:
      if (accel_magnitude > (1.0 + g_config.fall_ax_threshold)) {
        fall_state = FALL_POTENTIAL;
        potential_fall_time = millis();
        fall_ax = imu.ax;
        fall_ay = imu.ay;
        fall_az = imu.az;
        Serial.println("Fall Detection: Potential fall detected (acceleration spike)");
      }
      break;
      
    case FALL_POTENTIAL:
      if (accel_magnitude < FALL_MOTION_THRESHOLD) {
        if (millis() - potential_fall_time >= FALL_STILLNESS_DURATION_MS) {
          fall_state = FALL_CONFIRMED;
          Serial.println("Fall Detection: FALL CONFIRMED!");
        }
      } else {
        if (millis() - potential_fall_time > 2000) {
          Serial.println("Fall Detection: False alarm, resetting");
          fall_state = FALL_IDLE;
        }
      }
      break;
      
    case FALL_CONFIRMED:
      break;
  }
}

// ===================================================================
// Check if Fall Detected
// ===================================================================

bool fall_detection_check(float& ax, float& ay, float& az) {
  if (fall_state == FALL_CONFIRMED) {
    ax = fall_ax;
    ay = fall_ay;
    az = fall_az;
    return true;
  }
  return false;
}

// ===================================================================
// Reset Fall Detection
// ===================================================================

void fall_detection_reset() {
  fall_state = FALL_IDLE;
  potential_fall_time = 0;
}
