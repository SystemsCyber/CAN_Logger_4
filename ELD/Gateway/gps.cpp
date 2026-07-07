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