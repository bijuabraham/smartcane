#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===================================================================
// Runtime Configuration Structure
// ===================================================================
// These values can be updated via BLE CONFIG_CHAR
// ===================================================================

struct Config {
  uint16_t sensor_period_ms;       // Sensor sampling period (100-1000ms)
  uint16_t obstacle_threshold_mm;  // Obstacle alert distance (200-2000mm)
  float fall_ax_threshold;         // Fall detection impact threshold in g's
  float fall_motion_threshold;     // Stillness detection threshold in g's
  uint16_t fall_stillness_ms;      // Duration to confirm fall (ms)
  int8_t ble_tx_power;            // BLE transmission power (-12 to 9 dBm)
  
  // Default values
  Config() {
    sensor_period_ms = 200;
    obstacle_threshold_mm = 800;
    fall_ax_threshold = 2.2;
    fall_motion_threshold = 0.3;
    fall_stillness_ms = 1000;
    ble_tx_power = 7;
  }
  
  // Validation
  bool validate() {
    if (sensor_period_ms < 100 || sensor_period_ms > 1000) return false;
    if (obstacle_threshold_mm < 200 || obstacle_threshold_mm > 2000) return false;
    if (fall_ax_threshold < 0.5 || fall_ax_threshold > 5.0) return false;
    if (fall_motion_threshold < 0.1 || fall_motion_threshold > 1.0) return false;
    if (fall_stillness_ms < 200 || fall_stillness_ms > 5000) return false;
    if (ble_tx_power < -12 || ble_tx_power > 9) return false;
    return true;
  }
};

// Global configuration instance
extern Config g_config;

// ===================================================================
// Compile-time Options
// ===================================================================

// Uncomment to enable low-power mode (light sleep between cycles)
// #define LOW_POWER

// Low-power mode parameters
#ifdef LOW_POWER
  #define LIGHT_SLEEP_ENABLED true
  #define BLE_CONN_INTERVAL_MIN 80   // 100ms (units of 1.25ms)
  #define BLE_CONN_INTERVAL_MAX 160  // 200ms
#else
  #define LIGHT_SLEEP_ENABLED false
  #define BLE_CONN_INTERVAL_MIN 12   // 15ms
  #define BLE_CONN_INTERVAL_MAX 24   // 30ms
#endif

// ===================================================================
// Timing Constants
// ===================================================================

#define RFID_POLL_PERIOD_MS 200
#define RFID_DEDUPLICATE_MS 3000
#define BATTERY_READ_PERIOD_MS 10000
#define IMU_SAMPLE_RATE_HZ 100
#define OBSTACLE_ALERT_COOLDOWN_MS 1000
#define SOS_DEBOUNCE_MS 20

// ===================================================================
// Battery Monitoring Constants
// ===================================================================

#define BATTERY_MIN_VOLTAGE 3.0
#define BATTERY_MAX_VOLTAGE 4.2
#define BATTERY_FILTER_ALPHA 0.1  // Low-pass filter coefficient

// ===================================================================
// Fall Detection Constants
// ===================================================================

// Note: FALL_MOTION_THRESHOLD and FALL_STILLNESS_DURATION_MS are now
// configurable via g_config.fall_motion_threshold and g_config.fall_stillness_ms

#endif // CONFIG_H
