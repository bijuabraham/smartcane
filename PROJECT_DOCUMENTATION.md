# Smart Walking Stick - Project Documentation

## Table of Contents
1. [Project Summary](#project-summary)
2. [Features and Capabilities](#features-and-capabilities)
3. [System Design and Architecture](#system-design-and-architecture)
4. [Bill of Materials](#bill-of-materials)
5. [Technical Specifications](#technical-specifications)
6. [Setup and Installation](#setup-and-installation)

---

## Project Summary

The **Smart Walking Stick** is an IoT-enabled assistive device designed to enhance mobility and safety for elderly users and individuals with visual impairments. The system combines advanced sensor technology with real-time monitoring capabilities to provide:

- **Fall Detection**: Automatic detection of falls using 6-axis IMU (accelerometer + gyroscope)
- **Obstacle Avoidance**: Time-of-Flight (ToF) distance sensing for obstacle detection up to 4 meters
- **RFID Tracking**: NFC/RFID tag scanning for location tracking and object identification
- **Emergency SOS**: One-button emergency alert system
- **Real-time Monitoring**: Web-based dashboard with live sensor data visualization
- **Wireless Connectivity**: Bluetooth Low Energy (BLE) communication with mobile devices

The project includes:
1. **ESP32-S3 Firmware**: Production-ready embedded software for the walking stick
2. **Web Dashboard**: Mobile-friendly Progressive Web App for real-time monitoring
3. **Simulator Backend**: Testing environment for development without physical hardware

---

## Features and Capabilities

### Hardware Features

#### 1. **Fall Detection System**
- **Two-stage algorithm** for accurate fall detection:
  - Stage 1: Detects sudden acceleration spike (>3.2g threshold)
  - Stage 2: Confirms fall by detecting stillness after impact
- **Configurable sensitivity** via web dashboard
- **Haptic feedback** (buzzer/vibration) on fall detection
- **Real-time alerts** sent via BLE to connected devices

#### 2. **Obstacle Detection**
- **VL53L1X Time-of-Flight sensor** with 0-4000mm range
- **Adjustable detection threshold** (default: 800mm)
- **Fast response time** (50ms timing budget)
- **Proximity alerts** with haptic feedback
- **Visual distance display** on web dashboard

#### 3. **RFID/NFC Scanning**
- **MFRC522 RFID reader** (13.56 MHz)
- **Tag detection** with UID extraction
- **Location tracking** via pre-programmed RFID tags
- **Object identification** capabilities
- **Deduplication** to prevent repeated scans

#### 4. **Emergency SOS System**
- **Physical SOS button** (GPIO input with debouncing)
- **One-touch emergency alerts**
- **BLE notifications** to connected devices
- **Visual alerts** on web dashboard
- **Alert history tracking**

#### 5. **Battery Monitoring**
- **Real-time voltage monitoring** via ADC
- **Battery percentage calculation** with filtering
- **Low battery alerts** (configurable threshold)
- **Power consumption optimization** with sleep modes

#### 6. **Haptic Feedback**
- **Passive buzzer** with PWM control
- **Vibration motor** support (optional)
- **LED status indicator**
- **Different patterns** for different alert types:
  - Fall: Long vibration pattern
  - Obstacle: Short warning pulses
  - RFID: Quick confirmation beep
  - SOS: Continuous alert pattern

### Software Features

#### Web Dashboard
- **Real-time sensor visualization**
  - IMU acceleration (X, Y, Z axes)
  - Gyroscope data (pitch, roll, yaw)
  - Distance measurements
  - Battery status
- **Interactive charts** with historical data
- **Alert History panel** with color-coded events:
  - 🔴 SOS Alerts
  - 🟠 Fall Detection
  - 🟡 Obstacle Warnings
  - 🟢 RFID Scans
- **Configuration interface**:
  - Sensor update frequency (50-1000ms)
  - Fall threshold adjustment (1.5-4.0g)
  - Obstacle distance threshold (100-2000mm)
- **Dual operation modes**:
  - Hardware mode: Connect to physical ESP32 via Bluetooth
  - Simulator mode: Test with simulated sensor data
- **Mobile-responsive design** for smartphones and tablets
- **PWA capabilities** for offline use

#### Simulator Backend
- **WebSocket-based** real-time communication
- **Realistic sensor data generation**:
  - IMU motion patterns (walking simulation)
  - Random obstacle detection events
  - Periodic RFID scans
  - Randomized fall events for testing
- **Configurable alert frequencies**
- **Development testing** without physical hardware

#### Bluetooth Communication
- **BLE GATT Services** with custom UUIDs
- **Three characteristics**:
  - Sensor Data (notify): Periodic sensor readings
  - Alerts (notify): Event-driven alerts
  - Config (read/write): Device configuration
- **Web Bluetooth API** integration
- **Automatic reconnection** handling
- **Chrome/Edge browser support** (Windows, macOS, Android)

---

## System Design and Architecture

### System Overview

The Smart Walking Stick system consists of three main components:

```
┌─────────────────────────────────────────────────────────────────┐
│                     SMART WALKING STICK SYSTEM                   │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────────┐         ┌──────────────────────┐
│   ESP32-S3 Device    │         │   Web Dashboard      │
│   (Walking Stick)    │◄───────►│   (Browser)          │
│                      │   BLE   │                      │
│  ┌────────────────┐  │         │  ┌────────────────┐  │
│  │ Sensors:       │  │         │  │ UI Components: │  │
│  │ - MPU6050 IMU  │  │         │  │ - Sensor Cards │  │
│  │ - VL53L1X ToF  │  │         │  │ - Live Charts  │  │
│  │ - MFRC522 RFID │  │         │  │ - Alert Panel  │  │
│  │ - SOS Button   │  │         │  │ - Config Modal │  │
│  │ - Battery ADC  │  │         │  └────────────────┘  │
│  └────────────────┘  │         │                      │
│  ┌────────────────┐  │         │  ┌────────────────┐  │
│  │ Firmware:      │  │         │  │ Services:      │  │
│  │ - Fall Detect  │  │         │  │ - Web BLE API  │  │
│  │ - BLE Stack    │  │         │  │ - WebSocket    │  │
│  │ - Haptics      │  │         │  │ - State Mgmt   │  │
│  └────────────────┘  │         │  └────────────────┘  │
└──────────────────────┘         └──────────────────────┘
                                          │
                                          │ WebSocket
                                          │ (Dev/Test)
                                          ▼
                          ┌──────────────────────────┐
                          │  Simulator Backend       │
                          │  (Node.js/Express)       │
                          │                          │
                          │  - Sensor Simulation     │
                          │  - Alert Generation      │
                          │  - Config Storage        │
                          └──────────────────────────┘
```

### Hardware Architecture

#### ESP32-S3 Pin Connections

**I²C Bus (Shared by MPU6050 and VL53L1X):**
```
ESP32-S3          Sensors
GPIO 21 (SDA) ──→ MPU6050 SDA, VL53L1X SDA
GPIO 20 (SCL) ──→ MPU6050 SCL, VL53L1X SCL
3.3V          ──→ VCC/VIN (all I2C sensors)
GND           ──→ GND (all sensors)
```

**SPI Bus (MFRC522 RFID):**
```
ESP32-S3          MFRC522
GPIO 12 (SCK) ──→ SCK
GPIO 13 (MISO)──→ MISO
GPIO 11 (MOSI)──→ MOSI
GPIO 10 (CS)  ──→ CS/SDA
GPIO 14 (RST) ──→ RST
3.3V          ──→ VCC
GND           ──→ GND
```

**User Interface:**
```
ESP32-S3          Component
GPIO 15       ──→ SOS Button (with pull-up)
GPIO 2        ──→ Passive Buzzer (PWM)
GPIO 3        ──→ Vibration Motor (optional)
GPIO 1        ──→ Status LED
GPIO 4 (ADC)  ──→ Battery Voltage Divider
```

### Software Architecture

#### Firmware Structure
```
src/
├── main.cpp              # Main program loop and initialization
├── sensors.cpp/h         # Sensor interface layer
├── fall_detection.cpp/h  # Fall detection algorithm
├── ble.cpp/h            # Bluetooth Low Energy stack
├── haptics.cpp/h        # Haptic feedback control
├── config.h             # System configuration
└── pins.h               # Pin definitions
```

#### Data Flow

**Sensor Reading → Processing → BLE Transmission:**
```
1. Periodic sensor polling (200ms default)
2. IMU data → Fall detection algorithm
3. ToF data → Obstacle threshold check
4. RFID data → UID extraction and deduplication
5. Serialize to JSON format
6. Transmit via BLE notify characteristic
```

**Alert Generation:**
```
Event Detected → Haptic Feedback → BLE Alert → Dashboard Notification
```

#### Web Dashboard Stack
- **Frontend**: React 18 + Vite
- **State Management**: React Hooks (useState, useEffect)
- **Charts**: Recharts library
- **Styling**: Tailwind CSS
- **Icons**: Lucide React
- **Bluetooth**: Web Bluetooth API
- **Communication**: WebSocket (simulator mode)

---

## Bill of Materials

### Core Components

| Component | Description | Quantity | Approx. Price (USD) |
|-----------|-------------|----------|---------------------|
| **ESP32-S3 DevKit** | ESP32-S3-DevKitC-1 or compatible (USB-C) | 1 | $8-15 |
| **MPU6050** | 6-axis IMU (Accelerometer + Gyroscope) breakout board | 1 | $3-5 |
| **VL53L1X** | Time-of-Flight distance sensor (0-4m range) | 1 | $8-12 |
| **MFRC522** | 13.56MHz RFID/NFC reader module | 1 | $3-5 |
| **RFID Tags** | 13.56MHz passive RFID cards/tags | 5-10 | $5-10 (pack) |

### User Interface Components

| Component | Description | Quantity | Approx. Price (USD) |
|-----------|-------------|----------|---------------------|
| **Push Button** | Momentary tactile switch for SOS (6x6mm or larger) | 1 | $0.50-1 |
| **Passive Buzzer** | 5V passive piezo buzzer (3-5kHz) | 1 | $1-2 |
| **Vibration Motor** | Coin vibration motor (3V) - optional | 1 | $2-3 |
| **LED** | 5mm LED for status indication (any color) | 1 | $0.20-0.50 |
| **220Ω Resistor** | For LED current limiting | 1 | $0.10 |
| **10kΩ Resistor** | Pull-up for SOS button (if needed) | 1 | $0.10 |

### Power Components

| Component | Description | Quantity | Approx. Price (USD) |
|-----------|-------------|----------|---------------------|
| **Li-Po Battery** | 3.7V 2000-5000mAh rechargeable (optional) | 1 | $8-15 |
| **Battery Holder** | JST connector or battery case | 1 | $1-2 |
| **Voltage Divider Resistors** | 10kΩ + 10kΩ for battery monitoring | 2 | $0.20 |

### Mechanical Components

| Component | Description | Quantity | Approx. Price (USD) |
|-----------|-------------|----------|---------------------|
| **Walking Stick** | Standard adjustable walking stick/cane | 1 | $10-20 |
| **Enclosure** | Waterproof project box (80x60x30mm or similar) | 1 | $5-10 |
| **Mounting Hardware** | Velcro straps, zip ties, or 3D printed mount | 1 set | $2-5 |

### Wiring and Accessories

| Component | Description | Quantity | Approx. Price (USD) |
|-----------|-------------|----------|---------------------|
| **Jumper Wires** | Male-to-Female, 20cm (various colors) | 20-30 | $2-5 |
| **Breadboard** | For prototyping (optional) | 1 | $3-5 |
| **Perfboard/PCB** | For permanent assembly (optional) | 1 | $2-5 |
| **USB-C Cable** | For programming and power | 1 | $3-5 |
| **Heat Shrink Tubing** | Various sizes for wire protection | 1 set | $3-5 |

### **Total Estimated Cost: $75-130 USD**

(Cost varies based on component quality, shipping, and optional items)

---

## Technical Specifications

### Hardware Specifications

**Microcontroller:**
- **Model**: ESP32-S3 (Dual-core Xtensa LX7, 240 MHz)
- **Flash Memory**: 4MB (minimum)
- **RAM**: 512KB SRAM
- **Connectivity**: Bluetooth 5.0 LE, Wi-Fi 802.11 b/g/n
- **GPIO**: 45 programmable pins
- **ADC**: 12-bit resolution, multiple channels

**Sensors:**

*MPU6050 IMU:*
- 3-axis accelerometer: ±2g, ±4g, ±8g, ±16g (configured: ±8g)
- 3-axis gyroscope: ±250, ±500, ±1000, ±2000 °/s (configured: ±500°/s)
- 16-bit ADCs for digitizing sensor outputs
- I²C interface (address: 0x68 or 0x69)
- Update rate: Up to 1kHz

*VL53L1X ToF Sensor:*
- Range: 0-4000mm (0-4 meters)
- Accuracy: ±3% at distances <1m, ±5% at 1-4m
- Field of View: 27°
- Timing budget: 20-1000ms (configured: 50ms)
- I²C interface (address: 0x29)

*MFRC522 RFID:*
- Frequency: 13.56 MHz
- Supported protocols: ISO/IEC 14443A/MIFARE
- Read distance: Up to 50mm
- SPI interface (up to 10 Mbps)

**Power:**
- Operating voltage: 3.3V (internal)
- USB input: 5V
- Battery input: 3.0-4.2V (Li-Po compatible)
- Typical current: 80-150mA (active), <10mA (sleep)
- Battery life: ~24-48 hours (with 3000mAh battery)

### Software Specifications

**Firmware:**
- Platform: ESP32 Arduino Framework
- BLE Library: NimBLE (optimized for ESP32)
- Build System: PlatformIO
- Language: C++ (Arduino-compatible)
- OTA Updates: Supported (future enhancement)

**Communication Protocol:**
- BLE GATT Service UUID: `12345678-1234-1234-1234-1234567890ab`
- Characteristics:
  - Sensor Data: `...890ad` (Notify, JSON format)
  - Alerts: `...890ae` (Notify, JSON format)
  - Config: `...890af` (Read/Write, JSON format)
- MTU: 512 bytes (configurable)
- Connection interval: 50ms

**Web Dashboard:**
- Framework: React 18.x
- Build tool: Vite 7.x
- Target browsers: Chrome/Edge 89+, Safari 16+ (no BLE)
- Responsive breakpoints: 640px, 768px, 1024px, 1280px
- Update rate: 5-10 Hz (configurable)

**Simulator:**
- Runtime: Node.js 18+
- WebSocket: ws library
- Port: 3001
- Update rate: 5 Hz

---

## Setup and Installation

### Hardware Assembly

1. **Connect I²C Sensors** (MPU6050, VL53L1X):
   - Wire SDA pins to GPIO 8
   - Wire SCL pins to GPIO 9
   - Connect VCC to 3.3V, GND to GND

2. **Connect SPI RFID Reader** (MFRC522):
   - Follow pinout in Bill of Materials
   - Ensure proper power connections

3. **Connect User Interface**:
   - SOS button to GPIO 15 (with pull-up)
   - Buzzer to GPIO 2
   - LED to GPIO 1 (with 220Ω resistor)

4. **Mount to Walking Stick**:
   - Secure enclosure to stick shaft
   - Position ToF sensor pointing forward/downward
   - Ensure SOS button is easily accessible

### Firmware Installation

1. **Install PlatformIO**:
   - Download VS Code + PlatformIO extension
   - Or use PlatformIO CLI

2. **Upload Firmware**:
   ```bash
   platformio run --target upload --environment esp32-s3-devkitc-1
   ```

3. **Monitor Serial Output**:
   ```bash
   platformio device monitor
   ```
   - Verify all sensors initialize correctly
   - Confirm BLE advertising starts

### Web Dashboard Setup

1. **Install Dependencies**:
   ```bash
   cd web-dashboard
   npm install
   ```

2. **Start Development Server**:
   ```bash
   npm run dev
   ```
   - Access at `http://localhost:5173`

3. **Connect to Device**:
   - Switch to "Hardware" mode
   - Click "Connect" button
   - Select "SmartStick" from Bluetooth devices
   - Grant browser permissions

### Testing with Simulator

1. **Start Simulator Backend**:
   ```bash
   cd simulator-backend
   npm install
   npm start
   ```

2. **Connect Dashboard**:
   - Switch to "Simulator" mode in dashboard
   - Click "Connect"
   - See simulated sensor data and alerts

---

## Usage Guidelines

### Fall Detection Sensitivity

Adjust fall threshold based on user needs:
- **Higher values (3.0-4.0g)**: Less sensitive, fewer false positives
- **Lower values (1.5-2.5g)**: More sensitive, may trigger on sudden movements
- **Recommended**: 2.5-3.0g for most users

### Obstacle Detection Range

Configure distance threshold:
- **Short range (300-600mm)**: Indoor use, tight spaces
- **Medium range (600-1000mm)**: General purpose
- **Long range (1000-2000mm)**: Outdoor use, advance warning

### Battery Maintenance

- Charge when battery drops below 20%
- Avoid complete discharge
- Store at 50-70% charge if unused for extended periods
- Replace battery after ~500 charge cycles

---

## Future Enhancements

- GPS tracking for outdoor location monitoring
- Machine learning for improved fall detection accuracy
- Multi-user support with cloud backend
- Voice feedback and audio alerts
- Integration with smart home systems
- Companion mobile app (iOS/Android)
- Data analytics and health insights
- Emergency contact auto-notification via cellular/WiFi

---

## License and Credits

**Project**: Smart Walking Stick  
**Version**: 1.0.0  
**Date**: November 2025  

**Technologies Used**:
- ESP32-S3 (Espressif Systems)
- Arduino Framework
- NimBLE Stack
- React + Vite
- Recharts, Tailwind CSS
- Web Bluetooth API

---

**End of Documentation**
