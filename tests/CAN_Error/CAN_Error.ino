#include <FlexCAN_T4.h>

// Initialize CAN buses
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

CAN_message_t txMsg;    // CAN transmit message
CAN_message_t rxMsg;    // CAN receive message
CAN_error_t canError;   // CAN error structure

void setup() {
  Serial.begin(115200);
  
  // Initialize CAN buses at 500 kbps
  Can0.begin();
  Can0.setBaudRate(500000);
  Can1.begin();
  Can1.setBaudRate(500000);

  Serial.println("CAN Bus Error Detection Test Started...");

  // Prepare a message to send
  txMsg.id = 0x123;
  txMsg.len = 8;
  txMsg.buf[0] = 0xAA;
  txMsg.buf[1] = 0xBB;
  txMsg.buf[2] = 0xCC;
  txMsg.buf[3] = 0xDD;
  txMsg.buf[4] = 0xEE;
  txMsg.buf[5] = 0xFF;
  txMsg.buf[6] = 0x11;
  txMsg.buf[7] = 0x22;
}

void loop() {
  // Attempt to send a message on CAN0
  if (Can0.write(txMsg)) {
    Serial.println("Message sent on CAN0");
  } else {
    Serial.println("Failed to send message on CAN0");
    handleCANError(Can0, "CAN0");
  }

  // Attempt to send a message on CAN1
  if (Can1.write(txMsg)) {
    Serial.println("Message sent on CAN1");
  } else {
    Serial.println("Failed to send message on CAN1");
    handleCANError(Can1, "CAN1");
  }

  // Read a message from CAN0
  if (Can0.read(rxMsg)) {
    Serial.print("Received message on CAN0: ID = ");
    Serial.println(rxMsg.id, HEX);
  } else {
    handleCANError(Can0, "CAN0");
  }

  // Read a message from CAN1
  if (Can1.read(rxMsg)) {
    Serial.print("Received message on CAN1: ID = ");
    Serial.println(rxMsg.id, HEX);
  } else {
    handleCANError(Can1, "CAN1");
  }

  delay(1000);  // Adjust the delay as needed
}

// Template function to handle CAN errors and print details
template <typename CAN_T>
void handleCANError(CAN_T &can, String canBusName) {
  can.error(canError, true);  // Retrieve error details

  Serial.print(canBusName);
  Serial.println(" Error Detected:");

  if (canError.BIT1_ERR) Serial.println("BIT1 Error");
  if (canError.BIT0_ERR) Serial.println("BIT0 Error");
  if (canError.ACK_ERR) Serial.println("ACK Error");
  if (canError.CRC_ERR) Serial.println("CRC Error");
  if (canError.FRM_ERR) Serial.println("Form Error");
  if (canError.STF_ERR) Serial.println("Stuff Error");

  // Check for warning and bus-off conditions
  if (canError.TX_WRN) Serial.printf("TX Warning: %d\n", canError.TX_ERR_COUNTER);
  if (canError.RX_WRN) Serial.printf("RX Warning: %d\n", canError.RX_ERR_COUNTER);
  
  // Check if the CAN bus is in bus-off mode
  if (strcmp((char*)canError.FLT_CONF, "Bus off") == 0) {
    Serial.println("Bus is in bus-off state!");
  } else if (strcmp((char*)canError.FLT_CONF, "Error Passive") == 0) {
    Serial.println("Error Passive state.");
  } else {
    Serial.println("Error Active state.");
  }
}
