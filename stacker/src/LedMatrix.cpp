#include "LedMatrix.h"

int LedMatrix::level = 0;
int LedMatrix::rank = 0;
int LedMatrix::xLed = 0;
int LedMatrix::prevXLed = 0;
int LedMatrix::directionX = 0;
int LedMatrix::ledNumberX[BLOCK_WIDTH] = { 5, 4, 3, 3, 3, 3, 3, 2 };
int LedMatrix::speedX[BLOCK_WIDTH] = { 500, 490, 480, 470, 460, 450, 440, 430 };
AudioWebSocket LedMatrix::audioWs = AudioWebSocket(AUDIO_WS_HOST, WS_PATH, PORT, AUDIO_ENABLED);

LedMatrix::LedMatrix() {
  HUB75_I2S_CFG config(
    PANEL_RES,
    PANEL_RES,
    PANEL_CHAIN
  );
  config.gpio.e = E_PIN;
  config.latch_blanking = 4;
  config.i2sspeed = HUB75_I2S_CFG::HZ_10M;

  display = new MatrixPanel_I2S_DMA(config);
  display->begin();
  display->setBrightness8(128);
  display->clearScreen();
  black = display->color565(0, 0, 0);
  clearBlack = display->color565(23, 23, 21);
  white = display->color565(255, 255, 255);
  blue = display->color444(0, 0, 15);
  red = display->color444(15, 0, 0);
}

void LedMatrix::init() {
  audioWs.connect();
  transitionAnim();
  ledTimer = timer.every(speedX[level], moveLed);
}

void LedMatrix::update(int pos, int state) {
  if (state == 1) {
    display->drawRect(pos * BLOCK_WIDTH, PANEL_RES - level * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, clearBlack);
    display->fillRect(pos * BLOCK_WIDTH + 1, PANEL_RES - level * BLOCK_WIDTH - BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, white);
  } else {
    display->fillRect(pos * BLOCK_WIDTH, PANEL_RES - level * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, black);
  }
  ledMatrix[BLOCK_WIDTH - level - 1][pos] = state;
}

void LedMatrix::updateTimer() {
  audioWs.loop();
  timer.update();
}

void LedMatrix::moveLed() {
  if (xLed == 0) {
    directionX = 0;
    audioWs.send("2");
  } else if (xLed == BLOCK_WIDTH - ledNumberX[level]) {
    directionX = 1;
    audioWs.send("2");
  }
  
  if (directionX == 0) { 
    xLed++;
    prevXLed = xLed - ledNumberX[level];
    audioWs.send("1");
  } else { 
    xLed--;
    prevXLed = xLed + ledNumberX[level];
    audioWs.send("1");
  }
}

void LedMatrix::drawGame() {
  if (level > -1 && level < BLOCK_WIDTH && ledNumberX[level] > 0) {
    for (int i = 0; i < ledNumberX[level]; i++) {
      if (prevXLed < BLOCK_WIDTH) {
        display->fillRect((prevXLed + i) * BLOCK_WIDTH, PANEL_RES - level * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, black);
      }
      display->drawRect((xLed + i) * BLOCK_WIDTH, PANEL_RES - level * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, clearBlack);
      display->fillRect((xLed + i) * BLOCK_WIDTH + 1, PANEL_RES - level * BLOCK_WIDTH - BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, white);
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
  int pairs[BLOCK_WIDTH - 1][2] = { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } }; // List of pair of xLed coords with current top stack
  // TODO: Generate random position after stacking
  for (int i = 0; i < ledNumberX[level]; i++) {
    int pos = posX + i;
    if (ledMatrix[BLOCK_WIDTH - level][pos] != 1) {
      update(pos, 0);
      currentLedNum--;
      pairs[i][0] = pos;
      pairs[i][1] = getTopStack(pos);
    } else {
      update(pos, 1);
    }
  }
  if (currentLedNum < ledNumberX[level]) {
    // Add sound
    missAnim(pairs, ledNumberX[level] - currentLedNum);
    ledNumberX[level] = currentLedNum;
    removeLedsFromNextLevel(); // Bug: TODO some blocks are kept during next level
  }
  
  return currentLedNum != 0;
}

void LedMatrix::removeLedsFromNextLevel() {
  for (int i = level; i < BLOCK_WIDTH - 2; i++) {
    int nextLevel = i + 1;
    if (ledNumberX[nextLevel] > ledNumberX[level]) {
      ledNumberX[nextLevel]--;
    }
  }
}

void LedMatrix::levelUp() {
  level++;
  score = score + 1;
  audioWs.send("3"); // Add random audioWs.send("4"); and "5", "6" when upper
  if (level == BLOCK_WIDTH) {
    rankUp();
    transitionAnim();
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
        // Play sound
      }
    }
  }
}

int LedMatrix::getTopStack(int pos) {
  int top = 0;
  for (int i = level; i >= 0; i--) {
    if (ledMatrix[BLOCK_WIDTH - i - 1][pos] == 1) {
      top = i + 1;
      break;
    }
  }
  return top;
}

void LedMatrix::drawBitmap(int x, int y, int width, int height, const char *xbm, uint16_t color) {
  if (width % 8 != 0) {
    width =  ((width / 8) + 1) * 8;
  }
  for (int i = 0; i < width * height / 8; i++ ) {
    unsigned char charColumn = pgm_read_byte(xbm + i);
    for (int j = 0; j < 8; j++) {
      int targetX = (i * 8 + j) % width + x;
      int targetY = (8 * i / (width)) + y;
      if (bitRead(charColumn, j)) {
        display->drawPixel(targetX, targetY, color);
      }
    }
  }
}

