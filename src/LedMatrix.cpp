#include "LedMatrix.h"

int LedMatrix::level = 0;
int LedMatrix::rank = 0;
int LedMatrix::xLed = 0;
int LedMatrix::prevXLed = 0;
int LedMatrix::directionX = 0;
int LedMatrix::ledNumberX[MATRIX_LENGTH] = { 5, 4, 3, 3, 3, 3, 3, 2 };
int LedMatrix::speedX[MATRIX_LENGTH] = { 500, 490, 480, 470, 460, 450, 440, 430 };
Audio LedMatrix::audio = Audio(false);
Buzzer LedMatrix::buzzer = Buzzer(BUZZER_PIN1, BUZZER_PIN2, true);

LedMatrix::LedMatrix() {
  ledControl.shutdown(0, false); // Power-saving mode on startup
  ledControl.setIntensity(0, 0); // Set the brightness to maximum value
  ledControl.clearDisplay(0);    // and clear the display
}

void LedMatrix::init() {
  buzzer.init();
  startAnimation();
  ledTimer = timer.every(speedX[level], moveLed);
}

void LedMatrix::update(int pos, int state) {
  bool boolState = LOW;
  if (state == 1) boolState = HIGH;
  ledControl.setLed(0, pos, level, boolState);
  ledMatrix[MATRIX_LENGTH - level - 1][pos] = state;
}

void LedMatrix::updateTimer() {
  timer.update();
}

void LedMatrix::moveLed() {
  if (xLed == 0) {
    directionX = 0;
    //buzzer.emitSoundWithDelay(300, SOUND_LED_REACH_SIDE_DELAY);
    //buzzer.stopSoundWithDelay();
  } else if (xLed == MATRIX_LENGTH - ledNumberX[level]) {
    directionX = 1;
    //buzzer.emitSoundWithDelay(300, SOUND_LED_REACH_SIDE_DELAY);
    //buzzer.stopSoundWithDelay();
  }
  
  if (directionX == 0) { 
    xLed++;
    prevXLed = xLed - ledNumberX[level];
    //buzzer.emitSound(150, 35, 35); // speedX[level] / 4?
    //buzzer.emitSoundWithDelay(150, 25);
    //buzzer.stopSoundWithDelay();
  } else { 
    xLed--;
    prevXLed = xLed + ledNumberX[level];
    //buzzer.emitSoundWithDelay(150, 25);
    //buzzer.stopSoundWithDelay();
  }
}

void LedMatrix::drawGame() {
  if (level > -1 && level < MATRIX_LENGTH && ledNumberX[level] > 0) {
    for (int i = 0; i < ledNumberX[level]; i++) {
      if (prevXLed < MATRIX_LENGTH) {
        ledControl.setLed(0, prevXLed + i, level, LOW);
      }
      ledControl.setLed(0, xLed + i, level, HIGH);
    }
  }
}

void LedMatrix::stackLed() {
  int posX = xLed;
  if (level == 0) {
    drawFirstLevel(posX);
  } else if (level > 0) {
    if (hasLedBelow(posX)) {
      levelUp();
    } else {
      gameOver();
    }
  }
  #ifdef DEBUG
    print();
  #endif
}

void LedMatrix::drawFirstLevel(int posX) {
  for (int i = 0; i < ledNumberX[level]; i++) {
    update(posX + i, 1);
  }
  levelUp();
}

bool LedMatrix::hasLedBelow(int posX) {
  int currentLedNum = ledNumberX[level];
  int pairs[MATRIX_LENGTH - 1][2] = { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } }; // List of pair of xLed coords with current top stack
  for (int i = 0; i < ledNumberX[level]; i++) {
    int pos = posX + i;
    if (ledMatrix[MATRIX_LENGTH - level][pos] != 1) {
      update(pos, 0);
      currentLedNum--;
      pairs[i][0] = pos;
      pairs[i][1] = getTopStack(pos);
    } else {
      update(pos, 1);
    }
  }
  if (currentLedNum < ledNumberX[level]) {
    missAnim(pairs, ledNumberX[level] - currentLedNum);
    ledNumberX[level] = currentLedNum;
    removeLedsFromNextLevel();
  }
  
  return currentLedNum != 0;
}

void LedMatrix::removeLedsFromNextLevel() {
  for (int i = level; i < MATRIX_LENGTH - 2; i++) {
    int nextLevel = i + 1;
    if (ledNumberX[nextLevel] > ledNumberX[level]) {
      ledNumberX[nextLevel]--;
    }
  }
}

void LedMatrix::levelUp() {
  level++;
  score = score + 1;
  if (level == MATRIX_LENGTH) {
    rankUp();
  } else {
    unsigned long startTime = millis();
    bool done = false;
    int time = 1000; // Time for stack sound
    while (!done) {
      int interval = millis() - startTime;
      if (interval >= time) {
        timer.stop(ledTimer);
        ledTimer = timer.every(speedX[level], moveLed);
        done = true;
      } else {
        audio.play("stack");
      }
      audio.update();
    }
    //buzzer.emitSoundWithDelay(300, 150);
    //buzzer.emitSoundWithDelay(100, 50);
    //buzzer.stopSoundWithDelay();
    //delay(GAME_EVENT_DELAY);
  }
}

int LedMatrix::getTopStack(int pos) {
  int top = 0;
  for (int i = level; i >= 0; i--) {
    if (ledMatrix[MATRIX_LENGTH - i - 1][pos] == 1) {
      top = i + 1;
      break;
    }
  }
  return top;
}

