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
  float fall_ax_threshold;         // Fall detection threshold in g's
  int8_t ble_tx_power;            // BLE transmission power (-12 to 9 dBm)
  
  // Default values
  Config() {
    sensor_period_ms = 200;
    obstacle_threshold_mm = 800;
    fall_ax_threshold = 2.2;
    ble_tx_power = 7;
  }
  
  // Validation
  bool validate() {
    if (sensor_period_ms < 100 || sensor_period_ms > 1000) return false;
    if (obstacle_threshold_mm < 200 || obstacle_threshold_mm > 2000) return false;
    if (fall_ax_threshold < 0.5 || fall_ax_threshold > 5.0) return false;
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

#define FALL_MOTION_THRESHOLD 0.3    // Near-zero motion threshold (g's)
#define FALL_STILLNESS_DURATION_MS 1000  // Duration to confirm fall

#endif // CONFIG_H
