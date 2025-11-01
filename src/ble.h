#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <NimBLEDevice.h>

// ===================================================================
// BLE GATT Service and Characteristic UUIDs
// ===================================================================

#define SMART_STICK_SVC_UUID    "12345678-1234-1234-1234-1234567890ab"
#define SENSOR_DATA_CHAR_UUID   "12345678-1234-1234-1234-1234567890ad"
#define ALERTS_CHAR_UUID        "12345678-1234-1234-1234-1234567890ae"
#define CONFIG_CHAR_UUID        "12345678-1234-1234-1234-1234567890af"

#define BLE_DEVICE_NAME "SmartStick"

// ===================================================================
// BLE Function Declarations
// ===================================================================

void ble_init();
void ble_update();
bool ble_is_connected();
void ble_send_sensor_data(const char* json);
void ble_send_alert(const char* json);
void ble_set_tx_power(int8_t power);

// ===================================================================
// BLE Characteristic Pointers (extern)
// ===================================================================

extern NimBLECharacteristic* pSensorDataChar;
extern NimBLECharacteristic* pAlertsChar;
extern NimBLECharacteristic* pConfigChar;
extern NimBLEServer* pServer;

#endif // BLE_H
