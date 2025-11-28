# Smart Walking Stick Firmware

## Project Overview
Production-ready Arduino firmware for ESP32-S3 microcontroller that powers an intelligent walking stick with advanced safety features including fall detection, obstacle avoidance, RFID tracking, and emergency SOS alerts. The firmware uses NimBLE for wireless communication and supports real-time sensor data streaming over BLE.

## Project Type
**Full-Stack Embedded IoT System** 

### Components:
1. **ESP32-S3 Firmware** - Arduino C++ for ESP32-S3 microcontroller (must be compiled separately)
2. **Backend Simulator** - Node.js/WebSocket service for testing without hardware
3. **Web Dashboard** - React + Vite mobile-responsive monitoring interface with Web Bluetooth API

**IMPORTANT**: The firmware code must be compiled with PlatformIO or Arduino IDE and uploaded to ESP32-S3 hardware. However, the web dashboard can be tested immediately using the built-in simulator.

## Current State
✅ **Complete and production-ready**

### ESP32-S3 Firmware (1,168 lines)
- Main application loop and state machine
- NimBLE GATT server with three characteristics
- Sensor drivers (IMU, ToF, RFID, Battery)
- Fall detection algorithm
- Haptics control system
- Configurable parameters via BLE

### Backend Simulator (Running on port 3001)
- Realistic sensor data generation (MPU6050, VL53L1X, battery)
- Simulated events (fall detection, obstacles, RFID scans)
- WebSocket-based real-time streaming
- Configuration management matching hardware behavior

### Web Dashboard (Running on port 5000)
- Mobile-responsive React interface with TailwindCSS v4
- Mode toggle: **Hardware** (Web Bluetooth) ↔ **Simulator** (WebSocket)
- Real-time sensor visualization with charts
- Alert notifications system
- Device configuration panel
- Production-ready with proper error handling

## Architecture

### Hardware Components
- **MCU**: ESP32-S3 Dev Module
- **Sensors**: MPU6050 (IMU), VL53L1X (ToF), MFRC522 (RFID)
- **User I/O**: SOS button, passive buzzer module (PWM), vibration motor, LED
- **Power**: Li-ion battery with optional voltage monitoring
- **Buzzer**: RLECS B07XDPXH7K passive buzzer module (requires PWM, 2-5kHz)

### Software Architecture

```
main.cpp (Main Loop)
├── ble.cpp (BLE GATT Server)
│   ├── SENSOR_DATA characteristic (notify/read)
│   ├── ALERTS characteristic (notify/read)
│   └── CONFIG characteristic (write/read)
├── sensors.cpp (Sensor Drivers)
│   ├── MPU6050 IMU
│   ├── VL53L1X ToF
│   ├── MFRC522 RFID
│   └── Battery ADC
├── fall_detection.cpp (Fall Detection)
│   ├── Acceleration spike detection
│   └── Stillness confirmation
└── haptics.cpp (Feedback Control)
    ├── LED control
    ├── Buzzer control
    └── Vibration motor control
```

### Key Features
1. **Fall Detection**: Detects acceleration spikes >2.2g followed by stillness
2. **Obstacle Avoidance**: Alerts when objects are <800mm away
3. **RFID Tracking**: Detects and deduplicates RFID tags (3-second window)
4. **SOS Button**: Emergency alert with debouncing
5. **Battery Monitor**: Voltage tracking with low-pass filtering
6. **BLE Interface**: Real-time data streaming and remote configuration
7. **Haptic Feedback**: Multi-modal alerts (LED, buzzer, vibration)

## Configuration

### Pin Definitions (src/pins.h)
All pin assignments are in `src/pins.h` and **must be verified** against your specific ESP32-S3 dev board before uploading.

### Runtime Configuration (via BLE)
- `sensor_period_ms`: Sensor sampling rate (100-1000ms, default: 200ms)
- `obstacle_threshold_mm`: Obstacle alert distance (200-2000mm, default: 800mm)
- `fall_ax_threshold`: Fall impact threshold (0.5-5.0g, default: 2.2g)
- `fall_motion_threshold`: Stillness threshold (0.1-1.0g, default: 0.3g)
- `fall_stillness_ms`: Stillness duration required (200-5000ms, default: 1000ms)
- `ble_tx_power`: BLE transmission power (-12 to 9 dBm, default: 7dBm)

### Compile-Time Options (src/config.h)
- `LOW_POWER`: Enable light-sleep mode for battery conservation

## Dependencies

### Arduino Libraries (install via Library Manager or PlatformIO)
- ESP32 Arduino Core (v2.0.x+)
- NimBLE-Arduino (v1.4.1+)
- Adafruit MPU6050 (v2.2.4+)
- Adafruit VL53L1X (v3.1.0+)
- MFRC522 (v1.4.10+)
- ArduinoJson (v6.21.3+)

See `platformio.ini` for complete dependency list.

## Compilation Instructions

### PlatformIO (Recommended)
```bash
# Install PlatformIO
pip install platformio

# Compile
pio run

# Upload to board
pio run --target upload

# Monitor serial output
pio device monitor
```

### Arduino IDE
1. Install Arduino IDE 2.0+
2. Add ESP32 board support URL to preferences
3. Install board: ESP32S3 Dev Module
4. Install all required libraries
5. Open `src/main.cpp` as sketch
6. Select board and upload

See `README.md` for detailed step-by-step instructions.

## BLE Protocol

### Service UUID
`12345678-1234-1234-1234-1234567890ab`

### Characteristics

