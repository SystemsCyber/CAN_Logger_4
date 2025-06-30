#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>
#include <Wire.h>  

class GPSManager {
public:
    void begin(TwoWire& wire = Wire);
    void poll(); 

private:
    uint32_t lastFixTime = 0;
    void logGPS();
};

extern GPSManager gpsManager;

#endif
