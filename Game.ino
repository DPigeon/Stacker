/**
 * Arcade Game 
 * Stack dots on top of each other using a push button.
 */

#include "Button.h"
#include "LedMatrix.h"

#define BUTTON_PIN 33

Button button(BUTTON_PIN); 
LedMatrix ledMatrix;
                
void setup() { 
  Serial.begin(9600);
  ledMatrix.init();
}

void loop() {
  ledMatrix.updateTimer();
  handleButton();
  ledMatrix.drawGame();

  #ifdef DEBUG
    Serial.print(xLed);
    Serial.print("\n");
  #endif
}

void handleButton() {
  if (button.isPressed()) {
    if (ledMatrix.getGameOver()) ledMatrix.reset();
    ledMatrix.stackLed();
  }
}
