#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <Arduino.h>

/***********************************************************************
 * GPS Data
 ***********************************************************************/
struct GPSData
{
    bool valid;

    // UTC Time
    uint16_t year;
    uint8_t month;
    uint8_t day;

    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint32_t nanosecond;

    // Position
    int32_t latitude;      // degrees × 10^7
    int32_t longitude;     // degrees × 10^7
    int32_t altitude;      // mm above mean sea level

    // Motion
    uint32_t speed;        // mm/s
    int32_t heading;       // degrees × 10^-5

    uint8_t satellites;
};

/***********************************************************************
 * CAN Data
 ***********************************************************************/
struct CANMessageLog
{
    uint64_t timestamp;
    uint32_t id;
    uint32_t pgn;
    uint8_t priority;
    uint8_t source;
    uint8_t destination;
    uint8_t len;
    uint8_t buf[8];
    uint8_t extended;
    uint8_t overrun;
    uint8_t mailbox;
};

/***********************************************************************
 * ELD Data
 ***********************************************************************/
struct VehicleData
{
    bool validOdometer;
    bool validEngineHours;
    bool validVIN;
    bool validStarterMode;

    uint32_t odometer;          // meters
    float engineHours;

    uint8_t starterMode;

    char vin[18];
};

#endif