#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// === Logger Parameters ===
#define LOG_BUFFER_SIZE        8192     // per buffer (2 buffers total)
#define MAX_LOG_FILE_SIZE      (512UL * 1024UL * 1024UL) // 512 MB
#define LOG_FLUSH_INTERVAL_MS  100

// === CAN ===
#define CAN_BAUDRATE           500000   // common across all 4 CANs

// === SPI (WiFi/SD) Pins ===
#define SPI_MISO_PIN           12
#define SPI_MOSI_PIN           11
#define SPI_SCK_PIN            13
#define SPI_CS_SD              10       // shared with SD
#define WIFI_CS_PIN            10       // WiFi101
#define WIFI_IRQ_PIN           33
#define WIFI_RESET_PIN         34
#define WIFI_EN_PIN            35

// === IMU (ASM330LHHTR) ===
#define IMU_I2C_ADDR           0x6B

// === LED Pins ===
#define LED_CAN1               9
#define LED_CAN2               32
#define LED_GPS                14
#define LED_WIFI               15
#define LED_STATUS             LED_BUILTIN  // optional

// === Button Pins ===
#define BUTTON_START_PIN       36
#define BUTTON_STOP_PIN        37
#define BUTTON_DEBOUNCE_MS     50

// === FTP Server ===
#define FTP_SERVER_PORT        80
#define FTP_SSID               "CanLogger4"
#define FTP_PASS               "CanLogger4"

#endif // CONFIG_H
