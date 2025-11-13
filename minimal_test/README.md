# VL53L1X Minimal Test

This is a standalone minimal test to verify your VL53L1X sensor is working.

## How to Use:

### Option 1: Replace your main.cpp temporarily

1. **Backup your current main.cpp**
2. Copy `minimal_test/main.cpp` to your project's `src/main.cpp`
3. Upload to ESP32
4. Open Serial Monitor

### Option 2: Create new PlatformIO project

1. Create a new folder called `vl53_test`
2. Copy `minimal_test/main.cpp` to `vl53_test/src/main.cpp`
3. Copy `minimal_test/platformio.ini` to `vl53_test/platformio.ini`
4. Open in PlatformIO and upload

## Expected Output (SUCCESS):

```
=== VL53L1X MINIMAL TEST ===
I2C: GPIO 21 (SDA), GPIO 20 (SCL)
I2C initialized
SUCCESS: VL53L1X FOUND!
Ready! Reading distance...

Distance: 1234 mm
Distance: 1235 mm
Distance: 1230 mm
```

## Expected Output (FAILURE - Wiring Problem):

```
=== VL53L1X MINIMAL TEST ===
I2C: GPIO 21 (SDA), GPIO 20 (SCL)
I2C initialized
ERROR: VL53L1X NOT FOUND!
Check wiring:
  VL53L1X VIN -> ESP32 3.3V
  VL53L1X GND -> ESP32 GND
  VL53L1X SDA -> ESP32 GPIO 21
  VL53L1X SCL -> ESP32 GPIO 20
```

## Wiring:

```
VL53L1X    ESP32-S3
VIN    ->  3.3V
GND    ->  GND
SDA    ->  GPIO 21
SCL    ->  GPIO 20
```
