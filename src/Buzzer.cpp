/**
 * Buzzer class to handle the logic of the sound buzzer.
 */

#include "Buzzer.h"

Buzzer::Buzzer(byte pin1, byte pin2, bool enabled) {
  this->pin1 = pin1;
  this->pin2 = pin2;
  this->soundEnabled = enabled;
}

void Buzzer::init() {
  ledcSetup(channel1, 2000, 8);
  ledcAttachPin(pin1, channel1);
  ledcSetup(channel2, 2000, 8);
  ledcAttachPin(pin2, channel2);
}

void Buzzer::emitSound(double freq, int duration, int pauseBetweenDuration) {
  if (soundEnabled) {
    isPlaying = true;
    frequency = freq;
    soundDuration = duration;
    pauseBetweenSounds = pauseBetweenDuration;
  }
}

void Buzzer::stopSound() {
  isPlaying = false;
}

void Buzzer::emitSoundWithDelay(double freq, int delayMs) {
  if (soundEnabled) {
    ledcWriteTone(channel1, freq);
    ledcWriteTone(channel2, freq);
    delay(delayMs);
  }
}

void Buzzer::stopSoundWithDelay() {
  ledcWriteTone(channel1, 0);
  ledcWriteTone(channel2, 0);
}

