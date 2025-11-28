#include "fall_detection.h"
#include "config.h"

// ===================================================================
// Fall Detection State Variables
// ===================================================================

static FallState fall_state = FALL_IDLE;
static unsigned long potential_fall_time = 0;
static float fall_ax = 0, fall_ay = 0, fall_az = 0;

// ===================================================================
// Calibration State Variables
// ===================================================================

static CalibrationData calibration = {
  false,  // active
  false,  // complete
  0,      // start_time
  5000,   // duration_ms
  0.0,    // peak_acceleration
  999.0,  // min_motion (start high so any value is lower)
  0.0, 0.0, 0.0  // peak axis values
};

static bool peak_detected = false;  // Track if we've seen the impact spike

// ===================================================================
// Fall Detection Initialization
// ===================================================================

void fall_detection_init() {
  fall_state = FALL_IDLE;
  potential_fall_time = 0;
  Serial.println("Fall Detection: Initialized");
}

// ===================================================================
// Calibration Functions
// ===================================================================

void fall_calibration_start(unsigned long duration_ms) {
  calibration.active = true;
  calibration.complete = false;
  calibration.start_time = millis();
  calibration.duration_ms = duration_ms;
  calibration.peak_acceleration = 0.0;
  calibration.min_motion = 999.0;
  calibration.peak_ax = 0.0;
  calibration.peak_ay = 0.0;
  calibration.peak_az = 0.0;
  peak_detected = false;
  
  Serial.println("Fall Calibration: STARTED - Perform a fall now!");
  Serial.printf("Fall Calibration: Recording for %lu ms\n", duration_ms);
}

void fall_calibration_stop() {
  if (calibration.active) {
    calibration.active = false;
    calibration.complete = true;
    Serial.println("Fall Calibration: STOPPED");
    Serial.printf("Fall Calibration Results:\n");
    Serial.printf("  Peak Acceleration: %.3f g\n", calibration.peak_acceleration);
    Serial.printf("  Min Motion (after peak): %.3f g\n", calibration.min_motion);
    Serial.printf("  Peak Axis Values: X=%.3f, Y=%.3f, Z=%.3f\n", 
                  calibration.peak_ax, calibration.peak_ay, calibration.peak_az);
  }
}

bool fall_calibration_is_active() {
  return calibration.active;
}

bool fall_calibration_is_complete() {
  return calibration.complete;
}

CalibrationData fall_calibration_get_results() {
  return calibration;
}

// ===================================================================
// Calibration Update (called from fall_detection_update)
// ===================================================================

static void calibration_update(const IMUData& imu) {
  if (!calibration.active) return;
  
  unsigned long elapsed = millis() - calibration.start_time;
  
  // Check if calibration time is up
  if (elapsed >= calibration.duration_ms) {
    fall_calibration_stop();
    return;
  }
  
  float accel_magnitude = sqrt(imu.ax * imu.ax + imu.ay * imu.ay + imu.az * imu.az);
  
  // Track peak acceleration (the impact spike)
  if (accel_magnitude > calibration.peak_acceleration) {
    calibration.peak_acceleration = accel_magnitude;
    calibration.peak_ax = imu.ax;
    calibration.peak_ay = imu.ay;
    calibration.peak_az = imu.az;
    
    // Mark that we've seen a significant spike (above 1.5g)
    if (accel_magnitude > 1.5) {
      peak_detected = true;
    }
    
    Serial.printf("Calibration: New peak %.3f g at (%+.2f, %+.2f, %+.2f)\n", 
                  accel_magnitude, imu.ax, imu.ay, imu.az);
  }
  
  // Only track minimum motion AFTER we've detected the impact spike
  // This captures the "stillness" phase after the fall
  if (peak_detected && accel_magnitude < calibration.min_motion) {
    calibration.min_motion = accel_magnitude;
    Serial.printf("Calibration: New min motion %.3f g\n", accel_magnitude);
  }
}

// ===================================================================
// Fall Detection Update
// ===================================================================

void fall_detection_update(const IMUData& imu) {
  if (!imu.valid) return;
  
  // Update calibration if active
  calibration_update(imu);
  
  // Skip normal fall detection during calibration
  if (calibration.active) return;
  
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
      if (accel_magnitude < g_config.fall_motion_threshold) {
        if (millis() - potential_fall_time >= g_config.fall_stillness_ms) {
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
