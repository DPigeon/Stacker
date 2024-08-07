#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  
  private:
    byte pin1;
    byte pin2;
    bool soundEnabled;
    int channel1 = 0;
    int channel2 = 1;
    bool isPlaying = false;
    unsigned long previousMillis = 0;
    int soundDuration = 0;
    int pauseBetweenSounds = 0;
    double frequency = 0;
  
  public:
    Buzzer(byte pin1, byte pin2, bool enabled);
    void init();
    void emitSound(double freq, int duration, int pauseBetweenDuration);
    void stopSound();
    void emitSoundWithDelay(double frequency, int delayMs);
    void stopSoundWithDelay();
};

#endif
