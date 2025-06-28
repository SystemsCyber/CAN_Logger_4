#ifndef IMU_MANAGER_H
#define IMU_MANAGER_H

#include <Arduino.h>

class IMUManager {
public:
    void begin(TwoWire& wire = Wire);
    void poll(); 
private:
    void logIMU();
    void readBytes(uint8_t startReg, uint8_t* buffer, uint8_t len);
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    int16_t toInt16(uint8_t lsb, uint8_t msb);

    TwoWire* i2c;
    static constexpr uint8_t IMU_ADDR = 0x6B;
};

extern IMUManager imuManager;

#endif 
