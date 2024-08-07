/**
 * Util class for reusable components.
 */

#include <Arduino.h>

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