void LedMatrix::startAnimation() {
  for (int i = 0; i < MATRIX_LENGTH; i++) {
    for (int j = 0; j < MATRIX_LENGTH; j++) {
      ledControl.setLed(0, i, MATRIX_LENGTH - j - 1, HIGH);
      delay(ANIMATION_SPEED_MS); // Blocking, but OK
    }
  }
  delay(AFTER_START_ANIM_DELAY);
  ledControl.clearDisplay(0);
}

void LedMatrix::missAnim(int pairs[MATRIX_LENGTH - 1][2], int pairsLength) {
  #ifdef DEBUG
    for (int i = 0; i < pairsLength; i++) {
      Serial.print("\n");
      Serial.print(pairs[i][0]);
      Serial.print("\n");
      Serial.print(pairs[i][1]);
    }
  #endif
  if (pairsLength == 1) {
    for (int i = level - 1; i >= pairs[0][1]; i--) {
      if (pairs[0][0] != -1 && pairs[0][1] != -1) { // Problem
        ledControl.setLed(0, pairs[0][0], i, HIGH);
        delay(MISS_LED_ANIM_DELAY);
        ledControl.setLed(0, pairs[0][0], i, LOW);
        delay(MISS_LED_ANIM_DELAY);
      }
    }
  } else {
    for (int i = 0; i < pairsLength; i++) {
      for (int j = level - 1; j >= pairs[i][1]; j--) {
        ledControl.setLed(0, pairs[i][0], j, HIGH);
      }
    }
    delay(MISS_LED_ANIM_DELAY / 5);
    for (int i = 0; i < pairsLength; i++) {
      for (int j = level - 1; j >= pairs[i][1]; j--) {
        ledControl.setLed(0, pairs[i][0], j, LOW);
      }
    }
    delay(MISS_LED_ANIM_DELAY / 5);
  }
  score = score - (pairsLength * 3);
}

void LedMatrix::finishGameAnim() {
  int freq[6] = { 500, 500, 500, 800, 500, 600 };
  int ms[6] = { 200, 200, 200, 150, 500, 1000 }; 
  byte *sprite[6] = { winSprite1, winSprite2, winSprite3, winSprite4, winSprite1, winSprite4 };
  for (int i = 0; i < 6; i++) {
    buzzer.emitSoundWithDelay(freq[i], ms[i]);
    setSprite(sprite[i]);
  }
  delay(FINISH_GAME_ANIM_DELAY);
  ledControl.clearDisplay(0);
  delay(GAME_EVENT_DELAY);
}

void LedMatrix::setSprite(byte *sprite) {
  for (int i = 0; i < MATRIX_LENGTH; i++) {
    ledControl.setColumn(0, MATRIX_LENGTH - i - 1, sprite[i]);
  }
}

void LedMatrix::setAnim(byte *sprite, unsigned long duration, unsigned long pauseDuration) {
  for (int i = 0; i < MATRIX_LENGTH; i++) {
    spriteToAnimate[i] = sprite[i];
  }
  animDuration = duration;
  animPauseDuration = pauseDuration;
  isInPause = true;
}

int LedMatrix::getScore() {
  return score;
}

void LedMatrix::gameOver() {
  isGameOver = true;
  delay(GAME_EVENT_DELAY);
  unsigned long startTime = millis();
  bool done = false;
  int time = 5500; // Time for gameOver sound
  while (!done) {
    int interval = millis() - startTime;
    if (interval >= time) {
      resetGame();
      done = true;
    } else if (interval >= time / 2) {
      setSprite(gameOverSprite);
    } else if (interval >= time / 4) {
      ledControl.clearDisplay(0);
    } else {
      audio.play("gameOver");
      setSprite(gameOverSprite);
    }
    audio.update();
  }
}

bool LedMatrix::getGameOver() {
  return isGameOver;
}

void LedMatrix::setGameOver(bool g) {
  isGameOver = g;
}

void LedMatrix::resetGame() {
  if (isGameOver) {
    if (ledNumberX[level] == 0) {
      score = 0;
      level = 0;
      rank = 0;
      xLed = 0;
      prevXLed = 0;
      directionX = 0;
      for (int i = 0; i < MATRIX_LENGTH; i++) {
        ledNumberX[i] = ledNumberXInitial[i];
        speedX[i] = speedXInitial[i];
        for (int j = 0; j < MATRIX_LENGTH; j++) {
          ledMatrix[i][j] = ledMatrixInitial[i][j];
        }
      }
    }
    timer.stop(ledTimer);
    ledTimer = timer.every(speedX[level], moveLed);
    ledControl.clearDisplay(0);
  }
}

void LedMatrix::rankUp() {
  if (rank == MAX_RANK) {
    finishGameAnim();
    resetGame();
  } else {
    delay(250);
    buzzer.emitSoundWithDelay(300, 200);
    buzzer.emitSoundWithDelay(1000, 500);
    buzzer.stopSoundWithDelay();
    ledControl.clearDisplay(0);
    level = 0;
    rank++;
    score = score + 5;
    xLed = 0;
    prevXLed = 0;
    directionX = 0;
    for (int i = 0; i < MATRIX_LENGTH; i++) {
      ledNumberX[i] = ledNumberXLevels[rank][i];
      speedX[i] = speedXLevels[rank][i];
      for (int j = 0; j < MATRIX_LENGTH; j++) {
        ledMatrix[i][j] = ledMatrixInitial[i][j];
      }
    }
    timer.stop(ledTimer);
    ledTimer = timer.every(speedX[level], moveLed);
  }
}

void LedMatrix::print() {
  for (int i = 0; i < MATRIX_LENGTH; i++) {
    for (int j = 0; j < MATRIX_LENGTH; j++) {
      Serial.print(ledMatrix[i][j]);
    }
    Serial.print("\n");
  }
  Serial.print('\n');
}
