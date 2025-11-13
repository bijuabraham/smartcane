/*
 * Minimal VL53L1X Test for ESP32-S3
 * Tests ONLY the distance sensor - no other peripherals
 */

#include <Wire.h>
#include <Adafruit_VL53L1X.h>

// Pin definitions - adjust if needed
#define I2C_SDA 21
#define I2C_SCL 20

Adafruit_VL53L1X vl53;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=================================");
  Serial.println("  VL53L1X Test - ESP32-S3");
  Serial.println("=================================\n");
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("I2C initialized on GPIO 21 (SDA) and GPIO 20 (SCL)");
  
  // Try to initialize VL53L1X
  Serial.println("Looking for VL53L1X sensor...");
  
  if (!vl53.begin(0x29, &Wire)) {
    Serial.println("ERROR: VL53L1X not found!");
    Serial.println("\nCheck your wiring:");
    Serial.println("  VL53L1X VIN  -> ESP32 3.3V");
    Serial.println("  VL53L1X GND  -> ESP32 GND");
    Serial.println("  VL53L1X SDA  -> ESP32 GPIO 21");
    Serial.println("  VL53L1X SCL  -> ESP32 GPIO 20");
    while (1) delay(1000);
  }
  
  Serial.println("✓ VL53L1X found!");
  
  if (!vl53.startRanging()) {
    Serial.println("ERROR: Could not start ranging!");
    while (1) delay(1000);
  }
  
  Serial.println("✓ Ranging started!");
  
  vl53.setTimingBudget(50);
  Serial.println("✓ Timing budget set to 50ms");
  
  Serial.println("\n=================================");
  Serial.println("Reading distance every 200ms:");
  Serial.println("=================================\n");
}

void loop() {
  if (vl53.dataReady()) {
    int16_t distance = vl53.distance();
    
    if (distance > 0 && distance < 4000) {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" mm");
    } else {
      Serial.println("Out of range");
    }
    
    vl53.clearInterrupt();
  }
  
  delay(200);
}
