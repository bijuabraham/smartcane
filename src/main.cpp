#include <Arduino.h>
#include <ArduinoJson.h>
#include "pins.h"
#include "config.h"
#include "ble.h"
#include "sensors.h"
#include "fall_detection.h"
#include "haptics.h"

// ===================================================================
// Forward Declarations
// ===================================================================

void handle_sos_button(unsigned long now);
void update_sensors(unsigned long now);
void update_rfid(unsigned long now);

// ===================================================================
// Global Configuration Instance
// ===================================================================

Config g_config;

// ===================================================================
// State Variables
// ===================================================================

static unsigned long last_sensor_update = 0;
static unsigned long last_rfid_poll = 0;
static unsigned long last_battery_read = 0;
static unsigned long last_obstacle_alert = 0;

static bool sos_button_last_state = HIGH;
static unsigned long sos_button_debounce_time = 0;

static bool rfid_alert_sent = false;
static String last_rfid_uid = "";

// ===================================================================
// Setup
// ===================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=================================");
  Serial.println("  Smart Walking Stick Firmware  ");
  Serial.println("  ESP32-S3 + NimBLE              ");
  Serial.println("=================================\n");
  
  pinMode(SOS_BTN, INPUT_PULLUP);
  
  // Initialize haptics (buzzer, LED, vibration motor)
  haptics_init();
  
  Serial.println("Initializing sensors...");
  
  if (!sensors_init()) {
    Serial.println("WARNING: Sensor initialization failed!");
    Serial.println("Continuing without sensors (BLE test mode)...");
    Serial.println("You can still connect via Bluetooth and test the system.");
  } else {
    Serial.println("All sensors initialized successfully!");
    fall_detection_init();
  }
  
  Serial.println("\nInitializing BLE...");
  ble_init();
  
  // Startup beep to confirm buzzer is working
  haptics_trigger(HAPTIC_RFID);  // Quick beep
  delay(100);
  
  Serial.println("\nSetup complete! Ready to go.\n");
  Serial.print("Sensor update period: ");
  Serial.print(g_config.sensor_period_ms);
  Serial.println(" ms");
  Serial.print("Obstacle threshold: ");
  Serial.print(g_config.obstacle_threshold_mm);
  Serial.println(" mm");
  Serial.print("Fall threshold: ");
  Serial.print(g_config.fall_ax_threshold);
  Serial.println(" g");
}

// ===================================================================
// Loop
// ===================================================================

void loop() {
  unsigned long now = millis();
  
  haptics_update();
  ble_update();
  
  handle_sos_button(now);
  
  if (now - last_sensor_update >= g_config.sensor_period_ms) {
    last_sensor_update = now;
    update_sensors(now);
  }
  
  if (now - last_rfid_poll >= RFID_POLL_PERIOD_MS) {
    last_rfid_poll = now;
    update_rfid(now);
  }
  
#ifdef LOW_POWER
  if (LIGHT_SLEEP_ENABLED && !ble_is_connected()) {
    uint32_t sleep_time = min(
      g_config.sensor_period_ms - (now - last_sensor_update),
      RFID_POLL_PERIOD_MS - (now - last_rfid_poll)
    );
    if (sleep_time > 10) {
      esp_sleep_enable_timer_wakeup(sleep_time * 1000);
      esp_light_sleep_start();
    }
  }
#endif
  
  delay(10);
}

// ===================================================================
// SOS Button Handler
// ===================================================================

void handle_sos_button(unsigned long now) {
  static bool sos_triggered = false;
  bool current_state = digitalRead(SOS_BTN);
  
  // Detect state change and start debounce timer
  if (current_state != sos_button_last_state) {
    sos_button_debounce_time = now;
    sos_button_last_state = current_state;
    Serial.printf("SOS Button state changed to: %s\n", current_state == LOW ? "PRESSED" : "RELEASED");
  }
  
  // After debounce period, trigger action on button press (LOW)
  if (current_state == LOW && (now - sos_button_debounce_time) > SOS_DEBOUNCE_MS) {
    if (!sos_triggered) {
      sos_triggered = true;
      Serial.println("SOS BUTTON TRIGGERED!");
      
      haptics_trigger(HAPTIC_SOS);
      
      StaticJsonDocument<64> doc;
      doc["event"] = "SOS_BUTTON_PRESSED";
      
      char json[64];
      serializeJson(doc, json);
      ble_send_alert(json);
    }
  } else if (current_state == HIGH) {
    // Reset trigger when button is released
    sos_triggered = false;
  }
}

// ===================================================================
// Update Sensors
// ===================================================================

