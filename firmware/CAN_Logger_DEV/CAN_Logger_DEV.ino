// ========================================= Includes ===============================================================//

// Arduino
#include <Arduino.h>

// SD Card
#include <SD.h>
const int chipSelect = BUILTIN_SDCARD;

// File
#define MAX_FILE_SIZE 512UL * 1024 * 1024
File logFile;
String currentFilename;
uint32_t fileCounter = 0;


// Logging Flag
bool loggingActive = false;


// IMU (ASM330LHHTR via I2C)
#include <Wire.h>
// Register addresses
#define IMU_ADDR 0x6B
#define WHO_AM_I_REG     0x0F
#define CTRL1_XL         0x10  // accel config
#define CTRL2_G          0x11  // gyro config
#define CTRL3_C          0x12  // common control
#define OUTX_L_G         0x22  // gyro X low byte
#define OUTX_L_A         0x28  // accel X low byte

// WiFi module (ATWINC1500)
#include <SPI.h>
#include <WiFi101.h>
#define WIFI_MISO              12
#define WIFI_MOSI              11 
#define WIFI_CS                10
#define WIFI_SCK               13  
#define WIFI_IRQ               33
#define WIFI_RST               34
#define WIFI_EN                35
char ssid[] = "CanLogger4";
char pass[] = "CanLogger4";
int status = WL_IDLE_STATUS;
WiFiServer server(80);

// CAN1–CAN3 (Teensy Internal)
#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;
CAN_message_t msg1;
CAN_message_t msg2;
CAN_message_t msg3;
#define MAX_CAN_MSGS_PER_SECTION 50
struct CANMessageLog {
  uint64_t timestamp;
  uint32_t id;
  uint8_t len;
  uint8_t buf[8];
  uint8_t extended;
  uint8_t overrun;
  uint8_t mailbox;
};
struct CANErrorEntry {
  uint32_t timestamp;
  uint8_t busID;   
  uint8_t errorFlags;
  uint8_t txErr;
  uint8_t rxErr;
};
CANMessageLog can1Log[MAX_CAN_MSGS_PER_SECTION];
uint8_t can1LogCount = 0;
CANMessageLog can2Log[MAX_CAN_MSGS_PER_SECTION];
uint8_t can2LogCount = 0;
CANMessageLog can3Log[MAX_CAN_MSGS_PER_SECTION];
uint8_t can3LogCount = 0;

// CAN4 (MCP2517FD via SPI1)
#include <ACAN2517FD.h>
const byte MCP2517_SCK  = 27;
const byte MCP2517_SDI  = 26;
const byte MCP2517_SDO  = 39;
const byte MCP2517_CS   = 38;
const byte MCP2517_INT  = 6;
ACAN2517FD can4(MCP2517_CS, SPI1, MCP2517_INT);
CANMessage msg4 ;

// GPS (u-blox SAM-M10Q)
#include <SparkFun_u-blox_GNSS_v3.h>
SFE_UBLOX_GNSS myGNSS;
#define myWire Wire2
#define gnssAddress 0x42

// Ethernet
//#include <NativeEthernet.h>

// RTC
#include <TimeLib.h>
#include <time.h>
#define TIME_HEADER  "T" 
const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
elapsedMicros us_since_last_rtc;
uint32_t last_rtc_secs = 0;

// === Pin Definitions ===

// Logging Control Switches
const int switchPin1 = 36; // Start Logging
const int switchPin2 = 37; // Stop Logging
bool lastState1 = HIGH;
bool lastState2 = HIGH;

// Status LEDs
const int ledPinB = 14; // Blue
const int ledPinY = 15; // Yellow
const int ledPinG = 9;  // Green
const int ledPinR = 32; // Red
bool ledG = false;
uint32_t canTime = 0;
//const int ledBuiltIn = LED_BUILTIN;

// Terminating Resistor Control (ADG1612)
const int switchPins[] = {2, 3, 4, 5}; // IN1–IN4 of analog switch

// Data Buffers
#define BUFFER_SIZE 4096
const size_t BUFFER_FLUSH_THRESHOLD = BUFFER_SIZE - 512;
volatile bool flushRequested = false;

