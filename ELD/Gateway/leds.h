#ifndef LEDS_H
#define LEDS_H

/***********************************************************************
 * LEDs
 *
 * Initializes and controls the status LEDs.
 ***********************************************************************/

bool initLEDs(void);
void setBlueLED(bool state);
void setYellowLED(bool state);
void setGreenLED(bool state);
void setRedLED(bool state);

#endif