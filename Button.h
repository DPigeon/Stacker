#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
	
	private:
	  byte pin;
		bool buttonWasLow;
    bool pressed;
	
	public:
		Button(byte pin);
		bool readState();
    void handleState();
    bool isPressed();
};

#endif
