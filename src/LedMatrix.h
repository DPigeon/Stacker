/**
 * Core Game.
 * Level --> Stair on Matrix
 * Rank --> "New Difficulty"
 * Panel: https://www.waveshare.com/wiki/RGB-Matrix-P3-64x64-F
 * Using default pins in esp32-default-pins.hpp
 */

#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Timer.h> // https://github.com/JChristensen/Timer/blob/master/Timer.h

#include "AudioWebSocket.h"
#include "Config.cpp"
#include "Util.cpp"

#define PANEL_RES 64
#define PANEL_CHAIN 1
#define BLOCK_WIDTH 8
#define E_PIN 33

#define BUZZER_PIN1 32
#define BUZZER_PIN2 35

#define ANIMATION_SPEED_MS 40
#define AFTER_START_ANIM_DELAY 500
#define GAME_EVENT_DELAY 1500
#define MISS_LED_ANIM_DELAY 100
#define FINISH_GAME_ANIM_DELAY 250
#define SOUND_LED_REACH_SIDE_DELAY 50
#define CHANNEL 0

class LedMatrix {
  
  private:
    static AudioWebSocket audioWs;

    MatrixPanel_I2S_DMA *display = nullptr;
    uint16_t black = 0;
    uint16_t clearBlack = 0;
    uint16_t white = 0;
    uint16_t blue = 0;
    uint16_t red = 0;

    Timer timer;
    int ledTimer = 0;
    bool isGameOver = false;

    /**
     * Score: getScore()
     * +1 each level
     * +5 each rank
     * -2 per block missed
    */
    int score = 0; // Max Score: 1 * level * rank + 5 * rank = 60
    static int level;
    static int rank;
    static int xLed;
    static int prevXLed;
    static int directionX; // 0: -->, 1: <--;

    static int ledNumberX[BLOCK_WIDTH];
    const int ledNumberXInitial[BLOCK_WIDTH] = { 5, 4, 3, 3, 3, 3, 3, 2 };

    static int speedX[BLOCK_WIDTH];
    const int speedXInitial[BLOCK_WIDTH] = { 500, 490, 480, 470, 460, 450, 440, 430 }; // TODO: Change for proper speed

    int ledMatrix[BLOCK_WIDTH][BLOCK_WIDTH] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
    int ledMatrixInitial[BLOCK_WIDTH][BLOCK_WIDTH] = {
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
    void drawBitmap(int x, int y, int width, int height, const char *xbm, uint16_t color = 0xffff);
    void startAnimation();
    void transitionAnim();
    void missAnim(int pairs[BLOCK_WIDTH - 1][2], int pairsLength);
    void finishGameAnim();
    int getScore();
    uint16_t colorWheel(uint8_t pos);
    void gameOver();
    bool getGameOver();
    void setGameOver(bool g);
    void resetGame();
    void rankUp();
    void(*reset) (void) = 0; // To reset after game over
    void print();
};

#endif