uint32_t calculateCRC32(const uint8_t* data, size_t len) {
  uint32_t crc = 0xFFFFFFFF;
  while (len--) {
    uint8_t byte = *data++;
    for (uint8_t i = 0; i < 8; i++) {
      uint32_t mix = (crc ^ byte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0xEDB88320;
      byte >>= 1;
    }
  }
  return ~crc;
}

enum SectionID : uint8_t {
  SECTION_CAN1 = 0x01,
  SECTION_CAN2 = 0x02,
  SECTION_CAN3 = 0x03,
  SECTION_CAN4 = 0x04,
  SECTION_GPS  = 0x05,
  SECTION_IMU  = 0x06,
  SECTION_LIN  = 0x07,
  SECTION_J1708 = 0x08,
  SECTION_CAN_ERROR = 0x09,
};
struct __attribute__((packed))BufferHeader{
  uint8_t magic = 0xA5;        // For sanity check
  uint32_t timestamp;          // RTC timestamp
  uint8_t sectionCount = 0;    // Filled dynamically
};
struct __attribute__((packed)) SectionHeader {
  SectionID id;
  uint16_t length;             // Length of following section data
};
struct BufferBuilder {
  uint8_t data[BUFFER_SIZE];
  size_t offset = 0;
  uint8_t sectionCount = 0;

  void begin(uint32_t timestamp) {
//    #ifdef DEBUG
//  Serial.println("==== Begin buffer ====");
//  for (int i = 0; i < 8; i++) {
//    Serial.printf("PRE[%d] = 0x%02X\n", i, data[i]);  // show old contents
//  }
//    #endif
    offset = 0;
    sectionCount = 0;
  
    memset(data, 0, sizeof(data)); 
  
    BufferHeader hdr{}; // Zero-initialize all fields
    hdr.magic = 0xA5;
    hdr.timestamp = timestamp;
    hdr.sectionCount = 0; // Optional, already zero from {}
  
    memcpy(data + offset, &hdr, sizeof(hdr));
    offset += sizeof(hdr);

//    #ifdef DEBUG
//  
//    Serial.println("==== After header ====");
//    for (int i = 0; i < 8; i++) {
//         Serial.printf("POST[%d] = 0x%02X\n", i, data[i]);
//
//    #endif
    }

  bool addSection(SectionID id, const void* src, uint16_t len) {
    if (offset + sizeof(SectionHeader) + len + 4 > BUFFER_SIZE)
      return false;  // Not enough room

    SectionHeader shdr;
    shdr.id = id;
    shdr.length = len;

    memcpy(data + offset, &shdr, sizeof(shdr));
    offset += sizeof(shdr);

    memcpy(data + offset, src, len);
    offset += len;

    sectionCount++;
    return true;
  }

  void finalize() {
    BufferHeader* hdr = reinterpret_cast<BufferHeader*>(data);
    hdr->sectionCount = sectionCount;
//    #ifdef DEBUG
  //    Serial.print("Offset before CRC: ");
  //    Serial.println(offset);
//    #endif

    uint32_t crc = calculateCRC32(data, offset);
    memcpy(data + offset, &crc, sizeof(crc));
    offset += sizeof(crc);
  }
};
BufferBuilder bufferA;
BufferBuilder bufferB;
BufferBuilder* currentBuffer = &bufferA;
BufferBuilder* flushBuffer = &bufferB;

// Debug
#define DEBUG


// ========================================= Includes ===============================================================//

// ========================================= Setup Helper Functions =================================================//

void setupTerminationSwitches() {
  for (int i = 0; i < 4; i++) {
    pinMode(switchPins[i], OUTPUT);
    digitalWrite(switchPins[i], HIGH); // Default to termination ON
  }
}

void setupLoggingControlSwitches() {
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(switchPin2, INPUT_PULLUP);
}

void setupLEDs() {
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinY, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  // pinMode(ledPinR, OUTPUT);
  digitalWrite(ledPinB, HIGH);
  digitalWrite(ledPinY, HIGH);
  digitalWrite(ledPinG, HIGH);
  // digitalWrite(ledPinR, HIGH);
  delay(1000);
  digitalWrite(ledPinB, LOW);
  digitalWrite(ledPinY, LOW);
  digitalWrite(ledPinG, LOW);
  delay(1000);
  digitalWrite(ledPinB, HIGH);
  digitalWrite(ledPinY, HIGH);
  digitalWrite(ledPinG, HIGH);
  // digitalWrite(ledPinR, HIGH);
  delay(1000);
  digitalWrite(ledPinB, LOW);
  digitalWrite(ledPinY, LOW);
  digitalWrite(ledPinG, LOW);
}

uint64_t getMicroTimestamp() {
  uint32_t now_rtc = Teensy3Clock.get();
  uint32_t micros_now = us_since_last_rtc;

  if (now_rtc != last_rtc_secs) {
    us_since_last_rtc = 0;
    last_rtc_secs = now_rtc;
    micros_now = 0;
  }

  return static_cast<uint64_t>(last_rtc_secs) * 1000000ULL + micros_now;
}

void setupWifi()
{
  SPI.setMISO(WIFI_MISO);
  SPI.setMOSI(WIFI_MOSI);
  SPI.setCS(WIFI_CS);
  SPI.setSCK(WIFI_SCK);
  SPI.begin();
  WiFi.setPins(WIFI_CS, WIFI_IRQ, WIFI_RST, WIFI_EN);
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    // #ifdef DEBUG 
    //   Serial.println("WiFi shield not present");
    // #endif
    // don't continue:
    while (true);
  }
  // attempt to create access point
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    // #ifdef DEBUG
    //   Serial.println("Creating access point failed");
    // #endif
    // don't continue
    while (true);
  }
  delay(10000);
  server.begin();
  // Serial.println("Wifi Setup Successful");
}

