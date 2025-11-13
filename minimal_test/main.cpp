#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L1X.h>

#define I2C_SDA 21
#define I2C_SCL 20

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n=== VL53L1X MINIMAL TEST ===");
  Serial.println("I2C: GPIO 21 (SDA), GPIO 20 (SCL)");
  
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("I2C initialized");
  
  if (!vl53.begin(0x29, &Wire)) {
    Serial.println("ERROR: VL53L1X NOT FOUND!");
    Serial.println("Check wiring:");
    Serial.println("  VL53L1X VIN -> ESP32 3.3V");
    Serial.println("  VL53L1X GND -> ESP32 GND");
    Serial.println("  VL53L1X SDA -> ESP32 GPIO 21");
    Serial.println("  VL53L1X SCL -> ESP32 GPIO 20");
    while(1) delay(1000);
  }
  
  Serial.println("SUCCESS: VL53L1X FOUND!");
  
  if (!vl53.startRanging()) {
    Serial.println("ERROR: Could not start ranging");
    while(1) delay(1000);
  }
  
  vl53.setTimingBudget(50);
  Serial.println("Ready! Reading distance...\n");
}

void loop() {
  if (vl53.dataReady()) {
    int16_t distance = vl53.distance();
    
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" mm");
    
    vl53.clearInterrupt();
  }
  
  delay(200);
}
