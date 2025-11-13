#include <Arduino.h>
#include <Wire.h>

#define I2C_SDA 19
#define I2C_SCL 20

void scanI2C(uint32_t frequency) {
  Serial.print("\n=== I2C SCAN at ");
  Serial.print(frequency / 1000);
  Serial.println(" kHz ===");
  
  Wire.begin(I2C_SDA, I2C_SCL, frequency);
  delay(100);
  
  int count = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      count++;
    }
  }
  
  if (count == 0) {
    Serial.println("NO devices found!");
  } else {
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" device(s)");
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n====================================");
  Serial.println("  I2C ADVANCED DIAGNOSTIC TEST");
  Serial.println("====================================");
  Serial.print("SDA: GPIO ");
  Serial.println(I2C_SDA);
  Serial.print("SCL: GPIO ");
  Serial.println(I2C_SCL);
  Serial.println("====================================\n");
  
  // Test at different I2C speeds
  scanI2C(10000);    // 10 kHz - very slow, most reliable
  delay(500);
  
  scanI2C(50000);    // 50 kHz
  delay(500);
  
  scanI2C(100000);   // 100 kHz - standard mode
  delay(500);
  
  scanI2C(400000);   // 400 kHz - fast mode
  delay(500);
  
  Serial.println("\n====================================");
  Serial.println("Test complete!");
  Serial.println("====================================");
  Serial.println("\nIf NO devices found at ANY speed:");
  Serial.println("1. Check VL53L1X VIN = 3.3V");
  Serial.println("2. Check GND connected");
  Serial.println("3. Verify SDA wire to GPIO 19");
  Serial.println("4. Verify SCL wire to GPIO 20");
  Serial.println("5. Try external 4.7k pull-up resistors");
  Serial.println("6. Test sensor on different board");
  Serial.println("\nPress RESET to scan again...");
}

void loop() {
  // Nothing
}