uint32_t can1_autobaud() 
{
  const uint32_t CAN_BAUD_RATES[] = {125000, 250000, 500000, 1000000};
  const int NUM_CAN_BAUDS = sizeof(CAN_BAUD_RATES) / sizeof(CAN_BAUD_RATES[0]);
  for (int i = 0; i < NUM_CAN_BAUDS; ++i) {
    can1.begin();
    can1.setBaudRate(CAN_BAUD_RATES[i]);
    delay(10);
    unsigned long start = millis();
    while (millis() - start < 200) {
      if (can1.read(msg1)) {
        return CAN_BAUD_RATES[i];
      }
    }
    can1.reset(); 
  }
  return 0;
}

uint32_t can2_autobaud() 
{
  const uint32_t CAN_BAUD_RATES[] = {125000, 250000, 500000, 1000000};
  const int NUM_CAN_BAUDS = sizeof(CAN_BAUD_RATES) / sizeof(CAN_BAUD_RATES[0]);
  for (int i = 0; i < NUM_CAN_BAUDS; ++i) {
    can2.begin();
    can2.setBaudRate(CAN_BAUD_RATES[i]);
    delay(10);
    unsigned long start = millis();
    while (millis() - start < 200) {
      if (can2.read(msg2)) {
        return CAN_BAUD_RATES[i];
      }
    }
    can2.reset(); 
  }
  return 0;
}

uint32_t can3_autobaud() 
{
  const uint32_t CAN_BAUD_RATES[] = {125000, 250000, 500000, 1000000};
  const int NUM_CAN_BAUDS = sizeof(CAN_BAUD_RATES) / sizeof(CAN_BAUD_RATES[0]);
  for (int i = 0; i < NUM_CAN_BAUDS; ++i) {
    can3.begin();
    can3.setBaudRate(CAN_BAUD_RATES[i]);
    delay(10);
    unsigned long start = millis();
    while (millis() - start < 200) {
      if (can3.read(msg3)) {
        return CAN_BAUD_RATES[i];
      }
    }
    can3.reset(); 
  }
  return 0;
}

