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
    bool validSpeed;

    uint32_t odometer;          // meters
    float engineHours;
    float speed;              // km/h

    uint8_t starterMode;

    char vin[18];
};

/***********************************************************************
 * API Data
 ***********************************************************************/

struct APIHeader
{
    uint16_t magic;
    uint16_t length;
    uint8_t  version;
    uint8_t  command;
};

enum APICommand
{
    CMD_PING                = 0x01,
    CMD_LOGIN               = 0x02,
    CMD_LOGOUT              = 0x03,
    CMD_DRIVER_INFO         = 0x04,
    CMD_SET_DUTY_STATUS     = 0x05,
    CMD_SET_SHIPPING_ID     = 0x06,
    CMD_CERTIFY             = 0x07,
    CMD_GET_STATUS          = 0x08,
    CMD_GET_RODS            = 0x09,
    CMD_GET_EVENTS          = 0x0A,
    CMD_GET_DIAGNOSTICS     = 0x0B,
    CMD_TRANSFER            = 0x0C,
    CMD_TIMEZONE            = 0x0D
};

struct LoginRequest
{
    char username[32];
    uint8_t token[32];
    uint32_t nonce;
}

struct DriverInfo
{
    char firstName[32];
    char lastName[32];
    char username[32];
    char licenseNumber[32];
    char licenseState[3];
};

enum DutyStatusType
{
    DUTY_OFF_DUTY         = 0,
    DUTY_SLEEPER          = 1,
    DUTY_DRIVING          = 2,
    DUTY_ON_DUTY          = 3
};

struct ShippingDocument
{
    char id[32];
}

#endif