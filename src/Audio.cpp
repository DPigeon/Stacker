#include "Audio.h"

Audio::Audio(bool enabled) {
  this->enabled = enabled;
}

void Audio::update() {
  if (enabled) {
    dacAudio.FillBuffer();
    dacAudio.DacVolume = 10; // TODO: use with ROTARY ENCODER
  }
}

void Audio::play(String audio) {
  if (enabled) {
    if (audio == "gameOver") {
      if (!gameOverAudio.Playing) {
        dacAudio.Play(&gameOverAudio);
      }
    } else if (audio == "stack") {
      if (!stackAudio.Playing) {
        dacAudio.Play(&stackAudio);
      }
    }
  }
}