void can1Sniff(const CAN_message_t &msg1){
  if (can1LogCount >= MAX_CAN_MSGS_PER_SECTION-1) {
    flushRequested = true;
    return;
  }

  CANMessageLog &entry = can1Log[can1LogCount++];
  entry.timestamp = getMicroTimestamp();
//  Serial.println((uint64_t)entry.timestamp);
  entry.id = msg1.id;
  entry.len = msg1.len;
  memcpy(entry.buf, msg1.buf, msg1.len);
  entry.extended = msg1.flags.extended;
  entry.overrun = msg1.flags.overrun;
  entry.mailbox = msg1.mb;
  if (millis() - canTime >= 500){
    if (!ledG){
      digitalWrite(ledPinG, HIGH);
      ledG = true;
    }
    else{
      digitalWrite(ledPinG, LOW);
      ledG = false;
    }
    canTime = millis();
  }
}

void can2Sniff(const CAN_message_t &msg2){
  if (can2LogCount >= MAX_CAN_MSGS_PER_SECTION-1) {
    flushRequested = true;
    return;
  }
  
  CANMessageLog &entry = can2Log[can2LogCount++];
  entry.timestamp = getMicroTimestamp();
  entry.id = msg2.id;
  entry.len = msg2.len;
  memcpy(entry.buf, msg2.buf, msg2.len);
  entry.extended = msg2.flags.extended;
  entry.overrun = msg2.flags.overrun;
  entry.mailbox = msg2.mb;
  if (millis() - canTime >= 500){
    if (!ledG){
      digitalWrite(ledPinG, HIGH);
      ledG = true;
    }
    else{
      digitalWrite(ledPinG, LOW);
      ledG = false;
    }
    canTime = millis();
  }
  // if (flushRequested){return;}
}

//void can3Sniff(const CAN_message_t &msg3){
//  if (can3LogCount >= MAX_CAN_MSGS_PER_SECTION-1) {
//    flushRequested = true;
//  }
//
//  CANMessageLog &entry = can3Log[can3LogCount++];
//  entry.timestamp = getMicroTimestamp();
//  entry.id = msg3.id;
//  entry.len = msg3.len;
//  memcpy(entry.buf, msg3.buf, msg3.len);
//  entry.extended = msg3.flags.extended;
//  entry.overrun = msg3.flags.overrun;
//  entry.mailbox = msg3.mb;
//  if (flushRequested){return;}
//}

void setupCAN()
{
  // CAN1 Setup
  uint32_t baudRate = 0;
  baudRate = can1_autobaud();
  if (baudRate != 0){
    can1.begin(); 
    can1.setBaudRate(baudRate);
    can1.setMaxMB(16);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.onReceive(can1Sniff);
  }
  // CAN2 Setup
  baudRate = 0;
  baudRate = can2_autobaud();
  if (baudRate !=0){
    can2.begin();
    can2.setBaudRate(baudRate);
    can2.setMaxMB(16);
    can2.enableFIFO();
    can2.enableFIFOInterrupt();
    can2.onReceive(can2Sniff);
  }
  // CAN3 Setup
//  baudRate = can3_autobaud();
//  if (baudRate !=0){
//    can3.begin();
//    can3.setBaudRate(baudRate);
//    can3.setMaxMB(16);
//    can3.enableFIFO();
//    can3.enableFIFOInterrupt();
//    can3.onReceive(can3Sniff);
//    Serial.println("Can3 Setup");
//  }
  // CAN4 Setup
  SPI1.setMOSI (MCP2517_SDI);
  SPI1.setMISO (MCP2517_SDO);
  SPI1.setSCK (MCP2517_SCK);
  SPI1.begin ();
  ACAN2517FDSettings settings (
      ACAN2517FDSettings::OSC_40MHz,
      500 * 1000, // arbitration bitrate
      DataBitRateFactor::x1 // no CAN FD
  );
  const uint32_t errorCode = can4.begin (settings, [] { can4.isr () ; }) ;
  // #ifdef DEBUG
  //   if (errorCode == 0) {
  //     Serial.println ("CAN4 Configuration ok") ;
  //   }
  //   else{
  //     Serial.print ("CAN4 Configuration error 0x") ;
  //     Serial.println (errorCode, HEX) ;
  //   }
  // #endif
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(IMU_ADDR, 1);
  return Wire.available() ? Wire.read() : 0xFF;
}

void readBytes(uint8_t startReg, uint8_t *buffer, uint8_t len) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(startReg);
  Wire.endTransmission(false);
  Wire.requestFrom(IMU_ADDR, len);
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buffer[i] = Wire.read();
  }
}

