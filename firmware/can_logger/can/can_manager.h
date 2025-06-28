#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include <Arduino.h>

class CANManager {
public:
    void begin();
    void poll(); 

private:
    void initCAN1to3();
    void initCAN4();

    void pollCAN4();

    void logFrame(uint8_t sourceIndex, uint32_t id, uint8_t dlc, const uint8_t* data);
};

extern CANManager canManager;

#endif 
