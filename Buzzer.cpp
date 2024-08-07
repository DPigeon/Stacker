/**
 * Buzzer class to handle the logic of the sound buzzer.
 */

#include "Buzzer.h"

Buzzer::Buzzer(byte pin, bool enabled) {
  this->pin = pin;
  this->soundEnabled = enabled;
}

void Buzzer::emitSound(int frequency, int durationMs, int delayMs) {
  // Minimum freq: 31 Hz
  if (soundEnabled) {
    tone(pin, frequency, durationMs);
    delay(delayMs);
  }
}
