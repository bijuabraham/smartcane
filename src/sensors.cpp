#include "sensors.h"
#include "pins.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_VL53L1X.h>
#include <MFRC522.h>

// ===================================================================
// Sensor Objects
// ===================================================================

static Adafruit_MPU6050 mpu;
static Adafruit_VL53L1X vl53;
static MFRC522 rfid(RFID_CS, RFID_RST);

static RFIDData last_rfid_data = {0};
static float battery_filtered = 0.0;
static bool sensors_initialized = false;

// Individual sensor initialization flags
static bool mpu_initialized = false;
static bool vl53_initialized = false;
static bool rfid_initialized = false;

// ===================================================================
// Sensor Initialization
// ===================================================================

bool sensors_init() {
  Serial.println("Sensors: Initializing...");
  
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Try to initialize MPU6050
  if (mpu.begin()) {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("Sensors: MPU6050 OK");
    mpu_initialized = true;
  } else {
    Serial.println("Sensors: MPU6050 not found (skipping)");
    mpu_initialized = false;
  }
  
  // Try to initialize VL53L1X
  Serial.println("Sensors: Initializing VL53L1X on I2C...");
  delay(100); // Give sensor time to power up
  
  if (vl53.begin(0x29, &Wire)) {
    Serial.println("Sensors: VL53L1X detected at 0x29");
    delay(50);
    
    if (vl53.startRanging()) {
      vl53.setTimingBudget(50);
      Serial.println("Sensors: VL53L1X OK - Ranging started!");
      vl53_initialized = true;
    } else {
      Serial.println("ERROR: VL53L1X ranging start failed!");
      vl53_initialized = false;
    }
  } else {
    Serial.println("ERROR: VL53L1X not found at 0x29!");
    Serial.println("Check: SDA=GPIO19, SCL=GPIO20, VIN=3.3V, GND connected");
    vl53_initialized = false;
  }
  
  // Try to initialize MFRC522
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  rfid.PCD_Init();
  
  if (rfid.PCD_PerformSelfTest()) {
    rfid.PCD_Init();
    Serial.println("Sensors: MFRC522 OK");
    rfid_initialized = true;
  } else {
    Serial.println("Sensors: MFRC522 not found (skipping)");
    rfid_initialized = false;
  }
  
#ifdef BATTERY_ADC
  pinMode(BATTERY_ADC, INPUT);
  analogSetAttenuation(ADC_11db);
  Serial.println("Sensors: Battery monitoring enabled");
#endif
  
  // Check if at least one sensor initialized
  if (mpu_initialized || vl53_initialized || rfid_initialized) {
    Serial.print("Sensors: Initialization complete (");
    Serial.print("MPU:");
    Serial.print(mpu_initialized ? "Y" : "N");
    Serial.print(" VL53:");
    Serial.print(vl53_initialized ? "Y" : "N");
    Serial.print(" RFID:");
    Serial.print(rfid_initialized ? "Y" : "N");
    Serial.println(")");
    sensors_initialized = true;
    return true;
  } else {
    Serial.println("Sensors: No sensors found!");
    sensors_initialized = false;
    return false;
  }
}

// ===================================================================
// Sensor Update (periodic maintenance)
// ===================================================================

void sensors_update() {
}

// ===================================================================
// IMU Read
// ===================================================================

IMUData imu_read() {
  IMUData data = {0};
  
  if (!mpu_initialized) {
    data.valid = false;
    return data;
  }
  
  sensors_event_t accel, gyro, temp;
  if (mpu.getEvent(&accel, &gyro, &temp)) {
    data.ax = accel.acceleration.x / 9.81;
    data.ay = accel.acceleration.y / 9.81;
    data.az = accel.acceleration.z / 9.81;
    data.gx = gyro.gyro.x * 57.2958;
    data.gy = gyro.gyro.y * 57.2958;
    data.gz = gyro.gyro.z * 57.2958;
    data.valid = true;
  } else {
    data.valid = false;
  }
  
  return data;
}

// ===================================================================
// ToF Read
// ===================================================================

ToFData tof_read() {
  ToFData data = {0};
  
  if (!vl53_initialized) {
    data.distance_mm = -1;
    data.valid = false;
    return data;
  }
  
  if (vl53.dataReady()) {
    int16_t distance = vl53.distance();
    
    if (distance > 0 && distance < 4000) {
      data.distance_mm = distance;
      data.valid = true;
    } else {
      data.distance_mm = -1;
      data.valid = false;
    }
    
    vl53.clearInterrupt();
  } else {
    data.distance_mm = -1;
    data.valid = false;
  }
  
  return data;
}

// ===================================================================
// RFID Read
// ===================================================================

RFIDData rfid_read() {
  RFIDData data = {0};
  
  if (!rfid_initialized) {
    data.valid = false;
    return data;
  }
  
  if (!rfid.PICC_IsNewCardPresent()) {
    if (last_rfid_data.valid && 
        (millis() - last_rfid_data.last_seen_ms > RFID_DEDUPLICATE_MS)) {
      last_rfid_data.valid = false;
      last_rfid_data.uid[0] = '\0';
    }
    return last_rfid_data;
  }
  
  if (!rfid.PICC_ReadCardSerial()) {
    return last_rfid_data;
  }
  
  char uid_str[20] = {0};
  for (byte i = 0; i < rfid.uid.size; i++) {
    sprintf(uid_str + (i * 2), "%02X", rfid.uid.uidByte[i]);
  }
  
  bool is_new = (strcmp(uid_str, last_rfid_data.uid) != 0);
  
  strcpy(last_rfid_data.uid, uid_str);
  last_rfid_data.valid = true;
  last_rfid_data.last_seen_ms = millis();
  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  
  data = last_rfid_data;
  
  if (is_new) {
    Serial.print("Sensors: RFID detected: ");
    Serial.println(uid_str);
  }
  
  return data;
}

// ===================================================================
// Battery Read
// ===================================================================

BatteryData battery_read() {
  BatteryData data = {0};
  
#ifdef BATTERY_ADC
  int adc_value = analogRead(BATTERY_ADC);
  
  float voltage = (adc_value / 4095.0) * 3.3 * 2.0;
  
  if (battery_filtered == 0.0) {
    battery_filtered = voltage;
  } else {
    battery_filtered = (BATTERY_FILTER_ALPHA * voltage) + 
                      ((1.0 - BATTERY_FILTER_ALPHA) * battery_filtered);
  }
  
  data.voltage = battery_filtered;
  
  float percentage = ((battery_filtered - BATTERY_MIN_VOLTAGE) / 
                     (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) * 100.0;
  data.percentage = constrain((int)percentage, 0, 100);
  data.valid = true;
#else
  data.valid = false;
#endif
  
  return data;
}

// ===================================================================
// RFID Helper Functions
// ===================================================================

const char* rfid_get_current_uid() {
  if (last_rfid_data.valid && 
      (millis() - last_rfid_data.last_seen_ms <= RFID_DEDUPLICATE_MS)) {
    return last_rfid_data.uid;
  }
  return nullptr;
}

bool rfid_has_recent_tag() {
  return (last_rfid_data.valid && 
          (millis() - last_rfid_data.last_seen_ms <= RFID_DEDUPLICATE_MS));
}
