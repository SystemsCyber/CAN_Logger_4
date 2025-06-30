#include "imu_manager.h"
#include "../logger/logger_manager.h"
#include "../include/data_structs.h"
#include "../utils/led_status.h"

#define WHO_AM_I_REG     0x0F
#define CTRL1_XL         0x10
#define CTRL2_G          0x11
#define CTRL3_C          0x12
#define OUTX_L_G         0x22
#define OUTX_L_A         0x28

IMUManager imuManager;

void IMUManager::begin(TwoWire& wire) {
    i2c = &wire;
    i2c->begin();
    delay(100);

    uint8_t whoami = readRegister(WHO_AM_I_REG);
    if (whoami != 0x6B) {
        Serial.print("Unexpected IMU WHO_AM_I: 0x");
        Serial.println(whoami, HEX);
        return;
    }

    writeRegister(CTRL3_C, 0x01);
    delay(100);

    writeRegister(CTRL1_XL, 0x40);

    writeRegister(CTRL2_G, 0x40);

    Serial.println("ASM330LHHTR initialized");
}

void IMUManager::poll() {
    static elapsedMillis timer;
    if (timer < 50) return;
    timer = 0;

    uint8_t accelData[6], gyroData[6];
    readBytes(OUTX_L_A, accelData, 6);
    readBytes(OUTX_L_G, gyroData, 6);

    IMULogEntry entry;
    entry.accelX = toInt16(accelData[0], accelData[1]);
    entry.accelY = toInt16(accelData[2], accelData[3]);
    entry.accelZ = toInt16(accelData[4], accelData[5]);

    entry.gyroX  = toInt16(gyroData[0], gyroData[1]);
    entry.gyroY  = toInt16(gyroData[2], gyroData[3]);
    entry.gyroZ  = toInt16(gyroData[4], gyroData[5]);

    LogPacketHeader hdr;
    hdr.timestamp_us = micros();
    hdr.source = SOURCE_IMU;
    hdr.length = sizeof(entry);

    loggerManager.addLogEntry((uint8_t*)&hdr, sizeof(hdr));
    loggerManager.addLogEntry((uint8_t*)&entry, sizeof(entry));

    setLEDState(2, true);
}

void IMUManager::writeRegister(uint8_t reg, uint8_t value) {
    i2c->beginTransmission(IMU_ADDR);
    i2c->write(reg);
    i2c->write(value);
    i2c->endTransmission();
}

uint8_t IMUManager::readRegister(uint8_t reg) {
    i2c->beginTransmission(IMU_ADDR);
    i2c->write(reg);
    i2c->endTransmission(false);
    i2c->requestFrom(IMU_ADDR, (uint8_t)1);
    return i2c->available() ? i2c->read() : 0xFF;
}

void IMUManager::readBytes(uint8_t startReg, uint8_t *buffer, uint8_t len) {
    i2c->beginTransmission(IMU_ADDR);
    i2c->write(startReg);
    i2c->endTransmission(false);
    i2c->requestFrom(IMU_ADDR, len);
    for (uint8_t i = 0; i < len && i2c->available(); i++) {
        buffer[i] = i2c->read();
    }
}

int16_t IMUManager::toInt16(uint8_t lsb, uint8_t msb) {
    return (int16_t)((msb << 8) | lsb);
}
