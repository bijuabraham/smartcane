# Smart Walking Stick Firmware

Production-ready Arduino firmware for ESP32-S3 dev board that powers an intelligent walking stick with fall detection, obstacle avoidance, and RFID tracking capabilities.

## Features

- **Fall Detection**: MPU6050 IMU with intelligent fall detection algorithm
- **Obstacle Detection**: VL53L1X Time-of-Flight sensor for obstacle avoidance
- **RFID Tracking**: MFRC522 reader for location/checkpoint tracking
- **SOS Alert**: Emergency button with immediate notification
- **Battery Monitoring**: Real-time battery level tracking (optional)
- **BLE Interface**: NimBLE GATT server for wireless data streaming and configuration
- **Haptic Feedback**: Buzzer, vibration motor, and LED indicators

## Hardware Requirements

### Main Components
- **MCU**: ESP32-S3 Dev Module (no camera variant)
- **Power**: Single-cell Li-ion battery (3.7V) via onboard regulator

### Sensors (3.3V)
- **IMU**: MPU6050 (I2C)
- **Time-of-Flight**: VL53L1X (I2C)
- **RFID**: MFRC522 (SPI)

### User I/O
- SOS pushbutton (active LOW, internal pullup)
- Piezo buzzer
- Vibration motor
- Status LED
- Optional: Battery voltage divider for ADC monitoring

## Pin Configuration

Default pin assignments are defined in `src/pins.h`. **IMPORTANT**: Verify these match your specific ESP32-S3 dev board!

```cpp
// I2C (MPU6050, VL53L1X)
I2C_SDA = 8
I2C_SCL = 9

// SPI (MFRC522)
SPI_SCK = 12, SPI_MISO = 13, SPI_MOSI = 11
RFID_CS = 10, RFID_RST = 14

// User I/O
SOS_BTN = 15, BUZZER = 2, VIB_MOTOR = 3, LED = 1

// Battery (optional)
BATTERY_ADC = 4
```

## BLE GATT Interface

### Device Name
`SmartStick`

### Service UUID
`12345678-1234-1234-1234-1234567890ab`

### Characteristics

#### 1. SENSOR_DATA (Notify, Read)
**UUID**: `12345678-1234-1234-1234-1234567890ad`

Periodic JSON sensor data:
```json
{
  "ts": 123456,
  "imu": {"ax": 0.05, "ay": 0.02, "az": 1.01, "gx": 0.5, "gy": -0.3, "gz": 0.1},
  "dist_mm": 1250,
  "rfid": "A1B2C3D4",
  "battery": {"v": 3.85, "pct": 75}
}
```

#### 2. ALERTS (Notify, Read)
**UUID**: `12345678-1234-1234-1234-1234567890ae`

Event-driven JSON alerts:
```json
{"event":"SOS_BUTTON_PRESSED"}
{"event":"FALL_DETECTED","severity":"high","ax":2.5,"ay":0.1,"az":0.3}
{"event":"OBSTACLE_NEAR","dist_mm":420}
{"event":"RFID_SEEN","uid":"A1B2C3D4"}
```

#### 3. CONFIG (Write, Read)
**UUID**: `12345678-1234-1234-1234-1234567890af`

Configuration updates (write JSON):
```json
{
  "sensor_period_ms": 200,
  "obstacle_threshold_mm": 800,
  "fall_ax_threshold": 2.2,
  "ble_tx_power": 7
}
```

Response:
```json
{"ok": true}
```
or
```json
{"ok": false, "err": "Validation failed"}
```

## Compilation & Upload

### Option 1: PlatformIO (Recommended)

1. **Install PlatformIO**: 
   - [VSCode Extension](https://platformio.org/install/ide?install=vscode)
   - Or CLI: `pip install platformio`

2. **Open Project**:
   ```bash
   cd smart-walking-stick
   pio run
   ```

3. **Upload to Board**:
   ```bash
   pio run --target upload
   ```

4. **Monitor Serial Output**:
   ```bash
   pio device monitor
   ```

### Option 2: Arduino IDE

1. **Install Arduino IDE** (2.0 or later)

2. **Install ESP32 Board Support**:
   - File → Preferences
   - Add to Additional Boards Manager URLs:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager → Search "ESP32" → Install

3. **Select Board**:
   - Tools → Board → ESP32 Arduino → ESP32S3 Dev Module

4. **Install Required Libraries** (Tools → Manage Libraries):
   - Adafruit MPU6050
   - Adafruit VL53L1X
   - MFRC522
   - ArduinoJson (v6.x)
   - NimBLE-Arduino

5. **Configure Upload Settings**:
   - USB CDC On Boot: Enabled
   - Flash Mode: QIO 80MHz
   - Flash Size: 4MB (or your board's size)
   - Partition Scheme: Default 4MB
   - Upload Speed: 921600

6. **Compile & Upload**:
   - Sketch → Upload

## Configuration

### Enable Low-Power Mode

In `src/config.h`, uncomment:
```cpp
#define LOW_POWER
```

This enables light-sleep between sensor cycles and reduces BLE connection interval.

### Disable Battery Monitoring

In `src/pins.h`, comment out:
```cpp
// #define BATTERY_ADC 4
```

### Adjust Pin Assignments

Edit `src/pins.h` to match your board's pinout.

## Project Structure

```
├── src/
│   ├── main.cpp              # Main loop and state machine
│   ├── pins.h                # Pin definitions
│   ├── config.h              # Configuration and constants
│   ├── ble.h/.cpp            # NimBLE GATT server
│   ├── sensors.h/.cpp        # Sensor drivers (IMU, ToF, RFID, Battery)
│   ├── fall_detection.h/.cpp # Fall detection algorithm
│   └── haptics.h/.cpp        # Haptics control (LED, buzzer, vibration)
├── platformio.ini            # PlatformIO configuration
└── README.md                 # This file
```

## Testing

### Serial Monitor Output

After uploading, open the serial monitor (115200 baud) to see:
- Initialization status
- Sensor readings
- Fall detection events
- RFID detections
- BLE connection status

### BLE Testing

Use a BLE scanner app (e.g., nRF Connect) to:
1. Scan for "SmartStick"
2. Connect and discover services
3. Enable notifications on SENSOR_DATA and ALERTS
4. Write configuration to CONFIG characteristic

## Troubleshooting

### Sensor Initialization Fails
- Check I2C wiring (SDA, SCL)
- Verify sensor addresses (MPU6050: 0x68, VL53L1X: 0x29)
- Ensure 3.3V power supply is stable

### RFID Not Detecting
- Check SPI wiring (SCK, MISO, MOSI, CS, RST)
- Ensure MFRC522 has adequate 3.3V power (peak 50mA)
- Try different RFID cards/tags

### BLE Not Advertising
- Check serial output for initialization errors
- Ensure NimBLE-Arduino library is installed
- Try restarting the board

### Fall Detection Too Sensitive
- Adjust `fall_ax_threshold` via BLE CONFIG characteristic
- Default is 2.2g; increase for less sensitivity

## License

This firmware is provided as-is for educational and development purposes.

## Support

For issues or questions, check the serial monitor output for debug information.
