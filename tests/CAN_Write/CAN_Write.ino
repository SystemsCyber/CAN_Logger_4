#include <FlexCAN_T4.h>

// Initialize CAN buses
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can2;

CAN_message_t txMsg;  // Transmit message
CAN_error_t canError; // CAN error structure

long userBaudRate = 500000; // Default baud rate

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial connection
  }

  // Ask user to input the desired baud rate
  Serial.println("Enter CAN baud rate (e.g., 500000):");

  // Wait for user input
  while (Serial.available() == 0) {
    // Waiting for input
  }

  userBaudRate = Serial.parseInt(); // Parse user input to set baud rate
  Serial.print("Setting CAN baud rate to: ");
  Serial.println(userBaudRate);

  // Initialize CAN buses with user-defined baud rate
  Can0.begin();
  Can0.setBaudRate(userBaudRate);
  Can1.begin();
  Can1.setBaudRate(userBaudRate);
  Can2.begin();
  Can2.setBaudRate(userBaudRate);

  Serial.println("CAN Write Test Started");

  // Prepare a message to send
  txMsg.id = 0x123;  // CAN message ID
  txMsg.len = 8;     // Message length
  txMsg.buf[0] = 0xAA;  // Data
  txMsg.buf[1] = 0xBB;
  txMsg.buf[2] = 0xCC;
  txMsg.buf[3] = 0xDD;
  txMsg.buf[4] = 0xEE;
  txMsg.buf[5] = 0xFF;
  txMsg.buf[6] = 0x11;
  txMsg.buf[7] = 0x22;
}

void loop() {
  // Attempt to send message on CAN0
  if (Can0.write(txMsg)) {
    Serial.println("Message sent successfully on CAN0");
  } else {
    printCANWriteErrorCan0(Can0, "CAN0");
  }

  // Attempt to send message on CAN1
  if (Can1.write(txMsg)) {
    Serial.println("Message sent successfully on CAN1");
  } else {
    printCANWriteErrorCan1(Can1, "CAN1");
  }

  // Attempt to send message on CAN2
  if (Can2.write(txMsg)) {
    Serial.println("Message sent successfully on CAN2");
  } else {
    printCANWriteErrorCan2(Can2, "CAN2");
  }

  delay(1000);  // Send every second
}

// Overloaded function for CAN0
void printCANWriteErrorCan0(FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> &can, String canBusName) {
  // Retrieve and print error details
  can.error(canError, true);  // The true argument will print detailed errors automatically

  Serial.print(canBusName);
  Serial.println(" Write Failed! Error Details Printed.");
}

// Overloaded function for CAN1
void printCANWriteErrorCan1(FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> &can, String canBusName) {
  // Retrieve and print error details
  can.error(canError, true);  // The true argument will print detailed errors automatically

  Serial.print(canBusName);
  Serial.println(" Write Failed! Error Details Printed.");
}

// Overloaded function for CAN2
void printCANWriteErrorCan2(FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> &can, String canBusName) {
  // Retrieve and print error details
  can.error(canError, true);  // The true argument will print detailed errors automatically

  Serial.print(canBusName);
  Serial.println(" Write Failed! Error Details Printed.");
}