int16_t toInt16(uint8_t lsb, uint8_t msb) {
  return (int16_t)((msb << 8) | lsb);
}

void setupIMU()
{
  Wire.begin();
  delay(100);
  uint8_t whoami = readRegister(WHO_AM_I_REG);
  // Serial.print("WHO_AM_I: 0x"); Serial.println(whoami, HEX);
  if (whoami != 0x6B) {
    // #ifdef DEBUG
    // Serial.println("Unexpected WHO_AM_I value");
    // #endif
    while (1);
  }
  // Reset device
  writeRegister(CTRL3_C, 0x01);
  delay(100);
  // Enable accelerometer
  writeRegister(CTRL1_XL, 0x40);
  // Enable gyro
  writeRegister(CTRL2_G, 0x40);
  // #ifdef DEBUG
  //   Serial.println("ASM330LHHTR initialized.");
  // #endif
}


time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}


// void setupRTC()
// {
//   setSyncProvider(getTeensy3Time);
//   if (timeStatus() != timeSet) {
//     if (Serial.available()) {
//     time_t t = processSyncMessage();
//     if (t != 0) {
//       time_t utc = t + (6 * 3600);
//       Teensy3Clock.set(utc);
//       setTime(utc);
//       // #ifdef DEBUG
//       //   Serial.println("Unable to sync with the RTC");
//       // #endif
//     }
//   }
//   } 
//   else {
//     // #ifdef DEBUG
//     //   Serial.print("RTC has set the system time: ");
//     // #endif
//   }
// }

void setupRTC()
{
    time_t rtc = Teensy3Clock.get();

    // Serial.print("RTC: ");
    // Serial.println(rtc);

    // setSyncProvider(getTeensy3Time);

    // if (rtc < 1700000000)
    // {
    //     // Serial.println("RTC invalid.");
    //   while(1);
    // }
    // else
    // {
    //     // Serial.println("RTC OK.");
    //     setTime(rtc);
    // }
}

void setupGPS() {
  myWire.begin();
  // Serial.println("Wire Started");
//  while (!myGNSS.begin(myWire)){//, gnssAddress)) {
//    #ifdef DEBUG
//      Serial.println(F("u-blox GNSS not detected. Retrying..."));
//    #endif
//    delay(1000);
//  }
  if (!myGNSS.begin(Wire2)) {
    // Serial.println("u-blox GNSS not detected on I2C. Check wiring!");
    while (1);
  }
  // Serial.println("Init GPS");
  myGNSS.setI2COutput(COM_TYPE_UBX); // Disable NMEA, UBX only
    #ifdef DEBUG
    // Serial.println("GPS initialization successful.");
  #endif
}

void setupSDCard()
{
  if (!SD.begin(chipSelect)) {
    #ifdef DEBUG
      // Serial.println("Card failed or not present.");
    #endif
    while (1); // Halt system
  }
  #ifdef DEBUG
    // Serial.println("SD initialization successful.");
  #endif
}

