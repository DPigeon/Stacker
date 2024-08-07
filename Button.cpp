/**
 * Button class to handle the logic of the button.
 */

#include "Button.h"

Button::Button(byte pin) {
	this->pin = pin;
	this->buttonWasLow = true;
  this->pressed = false;
	pinMode(pin, INPUT);
}

bool Button::readState() {
	return digitalRead(pin);
}

void Button::handleState() {
  if (pressed) { // Only 1 call after pressed
    pressed = false;
  }
  if (readState() == LOW) {
    buttonWasLow = true;
  } else if (readState() == HIGH && buttonWasLow) {
    buttonWasLow = false;
    pressed = true;
  }
}

bool Button::isPressed() {
  handleState();
  return pressed;
}