void update_sensors(unsigned long now) {
  IMUData imu = imu_read();
  ToFData tof = tof_read();
  BatteryData battery;
  
  // Read battery immediately on first call (last_battery_read == 0), then every 10 seconds
  if (last_battery_read == 0 || (now - last_battery_read >= BATTERY_READ_PERIOD_MS)) {
    battery = battery_read();
    last_battery_read = now;
  } else {
    battery.valid = false;
  }
  
  // Debug output to Serial Monitor
  if (tof.valid) {
    Serial.print("ToF: ");
    Serial.print(tof.distance_mm);
    Serial.print(" mm  ");
  }
  if (imu.valid) {
    Serial.print("IMU: ax=");
    Serial.print(imu.ax, 2);
    Serial.print(" ay=");
    Serial.print(imu.ay, 2);
    Serial.print(" az=");
    Serial.print(imu.az, 2);
    Serial.print("  ");
  }
  if (battery.valid) {
    Serial.print("Batt: ");
    Serial.print(battery.percentage);
    Serial.print("%");
  }
  if (tof.valid || imu.valid || battery.valid) {
    Serial.println();
  }
  
  if (imu.valid) {
    fall_detection_update(imu);
    
    float fall_ax, fall_ay, fall_az;
    if (fall_detection_check(fall_ax, fall_ay, fall_az)) {
      haptics_trigger(HAPTIC_FALL);
      
      StaticJsonDocument<128> doc;
      doc["event"] = "FALL_DETECTED";
      doc["severity"] = "high";
      doc["ax"] = fall_ax;
      doc["ay"] = fall_ay;
      doc["az"] = fall_az;
      
      char json[128];
      serializeJson(doc, json);
      ble_send_alert(json);
      
      fall_detection_reset();
    }
  }
  
  if (tof.valid && tof.distance_mm > 0 && 
      tof.distance_mm < g_config.obstacle_threshold_mm) {
    if (now - last_obstacle_alert >= OBSTACLE_ALERT_COOLDOWN_MS) {
      last_obstacle_alert = now;
      
      haptics_trigger(HAPTIC_OBSTACLE);
      
      StaticJsonDocument<64> doc;
      doc["event"] = "OBSTACLE_NEAR";
      doc["dist_mm"] = tof.distance_mm;
      
      char json[64];
      serializeJson(doc, json);
      ble_send_alert(json);
    }
  }
  
  // Serialize sensor data to JSON
  StaticJsonDocument<256> doc;
  
  if (imu.valid) {
    JsonObject imu_obj = doc.createNestedObject("imu");
    imu_obj["ax"] = imu.ax;
    imu_obj["ay"] = imu.ay;
    imu_obj["az"] = imu.az;
    imu_obj["gx"] = imu.gx;
    imu_obj["gy"] = imu.gy;
    imu_obj["gz"] = imu.gz;
  }
  
  if (tof.valid) {
    doc["dist_mm"] = tof.distance_mm;
  }
  
  if (battery.valid) {
    JsonObject bat_obj = doc.createNestedObject("battery");
    bat_obj["v"] = battery.voltage;
    bat_obj["pct"] = battery.percentage;
  }
  
  doc["ts"] = now;
  
  char json[256];
  serializeJson(doc, json);
  ble_send_sensor_data(json);
}

// ===================================================================
// Update RFID
// ===================================================================

void update_rfid(unsigned long now) {
  RFIDData rfid = rfid_read();
  
  if (rfid.valid) {
    String current_uid = String(rfid.uid);
    
    if (current_uid != last_rfid_uid && !rfid_alert_sent) {
      haptics_trigger(HAPTIC_RFID);
      
      StaticJsonDocument<64> doc;
      doc["event"] = "RFID_SEEN";
      doc["uid"] = rfid.uid;
      
      char json[64];
      serializeJson(doc, json);
      ble_send_alert(json);
      
      rfid_alert_sent = true;
      last_rfid_uid = current_uid;
    }
  } else {
    if (last_rfid_uid != "") {
      last_rfid_uid = "";
      rfid_alert_sent = false;
    }
  }
}

// ===================================================================
// Send Sensor Data via BLE
// ===================================================================

void send_sensor_data(const IMUData& imu, const ToFData& tof, 
                     const BatteryData& battery, unsigned long now) {
  StaticJsonDocument<256> doc;
  
  doc["ts"] = now;
  
  if (imu.valid) {
    JsonObject imu_obj = doc.createNestedObject("imu");
    imu_obj["ax"] = round(imu.ax * 100) / 100.0;
    imu_obj["ay"] = round(imu.ay * 100) / 100.0;
    imu_obj["az"] = round(imu.az * 100) / 100.0;
    imu_obj["gx"] = round(imu.gx * 10) / 10.0;
    imu_obj["gy"] = round(imu.gy * 10) / 10.0;
    imu_obj["gz"] = round(imu.gz * 10) / 10.0;
  }
  
  doc["dist_mm"] = tof.valid ? tof.distance_mm : -1;
  
  const char* uid = rfid_get_current_uid();
  if (uid) {
    doc["rfid"] = uid;
  } else {
    doc["rfid"] = nullptr;
  }
  
#ifdef BATTERY_ADC
  if (battery.valid) {
    JsonObject batt_obj = doc.createNestedObject("battery");
    batt_obj["v"] = round(battery.voltage * 100) / 100.0;
    batt_obj["pct"] = battery.percentage;
  }
#endif
  
  char json[256];
  serializeJson(doc, json);
  ble_send_sensor_data(json);
}