String generateTimestampedFilename() {
  tmElements_t tm;
  breakTime(Teensy3Clock.get(), tm); // Get current RTC time
  char filename[32]; // 8.3 filename + null
  snprintf(filename, sizeof(filename), "%02d%02d%02d_%02d%02d_%lu.BIN",
         tmYearToY2k(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, fileCounter++);


  return String(filename);
}

void setup() {
  // Serial.begin(9600);
  delay(1000);
  // while (!Serial);
  setupSDCard();
  setupRTC();
  setupTerminationSwitches();
  setupLoggingControlSwitches();
  last_rtc_secs = Teensy3Clock.get();
  us_since_last_rtc = 0;
  setupLEDs();
  setupWifi();
  setupCAN();
  setupIMU();
  setupGPS();
  digitalWrite(ledPinB, HIGH);
  delay(1000);
  canTime = millis();
}


//=================================================================Logging==============================================================//

bool collectGPS(uint8_t* buf, size_t& len) {
  if (!myGNSS.getPVT()) {
    len = 0;
    return false; // No new GPS fix
  }

  int32_t latitude = myGNSS.getLatitude();         // degrees * 10^7
  int32_t longitude = myGNSS.getLongitude();       // degrees * 10^7
  int32_t altitude = myGNSS.getAltitudeMSL();      // mm
  uint32_t timestamp = Teensy3Clock.get();

//  #ifdef DEBUG

  //  Serial.print("[GPS] Lat: "); Serial.print(latitude);
  //  Serial.print(", Lon: "); Serial.print(longitude);
  //  Serial.print(", Alt: "); Serial.println(altitude);

//  #endif
  // Pack the data into the buffer as binary
  memcpy(buf, &timestamp, sizeof(timestamp));       // [0–7]
  memcpy(buf + 4, &latitude, sizeof(latitude));     // [8–11]
  memcpy(buf + 8, &longitude, sizeof(longitude));  // [12–15]
  memcpy(buf + 12, &altitude, sizeof(altitude));

  len = 16;
  return true;
}

bool collectIMU(uint8_t* buf, size_t& len) {
  uint8_t accelData[6], gyroData[6];

  readBytes(OUTX_L_A, accelData, 6);
  readBytes(OUTX_L_G, gyroData, 6);

  int16_t ax = toInt16(accelData[0], accelData[1]);
  int16_t ay = toInt16(accelData[2], accelData[3]);
  int16_t az = toInt16(accelData[4], accelData[5]);

  int16_t gx = toInt16(gyroData[0], gyroData[1]);
  int16_t gy = toInt16(gyroData[2], gyroData[3]);
  int16_t gz = toInt16(gyroData[4], gyroData[5]);
  uint32_t timestamp = Teensy3Clock.get();
//  #ifdef DEBUG

  //  Serial.print("[IMU] Accel: ");
  //  Serial.print(ax); Serial.print(", ");
  //  Serial.print(ay); Serial.print(", ");
  //  Serial.print(az); Serial.print(" | ");
  //
  //  Serial.print("Gyro: ");
  //  Serial.print(gx); Serial.print(", ");
  //  Serial.print(gy); Serial.print(", ");
  //  Serial.println(gz);

//    #endif

  // Pack the raw IMU data into the buffer
  

  memcpy(buf, &timestamp, sizeof(timestamp));       // [0–3]
  memcpy(buf + 4, &ax, sizeof(ax));                 // [4–5]
  memcpy(buf + 6, &ay, sizeof(ay));                 // [6–7]
  memcpy(buf + 8, &az, sizeof(az));                 // [8–9]
  memcpy(buf + 10, &gx, sizeof(gx));                // [10–11]
  memcpy(buf + 12, &gy, sizeof(gy));                // [12–13]
  memcpy(buf + 14, &gz, sizeof(gz));                // [14–15]

  len = 16;
  return true;
}


void handleClientRequest() {
  WiFiClient client = server.available();
  if (!client) return;

  // Serial.println("Client connected");

  while (client.connected() && !client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  // Serial.print("Request: ");
  // Serial.println(request);
  client.read(); // consume '\n'

  String filename = "/";
  int idx = request.indexOf(' ');
  if (idx != -1) {
    int idx2 = request.indexOf(' ', idx + 1);
    if (idx2 != -1) {
      filename = request.substring(idx + 1, idx2);
    }
  }

  // Serve file list if requesting root
  if (filename == "/") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<html><head><title>CAN Logger Files</title></head><body>");
    client.println("<h1>Logged Files:</h1><ul>");

    File root = SD.open("/");
    if (root && root.isDirectory()) {
      File entry = root.openNextFile();
      while (entry) {
        if (!entry.isDirectory()) {
          String name = entry.name();
          client.print("<li><a href=\"");
          client.print(name);
          client.print("\">");
          client.print(name);
          client.println("</a></li>");
        }
        entry.close();
        entry = root.openNextFile();
      }
    } else {
      client.println("<li>No files found.</li>");
    }

    client.println("</ul></body></html>");
    client.stop();
    return;
  }

  // Serve file download
  if (SD.exists(filename.c_str())) {
    File downloadFile = SD.open(filename.c_str());
    if (downloadFile) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/octet-stream");
      client.println("Content-Disposition: attachment; filename=\"" + filename + "\"");
      client.println("Connection: close");
      client.println();

      uint8_t buf[512];
      while (downloadFile.available()) {
        size_t bytesRead = downloadFile.read(buf, sizeof(buf));
        client.write(buf, bytesRead);
      }

      downloadFile.close();
      // Serial.println("File sent successfully.");
    } else {
      client.println("HTTP/1.1 500 Internal Server Error");
      client.println();
    }
  } else {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("404 File Not Found");
  }

  delay(10);
  client.stop();
  // Serial.println("Client disconnected");
}


