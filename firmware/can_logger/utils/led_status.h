#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>

enum LEDMode {
    LED_OFF,
    LED_ON,
    LED_BLINK_ONCE,
    LED_BLINK_FAST,
    LED_BLINK_SLOW
};

void initLEDs();

void setLEDMode(uint8_t index, LEDMode mode);

void setLEDState(uint8_t index, bool pulse);

void updateLEDs();

#endif
