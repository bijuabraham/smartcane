#include "ble.h"
#include "config.h"
#include "fall_detection.h"
#include <ArduinoJson.h>

// ===================================================================
// BLE Global Variables
// ===================================================================

NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pSensorDataChar = nullptr;
NimBLECharacteristic* pAlertsChar = nullptr;
NimBLECharacteristic* pConfigChar = nullptr;
NimBLECharacteristic* pCalibrationChar = nullptr;

static bool deviceConnected = false;
static bool oldDeviceConnected = false;

// ===================================================================
// Server Callbacks
// ===================================================================

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer) {
    deviceConnected = true;
    Serial.println("BLE: Client connected");
  }
  
  void onDisconnect(NimBLEServer* pServer) {
    deviceConnected = false;
    Serial.println("BLE: Client disconnected");
  }
};

// ===================================================================
// Config Characteristic Callbacks
// ===================================================================

class ConfigCharCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
      Serial.println("BLE: Received config write");
      
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, value.c_str());
      
      if (error) {
        Serial.print("BLE: JSON parse error: ");
        Serial.println(error.c_str());
        
        const char* response = "{\"ok\":false,\"err\":\"Invalid JSON\"}";
        size_t len = strlen(response);
        pCharacteristic->setValue((uint8_t*)response, len);
        pCharacteristic->notify();
        return;
      }
      
      Config new_config = g_config;
      
      if (doc.containsKey("sensor_period_ms")) {
        new_config.sensor_period_ms = doc["sensor_period_ms"];
      }
      if (doc.containsKey("obstacle_threshold_mm")) {
        new_config.obstacle_threshold_mm = doc["obstacle_threshold_mm"];
      }
      if (doc.containsKey("fall_ax_threshold")) {
        new_config.fall_ax_threshold = doc["fall_ax_threshold"];
      }
      if (doc.containsKey("fall_motion_threshold")) {
        new_config.fall_motion_threshold = doc["fall_motion_threshold"];
      }
      if (doc.containsKey("fall_stillness_ms")) {
        new_config.fall_stillness_ms = doc["fall_stillness_ms"];
      }
      if (doc.containsKey("ble_tx_power")) {
        new_config.ble_tx_power = doc["ble_tx_power"];
      }
      
      if (new_config.validate()) {
        g_config = new_config;
        
        if (doc.containsKey("ble_tx_power")) {
          ble_set_tx_power(g_config.ble_tx_power);
        }
        
        Serial.println("BLE: Config updated successfully");
        
        // Send success response with updated config
        StaticJsonDocument<256> responseDoc;
        responseDoc["ok"] = true;
        responseDoc["sensor_period_ms"] = g_config.sensor_period_ms;
        responseDoc["obstacle_threshold_mm"] = g_config.obstacle_threshold_mm;
        responseDoc["fall_ax_threshold"] = g_config.fall_ax_threshold;
        responseDoc["fall_motion_threshold"] = g_config.fall_motion_threshold;
        responseDoc["fall_stillness_ms"] = g_config.fall_stillness_ms;
        responseDoc["ble_tx_power"] = g_config.ble_tx_power;
        
        char response[256];
        size_t len = serializeJson(responseDoc, response, sizeof(response));
        pCharacteristic->setValue((uint8_t*)response, len);
        pCharacteristic->notify();
      } else {
        Serial.println("BLE: Config validation failed");
        const char* response = "{\"ok\":false,\"err\":\"Validation failed\"}";
        size_t len = strlen(response);
        pCharacteristic->setValue((uint8_t*)response, len);
        pCharacteristic->notify();
      }
    }
  }
};

// ===================================================================
// Calibration Characteristic Callbacks
// ===================================================================

class CalibrationCharCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
      Serial.println("BLE: Received calibration command");
      
      StaticJsonDocument<128> doc;
      DeserializationError error = deserializeJson(doc, value.c_str());
      
      if (error) {
        Serial.print("BLE: Calibration JSON parse error: ");
        Serial.println(error.c_str());
        return;
      }
      
      if (doc.containsKey("cmd")) {
        const char* cmd = doc["cmd"];
        
        if (strcmp(cmd, "start") == 0) {
          unsigned long duration = doc["duration_ms"] | 5000;
          fall_calibration_start(duration);
          
          StaticJsonDocument<128> responseDoc;
          responseDoc["status"] = "started";
          responseDoc["duration_ms"] = duration;
          
          char response[128];
          size_t len = serializeJson(responseDoc, response, sizeof(response));
          pCharacteristic->setValue((uint8_t*)response, len);
          pCharacteristic->notify();
        }
        else if (strcmp(cmd, "stop") == 0) {
          fall_calibration_stop();
          ble_send_calibration_result();
        }
        else if (strcmp(cmd, "status") == 0) {
          ble_send_calibration_result();
        }
      }
    }
  }
};

// ===================================================================
// Send Calibration Result over BLE
// ===================================================================

