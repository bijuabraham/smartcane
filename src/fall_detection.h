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
// Calibration Data Structure
// ===================================================================

struct CalibrationData {
  bool active;                // Calibration in progress
  bool complete;              // Calibration finished
  unsigned long start_time;   // When calibration started
  unsigned long duration_ms;  // How long to record (default 5000ms)
  float peak_acceleration;    // Highest acceleration magnitude recorded
  float min_motion;           // Lowest motion (after peak) for stillness detection
  float peak_ax, peak_ay, peak_az;  // Individual axis values at peak
};

// ===================================================================
// Fall Detection Functions
// ===================================================================

void fall_detection_init();
void fall_detection_update(const IMUData& imu);
bool fall_detection_check(float& ax, float& ay, float& az);
void fall_detection_reset();

// ===================================================================
// Calibration Functions
// ===================================================================

void fall_calibration_start(unsigned long duration_ms = 5000);
void fall_calibration_stop();
bool fall_calibration_is_active();
bool fall_calibration_is_complete();
CalibrationData fall_calibration_get_results();

#endif // FALL_DETECTION_H
