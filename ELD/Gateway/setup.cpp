#include <Arduino.h>
#include "setup.h"
#include "leds.h"
#include "storage.h"
#include "crypto.h"
#include "gps.h"
#include "can.h"
#include "wifi.h"
#include "timezone.h"
#include "eld.h"


/***********************************************************************
 * setupELD()
 *
 * Initializes the CAN Logger hardware and software modules.
 ***********************************************************************/
void setupELD(void)
{
    Serial.println();
    Serial.println("========================================");
    Serial.println("Setting up CAN Logger ELD Prototype");
    Serial.println("========================================");

    // Initialize status LEDs
    if (!initLEDs())
    {
        Serial.println("LED Initialization Failed");
        while (1);
    }
    
    if (!initStorage()){
        Serial.println("Storage Initialization Failed");
        while (1);
    }

    if (!initCrypto()){
        Serial.println("Crypto Initialization Failed");
        while (1);
    }

    if (!initGPS()){
        Serial.println("GPS Initialization Failed");
        while (1);
    }

    if(!initCAN()){
        Serial.println("CAN Initialization Failed");
        while (1);
    }

    if (!setupWifi()){
        Serial.println("WiFi Initialization Failed");
        while (1);
    }

    initTimeZone();

    initELD();
    Serial.println();
    Serial.println("ELD Initialization Complete.");
}