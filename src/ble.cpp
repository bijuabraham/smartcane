#include "ble.h"
#include "config.h"
#include <ArduinoJson.h>

// ===================================================================
// BLE Global Variables
// ===================================================================

NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pSensorDataChar = nullptr;
NimBLECharacteristic* pAlertsChar = nullptr;
NimBLECharacteristic* pConfigChar = nullptr;

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
        pCharacteristic->setValue(response);
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
      if (doc.containsKey("ble_tx_power")) {
        new_config.ble_tx_power = doc["ble_tx_power"];
      }
      
      if (new_config.validate()) {
        g_config = new_config;
        
        if (doc.containsKey("ble_tx_power")) {
          ble_set_tx_power(g_config.ble_tx_power);
        }
        
        Serial.println("BLE: Config updated successfully");
        const char* response = "{\"ok\":true}";
        pCharacteristic->setValue(response);
        pCharacteristic->notify();
      } else {
        Serial.println("BLE: Config validation failed");
        const char* response = "{\"ok\":false,\"err\":\"Validation failed\"}";
        pCharacteristic->setValue(response);
        pCharacteristic->notify();
      }
    }
  }
};

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
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
  );
  pConfigChar->setCallbacks(new ConfigCharCallbacks());
  
  const char* initialConfig = "{\"sensor_period_ms\":200,\"obstacle_threshold_mm\":800,\"fall_ax_threshold\":2.2,\"ble_tx_power\":7}";
  pConfigChar->setValue(initialConfig);
  
  pService->start();
  
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
