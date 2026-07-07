// CAN1–CAN3 (Teensy Internal)
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;

CAN_message_t msg1;
CAN_message_t msg2;
CAN_message_t msg3;

const uint32_t CAN_BAUD_RATES[] = {125000, 250000, 500000, 1000000};

struct CANMessage {
  uint64_t timestamp;
  uint32_t id;
  uint8_t len;
  uint8_t buf[8];
  uint8_t extended;
  uint8_t overrun;
  uint8_t mailbox;
};

static CANMessage entry;

static void processCANMessage(uint8_t bus, const CAN_message_t &msg);

uint32_t can1_autobaud() 
{
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
    processCANMessage(1, msg1);
}

void can2Sniff(const CAN_message_t &msg2){
    processCANMessage(2, msg2);
}

void can3Sniff(const CAN_message_t &msg3){
    processCANMessage(3, msg3);
}

bool initCAN()
{
    // CAN1 Setup
    uint32_t baudRate = can1_autobaud();
    if (baudRate !=0){
        can1.begin(); 
        can1.setBaudRate(baudRate);
        can1.setMaxMB(16);
        can1.enableFIFO();
        can1.enableFIFOInterrupt();
        can1.onReceive(can1Sniff);
        Serial.println("Can1 Setup");
    }
    // CAN2 Setup
    baudRate = can2_autobaud();
    if (baudRate !=0){
        can2.begin();
        can2.setBaudRate(baudRate);
        can2.setMaxMB(16);
        can2.enableFIFO();
        can2.enableFIFOInterrupt();
        can2.onReceive(can2Sniff);
        Serial.println("Can2 Setup");
    }
  //CAN3 Setup
    baudRate = can3_autobaud();
    if (baudRate !=0){
        can3.begin();
        can3.setBaudRate(baudRate);
        can3.setMaxMB(16);
        can3.enableFIFO();
        can3.enableFIFOInterrupt();
        can3.onReceive(can3Sniff);
        Serial.println("Can3 Setup");
    }
    return true;
}

static void processCANMessage(uint8_t bus, const CAN_message_t &msg)
{
    // Placeholder
    //
    // Later:
    //   Timestamp frame
    //   Queue frame
    //   Forward to eld.cpp
    entry.timestamp = getMicroTimestamp();
    entry.id = msg.id;
    entry.len = msg.len;
    memcpy(entry.buf, msg.buf, msg.len);
    entry.extended = msg.flags.extended;
    entry.overrun = msg.flags.overrun;
    entry.mailbox = msg.mb;
}