/**
 * Util class for reusable components.
 */

#include <Arduino.h>

const int LENGTH = 8;
const int MAX_RANK = 6;
static int ledNumberXLevels[MAX_RANK][LENGTH] = {
  5, 4, 3, 3, 3, 3, 3, 2,
  4, 4, 3, 3, 3, 3, 2, 2,
  3, 3, 3, 3, 3, 2, 2, 2,
  3, 3, 3, 3, 2, 2, 2, 1,
  3, 3, 2, 2, 1, 1, 1, 1,
  3, 3, 2, 2, 1, 1, 1, 1
};
static int speedXLevels[MAX_RANK][LENGTH] = { // 140ms tested, 25ms is very hard
  500, 490, 480, 470, 460, 450, 440, 430, 
  430, 420, 410, 400, 380, 360, 340, 320,
  360, 345, 330, 315, 295, 275, 255, 235,
  255, 240, 225, 210, 180, 150, 130, 110,
  140, 135, 130, 125, 115, 105, 95, 85,
  135, 128, 124, 110, 75, 50, 40, 25
};

// Sprites
static byte winSprite1[] = {
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00000000,
  B00000000,
  B00000000
};
static byte winSprite2[] = {
  B00000000,
  B00000000,
  B00111100,
  B00100100,
  B00100100,
  B00111100,
  B00000000,
  B00000000
};

static byte winSprite3[] = {
  B00000000,
  B01111110,
  B01000010,
  B01011010,
  B01011010,
  B01000010,
  B01111110,
  B00000000
};

static byte winSprite4[] = {
  B11111111,
  B10000001,
  B10111101,
  B10100101,
  B10100101,
  B10111101,
  B10000001,
  B11111111
};

static byte gameOverSprite[] = {
  B10000010,
  B01000100,
  B00101000,
  B00010000,
  B00101000,
  B01000100,
  B10000010,
  B00000000
};
