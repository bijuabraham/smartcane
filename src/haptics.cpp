#include "haptics.h"
#include "pins.h"

// ===================================================================
// Haptics State Variables
// ===================================================================

static unsigned long led_blink_end = 0;
static unsigned long vib_end = 0;
static unsigned long buzzer_end = 0;
static uint16_t current_buzzer_freq = 0;

// ===================================================================
// Passive Buzzer Helper Functions
// ===================================================================

static void buzzer_tone(uint16_t frequency) {
  if (frequency > 0) {
    ledcWriteTone(BUZZER_PWM_CHANNEL, frequency);
    current_buzzer_freq = frequency;
  }
}

static void buzzer_stop() {
  ledcWriteTone(BUZZER_PWM_CHANNEL, 0);
  current_buzzer_freq = 0;
}

// ===================================================================
// Haptics Initialization
// ===================================================================

void haptics_init() {
  pinMode(LED, OUTPUT);
  pinMode(VIB_MOTOR, OUTPUT);
  
  digitalWrite(LED, LOW);
  digitalWrite(VIB_MOTOR, LOW);
  
  // Configure PWM for passive buzzer
  ledcSetup(BUZZER_PWM_CHANNEL, 2000, BUZZER_PWM_RESOLUTION);
  ledcAttachPin(BUZZER, BUZZER_PWM_CHANNEL);
  buzzer_stop();
  
  Serial.println("Haptics: Initialized (Passive Buzzer with PWM)");
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
    buzzer_stop();
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
      // Urgent high-pitched tone for emergency
      digitalWrite(LED, HIGH);
      led_blink_end = now + 500;
      
      buzzer_tone(TONE_SOS);
      buzzer_end = now + 200;
      
      digitalWrite(VIB_MOTOR, HIGH);
      vib_end = now + 200;
      
      Serial.println("Haptics: SOS triggered (3000 Hz)");
      break;
      
    case HAPTIC_FALL:
      // High-medium urgent tone for fall detection
      digitalWrite(LED, HIGH);
      led_blink_end = now + 1000;
      
      buzzer_tone(TONE_FALL);
      buzzer_end = now + 300;
      
      digitalWrite(VIB_MOTOR, HIGH);
      vib_end = now + 200;
      
      Serial.println("Haptics: Fall alert triggered (2500 Hz)");
      break;
      
    case HAPTIC_OBSTACLE:
      // Medium warning tone for obstacle detection
      buzzer_tone(TONE_OBSTACLE);
      buzzer_end = now + 100;
      
      digitalWrite(VIB_MOTOR, HIGH);
      vib_end = now + 100;
      
      Serial.println("Haptics: Obstacle alert triggered (2000 Hz)");
      break;
      
    case HAPTIC_RFID:
      // High confirmation beep for RFID detection
      digitalWrite(LED, HIGH);
      led_blink_end = now + 200;
      
      buzzer_tone(TONE_RFID);
      buzzer_end = now + 50;
      
      Serial.println("Haptics: RFID detected (3500 Hz)");
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
