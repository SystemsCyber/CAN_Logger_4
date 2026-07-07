#ifndef ELD_H
#define ELD_H

#include "structures.h"

// Initialize ELD subsystem
bool initELD(void);

// Periodic ELD processing
void eldUpdate(void);

// Process a J1939 message received from CAN
void eldProcessCANMessage(const CANMessageLog &msg);

#endif