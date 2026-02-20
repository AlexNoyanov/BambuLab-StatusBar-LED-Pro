#ifndef SOUND_ALERTING_H
#define SOUND_ALERTING_H

#include "config.h"
#include "PrinterState.h"

// ===============================
// SOUND CONFIGURATION
// ===============================
#ifndef BUZZER_PIN
  #define BUZZER_PIN 4  // Default buzzer pin (change in config.h if needed)
#endif

// ESP8266: D6 on NodeMCU/Wemos = GPIO 12 (not 6). Map D6 -> 12 so wiring "D6" works.
#ifdef ESP8266
  #if BUZZER_PIN == 6
    #undef BUZZER_PIN
    #define BUZZER_PIN 12
  #endif
#endif

#ifndef SOUND_ENABLED
  #define SOUND_ENABLED true  // Set to false to disable sound
#endif

// Musical note frequencies (Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760

// Note durations
#define WHOLE_NOTE    1000
#define HALF_NOTE     500
#define QUARTER_NOTE  250
#define EIGHTH_NOTE   125
#define SIXTEENTH_NOTE 62
#define BEEP_SHORT 60   // Short tech beep (ms)
#define BEEP_TINY  35   // Tiny blip (ms)
#define GAP_SHORT  40   // Short gap between beeps (ms)
#define NOTE_REST 0

// Global variables
bool soundPlayed = false;
unsigned long soundStartTime = 0;
int currentNoteIndex = 0;

// ===============================
// HAPPY FINISH MELODY
// ===============================
// A cheerful, celebratory melody for finished prints
// Based on a happy ascending scale with a triumphant ending

struct Note {
  int frequency;
  int duration;
};

// Happy finish melody - cheerful and uplifting
const Note finishMelody[] = {
  {NOTE_C5, EIGHTH_NOTE},   // Do
  {NOTE_E5, EIGHTH_NOTE},   // Mi
  {NOTE_G5, EIGHTH_NOTE},   // Sol
  {NOTE_C6, QUARTER_NOTE},  // Do (high) - triumphant!
  {NOTE_G5, EIGHTH_NOTE},   // Sol
  {NOTE_E5, EIGHTH_NOTE},   // Mi
  {NOTE_C5, QUARTER_NOTE},  // Do
  {NOTE_G5, QUARTER_NOTE},  // Sol
  {NOTE_C6, HALF_NOTE},     // Do (high) - celebration!
  {NOTE_REST, EIGHTH_NOTE}, // Rest
  {NOTE_C5, EIGHTH_NOTE},   // Do
  {NOTE_D5, EIGHTH_NOTE},   // Re
  {NOTE_E5, EIGHTH_NOTE},   // Mi
  {NOTE_F5, EIGHTH_NOTE},   // Fa
  {NOTE_G5, EIGHTH_NOTE},   // Sol
  {NOTE_A5, EIGHTH_NOTE},   // La
  {NOTE_B5, EIGHTH_NOTE},   // Si
  {NOTE_C6, HALF_NOTE},     // Do (high) - final triumph!
  {NOTE_REST, QUARTER_NOTE} // Rest
};

#define FINISH_MELODY_LENGTH (sizeof(finishMelody) / sizeof(finishMelody[0]))

// ===============================
// STARTUP MELODY (Boot complete) - short tune, then silence
// ===============================
// Uses playTone so passive buzzers hear real notes: C5 -> E5 -> G5 -> C6

#define BOOT_NOTE_MS   180   // ms per note
#define BOOT_GAP_MS    80    // ms between notes
#define BOOT_LAST_MS   280   // ms final note
#define BOOT_SILENCE_MS 400  // ms silence at end (pin LOW)

// ===============================
// SOUND FUNCTIONS
// ===============================

void playTone(int frequency, int duration);  // Forward declaration
void stopSound();                            // Forward declaration

void initSound() {
  if (!SOUND_ENABLED) return;
  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("Sound module initialized");
}

void playStartupMelody() {
  if (!SOUND_ENABLED) return;
  
  Serial.println("🔊 Boot melody...");
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Ascending melody: C5 - E5 - G5 - C6 (boot complete)
  playTone(NOTE_C5, BOOT_NOTE_MS);
  delay(BOOT_GAP_MS);
  yield();
  
  playTone(NOTE_E5, BOOT_NOTE_MS);
  delay(BOOT_GAP_MS);
  yield();
  
  playTone(NOTE_G5, BOOT_NOTE_MS);
  delay(BOOT_GAP_MS);
  yield();
  
  playTone(NOTE_C6, BOOT_LAST_MS);
  
  stopSound();
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  delay(BOOT_SILENCE_MS);
  
  Serial.println("✅ Boot complete");
}

void playTone(int frequency, int duration) {
  if (!SOUND_ENABLED || frequency == NOTE_REST) {
    delay(duration);
    return;
  }
  
#ifdef ESP8266
  tone(BUZZER_PIN, frequency, duration);
  delay(duration);
  noTone(BUZZER_PIN);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  delay(2);
#elif defined(ESP32)
  static bool ledcReady = false;
  if (!ledcReady) {
    ledcSetup(0, 2000, 13);
    ledcReady = true;
  }
  ledcAttachPin(BUZZER_PIN, 0);
  ledcWriteTone(0, frequency);
  delay(duration);
  ledcWriteTone(0, 0);
  ledcWrite(0, 0);
  ledcDetachPin(BUZZER_PIN);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  delay(2);
#endif
}

void playFinishMelody() {
  if (!SOUND_ENABLED) return;
  
  Serial.println("🎵 Playing finish melody...");
  
  for (int i = 0; i < FINISH_MELODY_LENGTH; i++) {
    int frequency = finishMelody[i].frequency;
    int duration = finishMelody[i].duration;
    
    playTone(frequency, duration);
    
    // Small pause between notes for clarity
    delay(10);
    
    // Allow other tasks to run (non-blocking for ESP)
    yield();
  }
  
  stopSound();
  Serial.println("✅ Finish melody completed");
}

void updateSound(PrinterState currentState, unsigned long finishedSince) {
  if (!SOUND_ENABLED) return;
  
  // Play sound once when print finishes (Finished state)
  if (currentState == Finished) {
    // Play melody once within 5 seconds of entering Finished
    if (!soundPlayed && (millis() - finishedSince) < 5000) {
      soundPlayed = true;
      Serial.println("🎵 Print complete — playing finish melody");
      playFinishMelody();
    }
  } else {
    // Reset flag when leaving Finished so next print plays again
    if (soundPlayed && currentState != Finished) {
      soundPlayed = false;
      currentNoteIndex = 0;
    }
  }
}

void stopSound() {
  if (!SOUND_ENABLED) return;
  
#ifdef ESP8266
  noTone(BUZZER_PIN);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
#elif defined(ESP32)
  ledcWriteTone(0, 0);
  ledcWrite(0, 0);
  ledcDetachPin(BUZZER_PIN);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
#endif
  delay(5);
}

#endif // SOUND_ALERTING_H
