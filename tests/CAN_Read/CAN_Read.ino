#include <FlexCAN_T4.h>

// Initialize CAN buses
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can2;

CAN_message_t rxMsg;  // Receive message
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

  Serial.println("CAN Read Test Started");
}

void loop() {
  // Attempt to read a message on CAN0
  if (Can0.read(rxMsg)) {
    printCANMessage(rxMsg, "CAN0");
  } else {
    printCANReadError(Can0, "CAN0");
  }

  // Attempt to read a message on CAN1
  if (Can1.read(rxMsg)) {
    printCANMessage(rxMsg, "CAN1");
  } else {
    printCANReadError(Can1, "CAN1");
  }

  // Attempt to read a message on CAN2
  if (Can2.read(rxMsg)) {
    printCANMessage(rxMsg, "CAN2");
  } else {
    printCANReadError(Can2, "CAN2");
  }

  delay(1000);  // Wait 1 second between checks
}

// Function to print received CAN message (no template required)
void printCANMessage(CAN_message_t &msg, String canBusName) {
  Serial.print(canBusName);
  Serial.print(" Received message: ID = ");
  Serial.print(msg.id, HEX);
  Serial.print(" Data = ");
  for (int i = 0; i < msg.len; i++) {
    Serial.print(msg.buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

// Template function to handle CAN errors for any CAN bus type
template <typename CAN_T>
void printCANReadError(CAN_T &can, String canBusName) {
  // Retrieve and print error details
  can.error(canError, true);  // The true argument will print detailed errors automatically

  Serial.print(canBusName);
  Serial.println(" Read Failed! Error Details Printed.");
}
