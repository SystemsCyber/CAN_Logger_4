#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <Arduino.h>

#pragma pack(push, 1) 

enum LogSource : uint8_t {
    SOURCE_CAN1 = 0,
    SOURCE_CAN2,
    SOURCE_CAN3,
    SOURCE_CAN4,
    SOURCE_GPS,
    SOURCE_IMU,
    SOURCE_ETH
};

struct LogPacketHeader {
    uint32_t timestamp_us;  
    uint8_t source;        
    uint16_t length;      
};

// --- CAN Frame ---
struct CANLogEntry {
    uint32_t can_id;
    uint8_t dlc;
    uint8_t data[64];  
};

// --- GPS ---
struct GPSLogEntry {
    int32_t lat_scaled;    
    int32_t lon_scaled;    
    int32_t alt_mm;        
    uint8_t fixType;        
    uint8_t numSats;
    uint16_t reserved;     
};

// --- IMU ---
struct IMULogEntry {
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
};

// --- Ethernet Frame (raw) ---
struct ETHLogEntry {
    uint16_t frame_len;
    uint8_t frame_data[1500];
};

#pragma pack(pop)

#endif
