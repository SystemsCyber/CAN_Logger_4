#include "gps_manager.h"
#include "../include/data_structs.h"
#include "../logger/logger_manager.h"
#include "../utils/led_status.h"

#include <SparkFun_u-blox_GNSS_v3.h>

SFE_UBLOX_GNSS myGNSS;
GPSManager gpsManager;

void GPSManager::begin(TwoWire& wire) {
    if (!myGNSS.begin(wire)) {
        Serial.println("GNSS not detected on I2C bus!");
        return;
    }

    myGNSS.setI2COutput(COM_TYPE_UBX);      
    myGNSS.setNavigationFrequency(5);      
    myGNSS.setAutoPVT(true);              
    myGNSS.saveConfiguration();           
}

void GPSManager::poll() {
    if (myGNSS.getPVT()) {  
        logGPS();
    }
}

void GPSManager::logGPS() {
    setLEDState(2, true);

    if (myGNSS.getFixType() >= 2) {
        setLEDMode(2, LED_ON); 
    } else {
        setLEDMode(2, LED_BLINK_SLOW);
    }

    GPSLogEntry gps;
    gps.lat_scaled = myGNSS.getLatitude();
    gps.lon_scaled = myGNSS.getLongitude();
    gps.alt_mm     = myGNSS.getAltitudeMSL();
    gps.fixType    = myGNSS.getFixType();
    gps.numSats    = myGNSS.getSIV();
    gps.reserved   = 0;

    LogPacketHeader hdr;
    hdr.timestamp_us = micros();
    hdr.source = SOURCE_GPS;
    hdr.length = sizeof(gps);

    loggerManager.addLogEntry((uint8_t*)&hdr, sizeof(hdr));
    loggerManager.addLogEntry((uint8_t*)&gps, sizeof(gps));
}