void hexDump(const uint8_t* data, size_t len) {
  const size_t bytesPerLine = 16;
  for (size_t i = 0; i < len; i += bytesPerLine) {
    // Print offset
    Serial.printf("%04X  ", (unsigned int)i);

    // Print hex bytes
    for (size_t j = 0; j < bytesPerLine; ++j) {
      if (i + j < len)
        Serial.printf("%02X ", data[i + j]);
      else
        Serial.print("   ");
    }

    Serial.print(" ");

    // Print ASCII
    for (size_t j = 0; j < bytesPerLine; ++j) {
      if (i + j < len) {
        char c = data[i + j];
        if (c >= 32 && c <= 126)
          Serial.print(c);
        else
          Serial.print(".");
      }
    }
    Serial.println();
  }
}

// Helper: check if any error is present
bool hasAnyError(const CAN_error_t &e) {
  return e.BIT1_ERR || e.BIT0_ERR || e.ACK_ERR || e.CRC_ERR ||
         e.FRM_ERR || e.STF_ERR || e.TX_WRN || e.RX_WRN;
}

// Helper: pack individual error bits into a byte
uint8_t computeErrorFlags(const CAN_error_t &e) {
  return (e.BIT1_ERR << 7) |
         (e.BIT0_ERR << 6) |
         (e.ACK_ERR  << 5) |
         (e.CRC_ERR  << 4) |
         (e.FRM_ERR  << 3) |
         (e.STF_ERR  << 2) |
         (e.TX_WRN   << 1) |
         (e.RX_WRN   << 0);
}


