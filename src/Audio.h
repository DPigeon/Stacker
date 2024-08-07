#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <XT_DAC_Audio.h>

#include "Sounds.cpp"

#define AUDIO_PIN 25
#define TIMER 0

class Audio {
  
  private:
    XT_DAC_Audio_Class dacAudio = XT_DAC_Audio_Class(AUDIO_PIN, TIMER); // Must be 8k Hz, unsigned 8-bit PCM
    bool enabled;

    // Audio
    XT_Wav_Class stackAudio = XT_Wav_Class(stack);
    XT_Wav_Class moveBlockAudio = XT_Wav_Class(moveBlock);
    XT_Wav_Class moveCornerBlockAudio = XT_Wav_Class(moveCornerBlock);
    XT_Wav_Class gameOverAudio = XT_Wav_Class(gameOver);
  
  public:
    Audio(bool enabled);
    void update();
    void play(String audio);
};

#endif