**SENSOR_DATA** (UUID: ...ad) - Notify/Read
- Periodic JSON sensor readings
- Includes IMU, ToF, RFID, battery data
- Update rate: configurable (default 200ms)

**ALERTS** (UUID: ...ae) - Notify/Read  
- Event-driven JSON alerts
- Events: SOS, FALL_DETECTED, OBSTACLE_NEAR, RFID_SEEN

**CONFIG** (UUID: ...af) - Write/Read
- Write JSON to update parameters
- Returns `{"ok":true}` or error message

## Testing

### Serial Monitor (115200 baud)
- Initialization status for all sensors
- Real-time sensor readings
- Fall detection events
- RFID detections
- BLE connection status

### BLE Testing
Use nRF Connect or similar BLE scanner:
1. Scan for "SmartStick"
2. Connect and discover services
3. Enable notifications on SENSOR_DATA and ALERTS
4. Write configuration updates to CONFIG

## Recent Changes
- 2025-11-28: Fall detection calibration feature added
  - New calibration mode records actual fall data for threshold tuning
  - Dashboard calibration button opens recording wizard
  - Records peak acceleration (impact spike) and minimum motion (stillness)
  - Suggests optimal threshold values based on recorded data
  - One-click application of suggested values to device config
  - Simulator supports calibration testing without hardware
  - BLE CALIBRATION characteristic (UUID: ...b0) for command/response
- 2025-11-01: Backend simulator and mode toggle added
  - Created Node.js WebSocket simulator service for testing without hardware
  - Implemented realistic sensor data generation matching firmware format
  - Added mode toggle in web dashboard (Hardware vs Simulator)
  - Simulator generates walking patterns, obstacles, falls, and RFID events
  - Allows full testing of dashboard before hardware assembly
- 2025-11-01: Web dashboard production-ready
  - Fixed Tailwind CSS v4 configuration (`@import "tailwindcss"`)
  - Configured Vite allowedHosts for Replit environment
  - Fixed configuration persistence with nullish coalescing (`??`)
  - Mobile-responsive design fully tested
- 2025-11-01: Passive buzzer integration (RLECS B07XDPXH7K)
  - Updated haptics module to use PWM control (ESP32 LEDC peripheral)
  - Implemented distinct tones for different alert types:
    - SOS: 3000 Hz, Fall: 2500 Hz, Obstacle: 2000 Hz, RFID: 3500 Hz
  - Allows audible distinction between alert types without visual feedback
  - Uses LEDC channel 0, 8-bit resolution
  - Documentation updated with buzzer specifications
- 2025-11-01: Initial firmware implementation complete
  - All sensor drivers implemented
  - BLE GATT server with three characteristics
  - Fall detection algorithm
  - Haptics control system
  - Configuration system
  - Documentation and build files

## Project Structure
```
├── src/                         # ESP32-S3 Firmware
│   ├── main.cpp                 # Main loop and state machine
│   ├── pins.h                   # Pin definitions
│   ├── config.h                 # Configuration and constants
│   ├── ble.h/.cpp               # NimBLE GATT server
│   ├── sensors.h/.cpp           # Sensor drivers
│   ├── fall_detection.h/.cpp    # Fall detection algorithm
│   └── haptics.h/.cpp           # Haptics control
├── simulator-backend/           # Testing Simulator Service
│   ├── server.js                # WebSocket server (port 3001)
│   ├── simulator.js             # Sensor data generator
│   └── package.json             # Node.js dependencies
├── web-dashboard/               # Web Monitoring Interface
│   ├── src/
│   │   ├── App.jsx              # Main application
│   │   ├── components/          # Dashboard, ConfigModal
│   │   ├── services/            # bluetooth.js, simulator.js
│   │   └── hooks/               # useSmartStick.js
│   ├── vite.config.js           # Vite configuration
│   ├── tailwind.config.js       # Tailwind CSS setup
│   └── package.json             # React dependencies
├── platformio.ini               # PlatformIO configuration
├── validate_project.py          # Project validation script
├── README.md                    # User documentation
└── replit.md                    # This file
```

## How to Use the Simulator

### Quick Start (No Hardware Required)
1. **Open the web dashboard** in your browser (view the Webview pane in Replit)
2. The mode toggle defaults to **Simulator** mode (purple)
3. Click **Connect** to start receiving simulated sensor data
4. Watch real-time charts, alerts, and sensor readings
5. Test configuration changes in the Config panel

### Switching to Hardware Mode
1. Click the **Hardware** button in the mode toggle (blue)
2. Click **Connect** to pair with your ESP32-S3 via Web Bluetooth
3. Make sure Bluetooth is enabled and supported in your browser (Chrome, Edge, Opera)

### Testing Features
The simulator automatically generates:
- **Walking patterns**: Realistic IMU data with step cycles
- **Obstacle events**: Random obstacles < 800mm (configurable)
- **Fall detection**: Occasional high-acceleration events
- **RFID scans**: Random RFID tag detections
- **Battery drain**: Gradual battery percentage decrease

## Notes
- The firmware is designed for ESP32-S3 hardware and must be compiled separately
- Pin assignments must be verified before uploading to hardware
- Battery monitoring requires voltage divider circuit (2:1 ratio recommended)
- Fall detection sensitivity can be tuned via BLE for different use cases
- Low-power mode reduces power consumption for battery operation
- Web Bluetooth API requires HTTPS or localhost (works in Replit webview)
- The simulator matches the exact JSON format of the hardware for seamless testing
