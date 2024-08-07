#include "LedMatrix.h"

int LedMatrix::level = 0;
int LedMatrix::xLed = 0;
int LedMatrix::prevXLed = 0;
int LedMatrix::directionX = 0;
int LedMatrix::ledNumberX[MATRIX_LENGTH] = { 3, 3, 2, 2, 1, 1, 1, 1 };

LedMatrix::LedMatrix() {
  ledControl.shutdown(0, false); // Power-saving mode on startup
  ledControl.setIntensity(0, 0); // Set the brightness to maximum value
  ledControl.clearDisplay(0);    // and clear the display
}

void LedMatrix::init() {
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
  } else if (xLed == MATRIX_LENGTH - ledNumberX[level]) {
    directionX = 1;
  }
  
  if (directionX == 0) { 
    xLed++;
    prevXLed = xLed - ledNumberX[level];
  } else { 
    xLed--;
    prevXLed = xLed + ledNumberX[level];
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
  if (level == MATRIX_LENGTH) {
    finishGameAnim();
    reset();
  }
  timer.stop(ledTimer);
  ledTimer = timer.every(speedX[level], moveLed);
  // TODO: Add sound buzzer
  delay(GAME_EVENT_DELAY);
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
}

void LedMatrix::finishGameAnim() {
  int freq[6] = { 500, 500, 500, 800, 500, 600 };
  int ms[6] = { 200, 200, 200, 150, 500, 1000 }; 
  byte *sprite[6] = { winSprite1, winSprite2, winSprite3, winSprite4, winSprite1, winSprite4 };
  for (int i = 0; i < 6; i++) {
    buzzer.emitSound(freq[i], ms[i], ms[i]);
    setSprite(sprite[i]);
  }
  delay(FINISH_GAME_ANIM_DELAY);
  ledControl.clearDisplay(0);
  delay(GAME_EVENT_DELAY);
}

void LedMatrix::gameOverSoundAnim() {
  int noteDuration = 1000 / 8; // 8 note
  int noteDelay = noteDuration * 0.2;
  for (int i = 0; i < 4; i++) { // 4 times
    if (i%2) setSprite(gameOverSprite);
    for (int j = 25; j < 30; j++) {
      buzzer.emitSound(20 * j, noteDuration, noteDelay);
    }
    for (int j = 30; j >= 25; j--) {
      buzzer.emitSound(20 * j, noteDuration, noteDelay);
    }
    if (i%2) ledControl.clearDisplay(0);
  }
  delay(100);
  buzzer.emitSound(500, 200, 200);
  buzzer.emitSound(1200, 200, 200);
  setSprite(gameOverSprite);
  buzzer.emitSound(300, 200, 200);
  buzzer.emitSound(1000, 200, 200);
  buzzer.emitSound(400, 200, 200);
  buzzer.emitSound(1100, 200, 200);
  delay(500);
  ledControl.clearDisplay(0);
}

void LedMatrix::setSprite(byte *sprite) {
  for (int i = 0; i < MATRIX_LENGTH; i++) {
    ledControl.setColumn(0, MATRIX_LENGTH - i - 1, sprite[i]);
  }
}

void LedMatrix::gameOver() {
  isGameOver = true;
  delay(GAME_EVENT_DELAY);
  gameOverSoundAnim();
}

bool LedMatrix::getGameOver() {
  return isGameOver;
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
