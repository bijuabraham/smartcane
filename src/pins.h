#ifndef PINS_H
#define PINS_H

// ===================================================================
// ESP32-S3 Pin Definitions for Smart Walking Stick
// ===================================================================
// IMPORTANT: These pin assignments are DEFAULTS and may vary by vendor!
// Verify your specific ESP32-S3 dev board pinout before uploading.
// Adjust these values to match your actual hardware connections.
// ===================================================================

// I2C Pins (for MPU6050 IMU and VL53L1X ToF sensor)
// Trying GPIO 19 and GPIO 20 for better compatibility
#define I2C_SDA 19
#define I2C_SCL 20

// SPI Pins (for MFRC522 RFID reader)
#define SPI_SCK  12
#define SPI_MISO 13
#define SPI_MOSI 11
#define RFID_CS  10
#define RFID_RST 14

// User Interface Pins
#define SOS_BTN    15   // Active LOW with internal pullup
#define BUZZER     2    // Digital output for piezo buzzer
#define VIB_MOTOR  3    // Digital output (or PWM for variable intensity)
#define LED        1    // Status LED (active HIGH)

// Battery Monitoring (optional)
// Comment out the next line if battery monitoring is not wired
#define BATTERY_ADC 4   // ADC pin for battery voltage divider

// ===================================================================
// Pin Validation
// ===================================================================
// ESP32-S3 ADC channels: GPIO1-GPIO20 (ADC1), GPIO47-GPIO48 (ADC2 - avoid with WiFi/BT)
// Ensure BATTERY_ADC is a valid ADC1 pin if defined

#endif // PINS_H
