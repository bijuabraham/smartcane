#ifndef HAPTICS_H
#define HAPTICS_H

#include <Arduino.h>

// ===================================================================
// Passive Buzzer Configuration
// ===================================================================
// RLECS Passive Buzzer Module (Amazon B07XDPXH7K)
// Requires PWM signal at 2-5kHz to generate tones
// Different frequencies for different alert types

#define BUZZER_PWM_CHANNEL 0
#define BUZZER_PWM_RESOLUTION 8  // 8-bit resolution (0-255)

// Alert tone frequencies (Hz)
#define TONE_SOS       3000   // High-pitched urgent tone
#define TONE_FALL      2500   // High-medium urgent tone
#define TONE_OBSTACLE  2000   // Medium warning tone
#define TONE_RFID      3500   // High confirmation beep

// ===================================================================
// Haptics Event Types
// ===================================================================

enum HapticEvent {
  HAPTIC_SOS,
  HAPTIC_FALL,
  HAPTIC_OBSTACLE,
  HAPTIC_RFID
};

// ===================================================================
// Haptics Functions
// ===================================================================

void haptics_init();
void haptics_update();
void haptics_trigger(HapticEvent event);
void haptics_led_set(bool state);
void haptics_led_blink(uint16_t duration_ms);

#endif // HAPTICS_H