void ble_send_calibration_result() {
  if (!pCalibrationChar) return;
  
  CalibrationData cal = fall_calibration_get_results();
  
  StaticJsonDocument<256> doc;
  doc["status"] = cal.active ? "active" : (cal.complete ? "complete" : "idle");
  doc["peak_acceleration"] = cal.peak_acceleration;
  doc["min_motion"] = cal.min_motion < 900 ? cal.min_motion : 0.0;  // If still at 999, report 0
  doc["peak_ax"] = cal.peak_ax;
  doc["peak_ay"] = cal.peak_ay;
  doc["peak_az"] = cal.peak_az;
  
  // Suggest threshold values (with some headroom)
  if (cal.complete && cal.peak_acceleration > 1.5) {
    // Subtract 1.0g (resting) and add margin
    float suggested_impact = (cal.peak_acceleration - 1.0) * 0.8;
    float suggested_motion = cal.min_motion * 1.2;
    doc["suggested_impact_threshold"] = suggested_impact;
    doc["suggested_motion_threshold"] = suggested_motion;
  }
  
  char json[256];
  size_t len = serializeJson(doc, json, sizeof(json));
  pCalibrationChar->setValue((uint8_t*)json, len);
  pCalibrationChar->notify();
  
  Serial.print("BLE Calibration: ");
  Serial.println(json);
}

// ===================================================================
// Helper: Serialize current config to BLE characteristic
// ===================================================================

static void ble_sync_config() {
  if (!pConfigChar) return;
  
  StaticJsonDocument<256> doc;
  doc["sensor_period_ms"] = g_config.sensor_period_ms;
  doc["obstacle_threshold_mm"] = g_config.obstacle_threshold_mm;
  doc["fall_ax_threshold"] = g_config.fall_ax_threshold;
  doc["fall_motion_threshold"] = g_config.fall_motion_threshold;
  doc["fall_stillness_ms"] = g_config.fall_stillness_ms;
  doc["ble_tx_power"] = g_config.ble_tx_power;
  
  char json[256];
  size_t len = serializeJson(doc, json, sizeof(json));
  pConfigChar->setValue((uint8_t*)json, len);
}

// ===================================================================
// BLE Initialization
// ===================================================================

void ble_init() {
  Serial.println("BLE: Initializing...");
  
  NimBLEDevice::init(BLE_DEVICE_NAME);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  
  NimBLEService* pService = pServer->createService(SMART_STICK_SVC_UUID);
  
  pSensorDataChar = pService->createCharacteristic(
    SENSOR_DATA_CHAR_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );
  
  pAlertsChar = pService->createCharacteristic(
    ALERTS_CHAR_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );
  
  pConfigChar = pService->createCharacteristic(
    CONFIG_CHAR_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
  );
  pConfigChar->setCallbacks(new ConfigCharCallbacks());
  
  pCalibrationChar = pService->createCharacteristic(
    CALIBRATION_CHAR_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
  );
  pCalibrationChar->setCallbacks(new CalibrationCharCallbacks());
  
  pService->start();
  
  // Set initial config from current g_config values
  ble_sync_config();
  
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SMART_STICK_SVC_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMaxPreferred(0x12);
  
  NimBLEDevice::startAdvertising();
  Serial.println("BLE: Advertising started");
}

// ===================================================================
// BLE Update (handle connection state changes)
// ===================================================================

void ble_update() {
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("BLE: Restarted advertising");
    oldDeviceConnected = deviceConnected;
  }
  
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}

// ===================================================================
// BLE Connection Status
// ===================================================================

bool ble_is_connected() {
  return deviceConnected;
}

// ===================================================================
// Send Sensor Data
// ===================================================================

void ble_send_sensor_data(const char* json) {
  if (deviceConnected && pSensorDataChar->getSubscribedCount() > 0) {
    size_t len = strlen(json);
    pSensorDataChar->setValue((uint8_t*)json, len);
    pSensorDataChar->notify();
    Serial.print("BLE Sensor: ");
    Serial.println(json);
  }
}

// ===================================================================
// Send Alert
// ===================================================================

void ble_send_alert(const char* json) {
  if (deviceConnected && pAlertsChar->getSubscribedCount() > 0) {
    size_t len = strlen(json);
    pAlertsChar->setValue((uint8_t*)json, len);
    pAlertsChar->notify();
    Serial.print("BLE Alert: ");
    Serial.println(json);
  }
}

// ===================================================================
// Set BLE TX Power
// ===================================================================

void ble_set_tx_power(int8_t power) {
  esp_power_level_t powerLevel;
  
  if (power <= -12) powerLevel = ESP_PWR_LVL_N12;
  else if (power <= -9) powerLevel = ESP_PWR_LVL_N9;
  else if (power <= -6) powerLevel = ESP_PWR_LVL_N6;
  else if (power <= -3) powerLevel = ESP_PWR_LVL_N3;
  else if (power <= 0) powerLevel = ESP_PWR_LVL_N0;
  else if (power <= 3) powerLevel = ESP_PWR_LVL_P3;
  else if (power <= 6) powerLevel = ESP_PWR_LVL_P6;
  else powerLevel = ESP_PWR_LVL_P9;
  
  NimBLEDevice::setPower(powerLevel);
  Serial.print("BLE: TX power set to ");
  Serial.print(power);
  Serial.println(" dBm");
}
