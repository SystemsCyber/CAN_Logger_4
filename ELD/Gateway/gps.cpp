#include <Wire.h>
// GPS (u-blox SAM-M10Q)
#include <SparkFun_u-blox_GNSS_v3.h>
SFE_UBLOX_GNSS myGNSS;

#define myWire Wire2
#define gnssAddress 0x42

void initGPS() {
  myWire.begin();
  Serial.println("Wire Started");
  if (!myGNSS.begin(Wire2)) {
    Serial.println("u-blox GNSS not detected on I2C. Check wiring!");
    while (1);
  }
  myGNSS.setI2COutput(COM_TYPE_UBX); // Disable NMEA, UBX only
  return true;
}

bool collectGPS(GPSData &gps)
{
    if (!myGNSS.getPVT())
    {
        gps.valid = false;
        return false;
    }

    gps.valid = true;

    // UTC Date/Time
    gps.year       = myGNSS.getYear();
    gps.month      = myGNSS.getMonth();
    gps.day        = myGNSS.getDay();

    gps.hour       = myGNSS.getHour();
    gps.minute     = myGNSS.getMinute();
    gps.second     = myGNSS.getSecond();

    gps.nanosecond = myGNSS.getNanosecond();

    // Position
    gps.latitude   = myGNSS.getLatitude();
    gps.longitude  = myGNSS.getLongitude();
    gps.altitude   = myGNSS.getAltitudeMSL();

    // Motion
    gps.speed      = myGNSS.getGroundSpeed();
    gps.heading    = myGNSS.getHeading();

    gps.satellites = myGNSS.getSIV();

    return true;
}