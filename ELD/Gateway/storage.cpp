#include <SD.h>
const int chipSelect = BUILTIN_SDCARD;

bool initStorage()
{
  if (!SD.begin(chipSelect)) {
    #ifdef DEBUG
      Serial.println("Card failed or not present.");
    #endif
    while (1);
  }
  return true;
}
