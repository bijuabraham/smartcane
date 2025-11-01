#include "haptics.h"
#include "pins.h"

// ===================================================================
// Haptics State Variables
// ===================================================================

static unsigned long led_blink_end = 0;
static unsigned long vib_end = 0;
static unsigned long buzzer_end = 0;

// ===================================================================
// Haptics Initialization
// ===================================================================

void haptics_init() {
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(VIB_MOTOR, OUTPUT);
  
  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(VIB_MOTOR, LOW);
  
  Serial.println("Haptics: Initialized");
}

// ===================================================================
// Haptics Update (handle timing)
// ===================================================================

void haptics_update() {
  unsigned long now = millis();
  
  if (led_blink_end > 0 && now >= led_blink_end) {
    digitalWrite(LED, LOW);
    led_blink_end = 0;
  }
  
  if (vib_end > 0 && now >= vib_end) {
    digitalWrite(VIB_MOTOR, LOW);
    vib_end = 0;
  }
  
  if (buzzer_end > 0 && now >= buzzer_end) {
    digitalWrite(BUZZER, LOW);
    buzzer_end = 0;
  }
}

// ===================================================================
// Haptics Trigger
// ===================================================================

void haptics_trigger(HapticEvent event) {
  unsigned long now = millis();
  
  switch (event) {
    case HAPTIC_SOS:
      digitalWrite(LED, HIGH);
      led_blink_end = now + 500;
      
      digitalWrite(BUZZER, HIGH);
      buzzer_end = now + 200;
      
      digitalWrite(VIB_MOTOR, HIGH);
      vib_end = now + 200;
      
      Serial.println("Haptics: SOS triggered");
      break;
      
    case HAPTIC_FALL:
      digitalWrite(LED, HIGH);
      led_blink_end = now + 1000;
      
      digitalWrite(BUZZER, HIGH);
      buzzer_end = now + 300;
      
      digitalWrite(VIB_MOTOR, HIGH);
      vib_end = now + 200;
      
      Serial.println("Haptics: Fall alert triggered");
      break;
      
    case HAPTIC_OBSTACLE:
      digitalWrite(BUZZER, HIGH);
      buzzer_end = now + 100;
      
      digitalWrite(VIB_MOTOR, HIGH);
      vib_end = now + 100;
      
      Serial.println("Haptics: Obstacle alert triggered");
      break;
      
    case HAPTIC_RFID:
      digitalWrite(LED, HIGH);
      led_blink_end = now + 200;
      
      digitalWrite(BUZZER, HIGH);
      buzzer_end = now + 50;
      
      Serial.println("Haptics: RFID detected");
      break;
  }
}

// ===================================================================
// LED Control
// ===================================================================

void haptics_led_set(bool state) {
  digitalWrite(LED, state ? HIGH : LOW);
  led_blink_end = 0;
}

void haptics_led_blink(uint16_t duration_ms) {
  digitalWrite(LED, HIGH);
  led_blink_end = millis() + duration_ms;
}
