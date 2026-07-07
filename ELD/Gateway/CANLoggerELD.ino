/***********************************************************************
 * CAN Logger ELD Prototype
 *
 * Main firmware entry point.
 *
 * Responsibilities:
 *   - Initialize all hardware and subsystems
 *   - Execute periodic subsystem updates
 *
 * Business logic belongs inside the respective modules.
 ***********************************************************************/

#include "setup.h"
#include "gps.h"
#include "wifi.h"
#include "eld.h"
#include "storage.h"
#include "crypto.h"
#include "leds.h"


/***********************************************************************
 * Setup
 ***********************************************************************/
void setup()
{
    setupELD();
}


/***********************************************************************
 * Main Loop
 ***********************************************************************/
void loop()
{
    delay(5);
}