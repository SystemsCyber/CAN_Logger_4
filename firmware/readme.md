# CAN Logger — Teensy 4.1 + Multi-Bus + WiFi File Server

A modular, high-speed CAN logging system for Teensy 4.1 that supports:

- Logging from **4 CAN interfaces** (CAN1–3 via FlexCAN, CAN4 via MCP2517FD)
- **GPS** (u-blox SAM-M10Q) logging over I2C
- **IMU** (ASM330LHHTR) logging via raw I2C register reads
- Optional **Ethernet frame logging**
- Binary log format with **double-buffered block writes** to SD
- Two-button control (start/stop)
- Lightweight **HTTP-based FTP interface** over WiFi
- Activity/status displayed via **5 status LEDs**

---

## Project Structure

can_logger/
├── can_logger.ino # Main sketch — glue logic
├── Config.h # Centralized pinout and config
├── include/
│ └── DataStructs.h # Binary log record definitions
├── logger/
│ ├── LoggerManager.cpp/.h # Logging FSM and buffers
│ ├── DataLogger.cpp/.h # SD writes and file rotation
├── can/
│ └── CANManager.cpp/.h # CAN1–4 handling
├── sensors/
│ ├── GPSManager.cpp/.h # GNSS via SparkFun u-blox lib
│ ├── IMUManager.cpp/.h # ASM330LHHTR via raw I2C
├── net/
│ └── FTPServer.cpp/.h # HTTP file server + pause logging
├── utils/
│ └── LEDStatus.cpp/.h # LED blinking logic


---

## Hardware Pinout

| Function      | Pin           |
|---------------|---------------|
| LED_CAN1      | 9             |
| LED_CAN2      | 32            |
| LED_GPS       | 14            |
| LED_WIFI      | 15            |
| LED_STATUS    | LED_BUILTIN   |
| Button Start  | 36 (INPUT_PULLUP) |
| Button Stop   | 37 (INPUT_PULLUP) |
| WiFi SPI      | MOSI 11, MISO 12, SCK 13, CS 10 |
| MCP2517FD SPI | MOSI1 26, MISO1 39, SCK1 27, CS 38, INT 6 |
| GNSS + IMU    | I2C (Wire) shared |

---

## Build Requirements

Use Arduino IDE or PlatformIO with these libraries:

- [`FlexCAN_T4`](https://github.com/tonton81/FlexCAN_T4)
- [`ACAN2517FD`](https://github.com/pierremolinaro/acan2517fd)
- [`SparkFun u-blox GNSS v3`](https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library)
- [`WiFi101`](https://www.arduino.cc/en/Reference/WiFi101)

---

## Usage

### Button Controls

- **Start (Pin 36):** Begins logging to `log_000.bin`, `log_001.bin`, etc.
- **Stop (Pin 37):** Stops logging and flushes buffers

### Log Format

Each record is:

```
struct LogPacketHeader {
    uint32_t timestamp_us;
    uint8_t source;
    uint16_t length;
};

// Followed by CANLogEntry, GPSLogEntry, or IMULogEntry
```

See include/DataStructs.h for all supported log entry types.

## WiFi Log Access

1. Teensy starts a WiFi Access Point:
   - **SSID:** `CanLogger4`
   - **Password:** `CanLogger4`

2. Connect to the access point from your PC or phone.

3. Open your browser and go to: http://192.168.1.1


*(or the IP returned by `WiFi.localIP()` if different)*

4. You'll see a list of `.bin` log files. Click to download.

> **Note:** Logging is **paused** during file transfer and resumes automatically after the client disconnects.


## LED Indicators

| LED Index | Pin         | Function         | Behavior                       |
|-----------|-------------|------------------|--------------------------------|
| 0         | 9           | CAN1–3 activity  | Blink on message               |
| 1         | 32          | CAN4 activity    | Blink on message               |
| 2         | 14          | GPS / IMU        | Solid = GPS fix, Blink = none  |
| 3         | 15          | WiFi file server | Blink = idle, Solid = transfer |
| 4         | LED_BUILTIN | Optional status  | Reserved for debug/heartbeat   |

## Log Storage

- Logs are saved as `log_000.bin`, `log_001.bin`, etc.
- Maximum file size: **512 MB** (defined in `Config.h`)
- Uses **double buffering** to prevent data loss during slow SD writes
- When a file exceeds the size limit, the next numbered file is opened automatically

## Advanced Features (To Do)

- Add CRC or magic sync header to each log record
- Add support for Ethernet frame logging (if needed)
- Add serial command interface for runtime control
- Implement timestamp accuracy via GPS PPS or RTC
- Add dynamic configuration loading from SD (e.g., JSON or INI)

## Notes

- Codebase is fully modular — each subsystem is implemented in `.cpp/.h` files
- `can_logger.ino` only handles setup, loop, and delegating control
- Compatible with both **Arduino IDE** and **PlatformIO**
- Logging system is non-blocking and double-buffered to avoid SD latency issues
- WiFi server uses a minimal HTTP file browser — simple and robust
