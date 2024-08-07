#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  
  private:
    byte pin;
    bool soundEnabled;
  
  public:
    Buzzer(byte pin, bool enabled);
    void emitSound(int frequency, int durationMs, int delayMs);
};

#endif
