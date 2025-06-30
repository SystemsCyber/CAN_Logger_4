#include "led_status.h"


static const uint8_t ledPins[] = { 9, 32, 14, 15 };
static const size_t ledCount = sizeof(ledPins) / sizeof(ledPins[0]);

static LEDMode modes[ledCount];
static elapsedMillis timers[ledCount];
static bool ledStates[ledCount];

void initLEDs() {
    for (size_t i = 0; i < ledCount; ++i) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
        modes[i] = LED_OFF;
        timers[i] = 0;
        ledStates[i] = false;
    }

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void setLEDMode(uint8_t index, LEDMode mode) {
    if (index >= ledCount) return;
    modes[index] = mode;
    timers[index] = 0;

    if (mode == LED_OFF || mode == LED_ON) {
        digitalWrite(ledPins[index], mode == LED_ON ? HIGH : LOW);
        ledStates[index] = (mode == LED_ON);
    }
}

void setLEDState(uint8_t index, bool pulse) {
    if (index >= ledCount) return;
    if (pulse) {
        modes[index] = LED_BLINK_ONCE;
        timers[index] = 0;
        digitalWrite(ledPins[index], HIGH);
        ledStates[index] = true;
    }
}

void updateLEDs() {
    for (size_t i = 0; i < ledCount; ++i) {
        switch (modes[i]) {
            case LED_BLINK_ONCE:
                if (timers[i] >= 100) {
                    digitalWrite(ledPins[i], LOW);
                    ledStates[i] = false;
                    modes[i] = LED_OFF;
                }
                break;

            case LED_BLINK_FAST:
                if (timers[i] >= 250) {
                    ledStates[i] = !ledStates[i];
                    digitalWrite(ledPins[i], ledStates[i] ? HIGH : LOW);
                    timers[i] = 0;
                }
                break;

            case LED_BLINK_SLOW:
                if (timers[i] >= 1000) {
                    ledStates[i] = !ledStates[i];
                    digitalWrite(ledPins[i], ledStates[i] ? HIGH : LOW);
                    timers[i] = 0;
                }
                break;

            default:
                break;
        }
    }
}
