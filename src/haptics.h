#ifndef HAPTICS_H
#define HAPTICS_H

#include <Arduino.h>

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