void loop() {
  static bool fileInitialized = false;
  static unsigned long lastDebounceTime1 = 0;
  static unsigned long lastDebounceTime2 = 0;
  const unsigned long debounceDelay = 50;

  // Read current button states
  bool currentState1 = digitalRead(switchPin1);
  unsigned long currentTime = millis();

  // Button 1: Start Logging (detect press)
  if (lastState1 == HIGH && currentState1 == LOW && (currentTime - lastDebounceTime1 > debounceDelay)) {
    if (!loggingActive) {
      loggingActive = true;
      fileInitialized = false;  // Force file (re)init
      digitalWrite(ledPinY, HIGH);
      // #ifdef DEBUG
      //   Serial.println("Logging started");
      // #endif
    }
    lastDebounceTime1 = currentTime;
  }
  lastState1 = currentState1;

  // Button 2: Stop Logging (detect press)
  bool currentState2 = digitalRead(switchPin2);
  if (lastState2 == HIGH && currentState2 == LOW && (currentTime - lastDebounceTime2 > debounceDelay)) {
    if (loggingActive) {
      loggingActive = false;
      digitalWrite(ledPinY, LOW);
      // #ifdef DEBUG
      //   Serial.println("Logging stopped");
      // #endif
    }
    lastDebounceTime2 = currentTime;
  }
  lastState2 = currentState2;

  handleClientRequest();

  if (!loggingActive) return;

  if (!fileInitialized) {
    currentFilename = generateTimestampedFilename();
    logFile = SD.open(currentFilename.c_str(), FILE_WRITE);
    if (!logFile) {
      Serial.println("Failed to open log file!");
      while (1); // halt if file can't be opened
    }
    uint32_t startTimestamp = Teensy3Clock.get();
    currentBuffer->begin(startTimestamp);
    fileInitialized = true;
  }

  if (currentBuffer->offset >= BUFFER_FLUSH_THRESHOLD || flushRequested) {
    uint32_t timestamp = Teensy3Clock.get();
    if (can1LogCount > 0) {
      currentBuffer->addSection(SECTION_CAN1, can1Log, can1LogCount * sizeof(CANMessageLog));
//      #ifdef DEBUG
  //      Serial.print("CAN1 messages: "); Serial.println(can1LogCount);
//      #endif
      can1LogCount = 0;
    }
    if (can2LogCount > 0) {
      currentBuffer->addSection(SECTION_CAN2, can2Log, can2LogCount * sizeof(CANMessageLog));
//      #ifdef DEBUG
  //      Serial.print("CAN2 messages: "); Serial.println(can2LogCount);
//      #endif
      can2LogCount = 0;

    }
    if (can3LogCount > 0) {
      currentBuffer->addSection(SECTION_CAN3, can3Log, can3LogCount * sizeof(CANMessageLog));
//      #ifdef DEBUG
  //      Serial.print("CAN3 messages: "); Serial.println(can3LogCount);
//      #endif
      can3LogCount = 0;
    }

    // GPS
    uint8_t gpsBuf[12];
    size_t gpsLen = 0;
    if (collectGPS(gpsBuf, gpsLen)) {
      currentBuffer->addSection(SECTION_GPS, gpsBuf, gpsLen);
    }

    // IMU
    uint8_t imuBuf[12];
    size_t imuLen = 0;
    if (collectIMU(imuBuf, imuLen)) {
      currentBuffer->addSection(SECTION_IMU, imuBuf, imuLen);
    }
//    #ifdef DEBUG
  //    Serial.println("Buffer before finalize:");
  //    hexDump(currentBuffer->data, currentBuffer->offset);
//    #endif


    // CAN Errors
    CANErrorEntry errorEntries[3] {};  // One for each CAN interface
    CAN_error_t err1;
    if (can1.error(err1, false)) {
      errorEntries[0] = {
        .timestamp = Teensy3Clock.get(),
        .busID = 1,
        .errorFlags = computeErrorFlags(err1),
        .txErr = err1.TX_ERR_COUNTER,
        .rxErr = err1.RX_ERR_COUNTER
      };
    } else {
      errorEntries[0] = {.timestamp = Teensy3Clock.get(), .busID = 1, .errorFlags = 0, .txErr = 0, .rxErr = 0 };
    }
    
    CAN_error_t err2;
    if (can2.error(err2, false)) {
      errorEntries[1] = {
        .timestamp = Teensy3Clock.get(),
        .busID = 2,
        .errorFlags = computeErrorFlags(err2),
        .txErr = err2.TX_ERR_COUNTER,
        .rxErr = err2.RX_ERR_COUNTER
      };
    } else {
      errorEntries[1] = {.timestamp = Teensy3Clock.get(), .busID = 2, .errorFlags = 0, .txErr = 0, .rxErr = 0 };
    }
    
    CAN_error_t err3;
    if (can3.error(err3, false)) {
      errorEntries[2] = {
        .timestamp = Teensy3Clock.get(),
        .busID = 3,
        .errorFlags = computeErrorFlags(err3),
        .txErr = err3.TX_ERR_COUNTER,
        .rxErr = err3.RX_ERR_COUNTER
      };
    } else {
      errorEntries[2] = {.timestamp = Teensy3Clock.get(), .busID = 3, .errorFlags = 0, .txErr = 0, .rxErr = 0 };
    }

    currentBuffer->addSection(SECTION_CAN_ERROR, errorEntries, sizeof(errorEntries));


    currentBuffer->finalize();
//   #ifdef DEBUG
  //    Serial.println("=== BUFFER DUMP START ===");
  //    hexDump(currentBuffer->data, currentBuffer->offset);
  //    Serial.println("=== BUFFER DUMP END ===");
  //    while (1);  // halt here
//    #endif

    
    if (!logFile || logFile.size() >= MAX_FILE_SIZE) {
      if (logFile) logFile.close();
      currentFilename = generateTimestampedFilename();
      logFile = SD.open(currentFilename.c_str(), FILE_WRITE);
    }
    
    logFile.write(currentBuffer->data, currentBuffer->offset);
    logFile.flush();
    
    BufferBuilder* temp = currentBuffer;
    currentBuffer = flushBuffer;
    flushBuffer = temp;

    flushRequested = false;
    
    currentBuffer->begin(timestamp);
  }
}
