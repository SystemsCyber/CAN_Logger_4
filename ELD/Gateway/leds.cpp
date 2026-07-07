#include <Arduino.h>
#include "leds.h"
// Status LEDs
const int ledPinB = 14; // Blue
const int ledPinY = 15; // Yellow
const int ledPinG = 9;  // Green
const int ledPinR = 32; // Red

bool initLEDs() {
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinY, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinR, OUTPUT);

  digitalWrite(ledPinB, HIGH);
  digitalWrite(ledPinY, HIGH);
  digitalWrite(ledPinG, HIGH);
  digitalWrite(ledPinR, HIGH);

  return true;
}

void setBlueLED(bool state)
{
    digitalWrite(ledPinB, state);
}

void setYellowLED(bool state)
{
    digitalWrite(ledPinY, state);
}

void setGreenLED(bool state)
{
    digitalWrite(ledPinG, state);
}

void setRedLED(bool state)
{
    digitalWrite(ledPinR, state);
}