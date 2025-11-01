#!/usr/bin/env python3
"""
Smart Walking Stick Firmware - Project Validator
Validates that all required firmware files are present and displays project information.
"""

import os
import sys
from pathlib import Path

REQUIRED_FILES = [
    "src/main.cpp",
    "src/pins.h",
    "src/config.h",
    "src/ble.h",
    "src/ble.cpp",
    "src/sensors.h",
    "src/sensors.cpp",
    "src/fall_detection.h",
    "src/fall_detection.cpp",
    "src/haptics.h",
    "src/haptics.cpp",
    "platformio.ini",
    "README.md"
]

def print_banner():
    print("\n" + "="*60)
    print("  Smart Walking Stick Firmware - Project Validator")
    print("  ESP32-S3 + NimBLE")
    print("="*60 + "\n")

def validate_files():
    print("📋 Checking required files...\n")
    
    all_present = True
    for file_path in REQUIRED_FILES:
        exists = Path(file_path).exists()
        status = "✅" if exists else "❌"
        print(f"  {status} {file_path}")
        
        if not exists:
            all_present = False
    
    print()
    return all_present

def get_file_info():
    print("📊 Project Statistics:\n")
    
    total_lines = 0
    for file_path in REQUIRED_FILES:
        if Path(file_path).exists() and file_path.endswith(('.cpp', '.h')):
            with open(file_path, 'r') as f:
                lines = len(f.readlines())
                total_lines += lines
    
    print(f"  Total lines of code: {total_lines}")
    print(f"  Source files: {len([f for f in REQUIRED_FILES if f.endswith(('.cpp', '.h'))])}")
    print()

def show_compilation_guide():
    print("🔧 Compilation Guide:\n")
    print("  This is Arduino firmware for ESP32-S3 hardware.")
    print("  It CANNOT run on this Replit environment.\n")
    print("  To compile and upload:\n")
    print("  Option 1 - PlatformIO (Recommended):")
    print("    1. Install PlatformIO: https://platformio.org/install")
    print("    2. Run: pio run")
    print("    3. Upload: pio run --target upload\n")
    print("  Option 2 - Arduino IDE:")
    print("    1. Install Arduino IDE 2.0+")
    print("    2. Install ESP32 board support")
    print("    3. Install required libraries (see README.md)")
    print("    4. Select 'ESP32S3 Dev Module' board")
    print("    5. Upload sketch\n")
    print("  📖 See README.md for detailed instructions\n")

def show_features():
    print("✨ Features:\n")
    features = [
        "Fall Detection (MPU6050 IMU)",
        "Obstacle Detection (VL53L1X ToF)",
        "RFID Tracking (MFRC522)",
        "SOS Emergency Button",
        "Battery Monitoring",
        "BLE GATT Interface (NimBLE)",
        "Haptic Feedback (Buzzer, Vibration, LED)"
    ]
    for feature in features:
        print(f"  • {feature}")
    print()

def main():
    print_banner()
    
    files_ok = validate_files()
    
    if files_ok:
        print("✅ All required files are present!\n")
        get_file_info()
        show_features()
        show_compilation_guide()
        print("="*60)
        print("✅ Project validation successful!")
        print("="*60 + "\n")
        return 0
    else:
        print("❌ Some required files are missing!")
        print("   Please ensure all firmware files are present.\n")
        return 1

if __name__ == "__main__":
    sys.exit(main())
