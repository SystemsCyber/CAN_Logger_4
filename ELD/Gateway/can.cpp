// CAN1–CAN3 (Teensy Internal)
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;

CAN_message_t msg1;
CAN_message_t msg2;
CAN_message_t msg3;

const uint32_t CAN_BAUD_RATES[] = {125000, 250000, 500000, 1000000};


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

bool decodeJ1939ID(const CAN_message_t &msg,
                   CANMessageLog &entry)
{
    if (!msg.flags.extended)
        return false;

    uint8_t priority = (msg.id >> 26) & 0x07;
    uint8_t pf       = (msg.id >> 16) & 0xFF;
    uint8_t ps       = (msg.id >> 8)  & 0xFF;
    uint8_t sa       =  msg.id        & 0xFF;

    entry.priority = priority;
    entry.source   = sa;

    if (pf < 240)
    {
        // PDU1 (Destination Specific)
        entry.destination = ps;
        entry.pgn = ((uint32_t)pf << 8);
    }
    else
    {
        // PDU2 (Broadcast)
        entry.destination = 0xFF;
        entry.pgn = ((uint32_t)pf << 8) | ps;
    }

    return true;
}

static void processCANMessage(uint8_t bus, const CAN_message_t &msg)
{
    entry.timestamp = getMicroTimestamp();
    entry.id        = msg.id;
    entry.extended  = msg.flags.extended;
    entry.overrun   = msg.flags.overrun;
    entry.mailbox   = msg.mb;
    // Ignore anything that isn't a valid J1939 frame
    if (!decodeJ1939ID(msg, entry))
        return;
    entry.len = msg.len;
    memcpy(entry.buf, msg.buf, msg.len);
    eldProcessCANMessage(entry);
}