#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// ===================================================================
// Sensor Data Structures
// ===================================================================

struct IMUData {
  float ax, ay, az;  // Acceleration in g's
  float gx, gy, gz;  // Gyroscope in deg/s
  bool valid;
};

struct ToFData {
  int16_t distance_mm;  // -1 if invalid
  bool valid;
};

struct RFIDData {
  char uid[20];         // Hex string of UID
  bool valid;
  unsigned long last_seen_ms;
};

struct BatteryData {
  float voltage;
  uint8_t percentage;
  bool valid;
};

// ===================================================================
// Sensor Function Declarations
// ===================================================================

bool sensors_init();
void sensors_update();

IMUData imu_read();
ToFData tof_read();
RFIDData rfid_read();
BatteryData battery_read();

const char* rfid_get_current_uid();
bool rfid_has_recent_tag();

#endif // SENSORS_H
