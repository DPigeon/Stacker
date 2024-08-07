/**
 * Core Game.
 * Level --> Stair on Matrix
 * Rank --> "New Difficulty"
 */

#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <Arduino.h>
#include <LedControl.h> // https://github.com/wayoda/LedControl/blob/master/src/LedControl.h
#include <Timer.h> // https://github.com/JChristensen/Timer/blob/master/Timer.h

#include "Audio.h"
#include "Buzzer.h"
#include "Util.cpp"

#define DIN_PIN 15   // Data In
#define CLK_PIN 2    // Clock SCK
#define CS_PIN 4     // Load Chip Select
#define BUZZER_PIN1 14
#define BUZZER_PIN2 34
#define NUM_DEVICE 1
#define MATRIX_LENGTH 8
#define ANIMATION_SPEED_MS 40
#define AFTER_START_ANIM_DELAY 500
#define GAME_EVENT_DELAY 1500
#define MISS_LED_ANIM_DELAY 100
#define FINISH_GAME_ANIM_DELAY 250
#define SOUND_LED_REACH_SIDE_DELAY 50
#define CHANNEL 0

class LedMatrix {
  
  private:
    static Audio audio;
    static Buzzer buzzer;
    LedControl ledControl = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICE);
    Timer timer;
    int ledTimer = 0;
    bool isGameOver = false;
    bool isAnimating, isInPause = false;
    unsigned long previousMillis, animDuration, animPauseDuration = 0;
    byte spriteToAnimate[MATRIX_LENGTH] = {
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000
    };
    /**
     * Score: getScore()
     * +1 each level
     * +5 each rank
     * -3 per block missed
    */
    int score = 0; // Max Score: 1 * level * rank + 5 * rank = 60
    static int level;
    static int rank;
    static int xLed;
    static int prevXLed;
    static int directionX; // 0: -->, 1: <--;

    static int ledNumberX[MATRIX_LENGTH];
    const int ledNumberXInitial[MATRIX_LENGTH] = { 5, 4, 3, 3, 3, 3, 3, 2 };

    static int speedX[MATRIX_LENGTH];
    const int speedXInitial[MATRIX_LENGTH] = { 500, 490, 480, 470, 460, 450, 440, 430 }; // TODO: Change for proper speed

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
    int ledMatrixInitial[MATRIX_LENGTH][MATRIX_LENGTH] = {
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
    void setSprite(byte *sprite);
    void setAnim(byte *sprite, unsigned long animDuration, unsigned long animPauseDuration);
    int getScore();
    void gameOver();
    bool getGameOver();
    void setGameOver(bool g);
    void resetGame();
    void rankUp();
    void(*reset) (void) = 0; // To reset after game over
    void print();
};

#endif