void LedMatrix::startAnimation() {
  for (int i = 0; i < BLOCK_WIDTH; i++) {
    for (int j = 0; j < BLOCK_WIDTH; j++) {
      display->drawRect(i * BLOCK_WIDTH, PANEL_RES - j * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, clearBlack);
      display->fillRect(i * BLOCK_WIDTH + 1, PANEL_RES - j * BLOCK_WIDTH - BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, white);
      delay(ANIMATION_SPEED_MS); // Blocking, but OK
    }
  }
  delay(AFTER_START_ANIM_DELAY);
  display->clearScreen();
}

void LedMatrix::transitionAnim() {
  display->setTextSize(1);
  display->setTextWrap(false);
  display->setCursor(8, 20);
  char c[3];
  sprintf(c, "%ld", rank + 1);
  String msg = "Level: ";
  msg += c;
  for (uint8_t w = 0; w < msg.length(); w++) {
    display->setTextColor(blue);
    display->print(msg[w]);
  }

  drawBitmap(16, 20, 32, 32, lightningBits, white);
  delay(2000);
  display->clearScreen();
  delay(500);
}

void LedMatrix::missAnim(int pairs[BLOCK_WIDTH - 1][2], int pairsLength) {
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
        display->drawRect(pairs[0][0] * BLOCK_WIDTH, PANEL_RES - i * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, clearBlack);
        display->fillRect(pairs[0][0] * BLOCK_WIDTH + 1, PANEL_RES - i * BLOCK_WIDTH - BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, white);
        delay(MISS_LED_ANIM_DELAY);
        display->fillRect(pairs[0][0] * BLOCK_WIDTH, PANEL_RES - i * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, black);
        delay(MISS_LED_ANIM_DELAY);
      }
    }
  } else {
    for (int i = 0; i < pairsLength; i++) {
      for (int j = level - 1; j >= pairs[i][1]; j--) {
        display->drawRect(pairs[i][0] * BLOCK_WIDTH, PANEL_RES - j * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, clearBlack);
        display->fillRect(pairs[i][0] * BLOCK_WIDTH + 1, PANEL_RES - j * BLOCK_WIDTH - BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, white);
      }
    }
    delay(MISS_LED_ANIM_DELAY / 5);
    for (int i = 0; i < pairsLength; i++) {
      for (int j = level - 1; j >= pairs[i][1]; j--) {
        display->fillRect(pairs[i][0] * BLOCK_WIDTH, PANEL_RES - j * BLOCK_WIDTH - BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, black);
      }
    }
    delay(MISS_LED_ANIM_DELAY / 5);
  }
  score = score - (pairsLength * 2);
}

void LedMatrix::finishGameAnim() {
  delay(FINISH_GAME_ANIM_DELAY);
  display->clearScreen();
  delay(GAME_EVENT_DELAY);
}

int LedMatrix::getScore() {
  return score;
}

uint16_t LedMatrix::colorWheel(uint8_t pos) { // From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
  if (pos < 85) {
    return display->color565(pos * 3, 255 - pos * 3, 0);
  } else if (pos < 170) {
    pos -= 85;
    return display->color565(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return display->color565(0, pos * 3, 255 - pos * 3);
  }
}

void LedMatrix::gameOver() {
  isGameOver = true;
  delay(GAME_EVENT_DELAY);
  unsigned long startTime = millis();
  bool done = false;
  int time = 5500; // Time for gameOver sound
  audioWs.send("9"); // add "8" first and then "9"
  while (!done) {
    int interval = millis() - startTime;
    if (interval >= time) {
      resetGame();
      done = true;
    } else if (interval >= time - 1000) {
      display->clearScreen();
    } else {
      display->setTextSize(1);
      display->setTextWrap(false);
      display->setCursor(5, 0);
      const char *str = "GAME OVER";
      for (uint8_t w = 0; w < strlen(str); w++) {
        display->setTextColor(colorWheel((w * 32)));
        display->print(str[w]);
      }
    }
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
      for (int i = 0; i < BLOCK_WIDTH; i++) {
        ledNumberX[i] = ledNumberXInitial[i];
        speedX[i] = speedXInitial[i];
        for (int j = 0; j < BLOCK_WIDTH; j++) {
          ledMatrix[i][j] = ledMatrixInitial[i][j];
        }
      }
    }
    timer.stop(ledTimer);
    ledTimer = timer.every(speedX[level], moveLed);
    display->clearScreen();
  }
}

void LedMatrix::rankUp() {
  if (rank == MAX_RANK) {
    finishGameAnim();
    resetGame();
  } else {
    delay(250);
    display->clearScreen();
    level = 0;
    rank++;
    score = score + 5;
    xLed = 0;
    prevXLed = 0;
    directionX = 0;
    for (int i = 0; i < BLOCK_WIDTH; i++) {
      ledNumberX[i] = ledNumberXLevels[rank][i];
      speedX[i] = speedXLevels[rank][i];
      for (int j = 0; j < BLOCK_WIDTH; j++) {
        ledMatrix[i][j] = ledMatrixInitial[i][j];
      }
    }
    timer.stop(ledTimer);
    ledTimer = timer.every(speedX[level], moveLed);
  }
}

void LedMatrix::print() {
  for (int i = 0; i < BLOCK_WIDTH; i++) {
    for (int j = 0; j < BLOCK_WIDTH; j++) {
      Serial.print(ledMatrix[i][j]);
    }
    Serial.print("\n");
  }
  Serial.print('\n');
}
