#include <Arduino.h>
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 20

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== I2C SCANNER ===");
  Serial.println("Scanning I2C bus...");
  Serial.println("SDA: GPIO 21");
  Serial.println("SCL: GPIO 20\n");
  
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);
  
  byte count = 0;
  
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at address 0x");
      if (addr < 16) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.println();
      count++;
    }
  }
  
  Serial.println("\n=== SCAN COMPLETE ===");
  if (count == 0) {
    Serial.println("NO I2C devices found!");
    Serial.println("\nPossible problems:");
    Serial.println("1. Loose wires - check all 4 connections");
    Serial.println("2. Wrong pins - verify GPIO 21 and 20");
    Serial.println("3. No power to sensor - check 3.3V");
    Serial.println("4. Faulty sensor or wires");
  } else {
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" device(s)");
    Serial.println("\nVL53L1X should be at 0x29");
  }
}

void loop() {
  delay(5000);
  Serial.println("\nPress RESET to scan again...");
  delay(5000);
}
