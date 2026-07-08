#include "eld.h"

static void processPGN65217(const CANMessageLog &msg);
static void processPGN65253(const CANMessageLog &msg);
static void processPGN65260(const CANMessageLog &msg);
static void processPGN61444(const CANMessageLog &msg);
static void processPGN65265(const CANMessageLog &msg);

bool initELD(void)
{
    return true;
}

void eldUpdate(void)
{
    // Future:
    // - Duty status
    // - Driver login
    // - RODS generation
    // - Diagnostics
}

void eldProcessCANMessage(const CANMessageLog &msg)
{
    switch (msg.pgn)
    {
        case 65217:
            processPGN65217(msg);
            break;

        case 65253:
            processPGN65253(msg);
            break;

        case 65260:
            processPGN65260(msg);
            break;

        case 61444:
            processPGN61444(msg);
            break;

        case 65265:
            processPGN65265(msg);
            break;

        default:
            break;
    }
}

static void processPGN65217(const CANMessageLog &msg)
{
    if (msg.len < 4)
        return;

    uint32_t raw =
          ((uint32_t)msg.buf[0])
        | ((uint32_t)msg.buf[1] << 8)
        | ((uint32_t)msg.buf[2] << 16)
        | ((uint32_t)msg.buf[3] << 24);

    vehicle.validOdometer = true;

    // meters
    vehicle.odometer = raw * 5;
}

static void processPGN65253(const CANMessageLog &msg)
{
    if (msg.len < 4)
        return;

    uint32_t raw =
          ((uint32_t)msg.buf[0])
        | ((uint32_t)msg.buf[1] << 8)
        | ((uint32_t)msg.buf[2] << 16)
        | ((uint32_t)msg.buf[3] << 24);

    vehicle.validEngineHours = true;

    vehicle.engineHours = raw * 0.05f;
}

static void processPGN65260(const CANMessageLog &msg)
{
    //
    // VIN uses the J1939 Transport Protocol.
    //
    // Later:
    //   RTS
    //   CTS
    //   TP.DT Assembly
    //
}

static void processPGN61444(const CANMessageLog &msg)
{
    if (msg.len < 8)
        return;

    vehicle.validStarterMode = true;

    // Placeholder
    // Replace once SPN layout is verified from the Digital Annex
    vehicle.starterMode = (msg.buf[6] >> 0) & 0x0F;
}

static void processPGN65265(const CANMessageLog &msg)
{
    if (msg.len < 8)
        return;

    vehicle.validVIN = true;

    // Placeholder
    // Replace once SPN layout is verified from the Digital Annex
    memcpy(vehicle.vin, msg.buf, 17);
    vehicle.vin[17] = '\0';
}

