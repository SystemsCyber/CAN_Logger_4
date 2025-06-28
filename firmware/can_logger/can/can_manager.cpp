#include "CANManager.h"
#include "DataStructs.h"
#include "LoggerManager.h"
#include "LEDStatus.h"

#include <FlexCAN_T4.h>
#include <ACAN2517FD.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;

const byte MCP2517_SCK = 27;
const byte MCP2517_SDI = 26;
const byte MCP2517_SDO = 39;
const byte MCP2517_CS  = 38;
const byte MCP2517_INT = 6;
ACAN2517FD can4(MCP2517_CS, SPI1, MCP2517_INT);

CANManager canManager;

void CANManager::begin() {
    SPI1.setMOSI(MCP2517_SDI);
    SPI1.setMISO(MCP2517_SDO);
    SPI1.setSCK(MCP2517_SCK);
    SPI1.begin();

    initCAN1to3();
    initCAN4();
}

void CANManager::initCAN1to3() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.setMaxMB(16);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.onReceive([](const CAN_message_t &msg) {
        canManager.logFrame(SOURCE_CAN1, msg.id, msg.len, msg.buf);
        setLEDState(0, true);
    });

    can2.begin();
    can2.setBaudRate(500000);
    can2.setMaxMB(16);
    can2.enableFIFO();
    can2.enableFIFOInterrupt();
    can2.onReceive([](const CAN_message_t &msg) {
        canManager.logFrame(SOURCE_CAN2, msg.id, msg.len, msg.buf);
        setLEDState(1, true);
    });

    can3.begin();
    can3.setBaudRate(500000);
    can3.setMaxMB(16);
    can3.enableFIFO();
    can3.enableFIFOInterrupt();
    can3.onReceive([](const CAN_message_t &msg) {
        canManager.logFrame(SOURCE_CAN3, msg.id, msg.len, msg.buf);
        setLEDState(2, true);
    });
}

void CANManager::initCAN4() {
    ACAN2517FDSettings settings(
        ACAN2517FDSettings::OSC_40MHz,
        500000,
        DataBitRateFactor::x1
    );
    settings.mRequestedMode = ACAN2517FDSettings::NormalMode;

    uint32_t errorCode = can4.begin(settings, [] { can4.isr(); });
    if (errorCode != 0) {
        Serial.print("CAN4 config error 0x");
        Serial.println(errorCode, HEX);
    } else {
        Serial.println("CAN4 initialized");
    }
}

void CANManager::poll() {
    pollCAN4();
}

void CANManager::pollCAN4() {
    CANMessage frame;
    while (can4.available()) {
        can4.receive(frame);
        logFrame(SOURCE_CAN4, frame.id, frame.len, frame.data);
        setLEDState(3, true);
    }
}

void CANManager::logFrame(uint8_t sourceIndex, uint32_t id, uint8_t dlc, const uint8_t* data) {
    CANLogEntry entry;
    entry.can_id = id;
    entry.dlc = dlc;
    memcpy(entry.data, data, dlc);

    LogPacketHeader hdr;
    hdr.timestamp_us = micros();
    hdr.source = sourceIndex;
    hdr.length = sizeof(entry);

    loggerManager.addLogEntry((uint8_t*)&hdr, sizeof(hdr));
    loggerManager.addLogEntry((uint8_t*)&entry, sizeof(entry));
}
