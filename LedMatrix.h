#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <Arduino.h>
#include <LedControl.h> // https://github.com/wayoda/LedControl/blob/master/src/LedControl.h
#include <Timer.h> // https://github.com/JChristensen/Timer/blob/master/Timer.h

#include "Buzzer.h"
#include "Util.cpp"

#define DIN_PIN 15    // Data In
#define CLK_PIN 2    // Clock SCK
#define CS_PIN 4     // Load Chip Select
#define BUZZER_PIN 14
#define NUM_DEVICE 1
#define MATRIX_LENGTH 8
#define ANIMATION_SPEED_MS 40
#define AFTER_START_ANIM_DELAY 500
#define GAME_EVENT_DELAY 1500
#define MISS_LED_ANIM_DELAY 100
#define FINISH_GAME_ANIM_DELAY 250

class LedMatrix {
  
  private:
    Buzzer buzzer = Buzzer(BUZZER_PIN, true); // Sound enabled: true
    LedControl ledControl = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICE);
    Timer timer;
    int ledTimer = 0;
    bool isGameOver = false;
    static int level;
    static int xLed;
    static int prevXLed;
    static int directionX; // 0: -->, 1: <--;
    static int ledNumberX[MATRIX_LENGTH];
    int speedX[MATRIX_LENGTH] = { 140, 135, 125, 110, 75, 50, 40, 25 }; // TODO: Change for proper speed
    int ledMatrix[MATRIX_LENGTH][MATRIX_LENGTH] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
  
  public:
    LedMatrix();
    void init();
    void update(int pos, int state);
    void updateTimer();
    static void moveLed();
    void drawGame();
    void stackLed();
    void drawFirstLevel(int posX);
    bool hasLedBelow(int posX);
    void removeLedsFromNextLevel();
    void levelUp();
    int getTopStack(int pos);
    void startAnimation();
    void missAnim(int pairs[MATRIX_LENGTH - 1][2], int pairsLength);
    void finishGameAnim();
    void gameOverSoundAnim();
    void setSprite(byte *sprite);
    void gameOver();
    bool getGameOver();
    void(*reset) (void) = 0; // To reset after game over
    void print();
};

#endif
