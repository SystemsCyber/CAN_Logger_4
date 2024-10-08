#include <SPI.h>

const int slaveSelectPin = 10;  // CS pin for the ESP8266 (slave)

void setup() {
  Serial.begin(115200);
  
  // Set the CS pin as output
  pinMode(slaveSelectPin, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);  // Make sure the slave is not selected

  // Initialize SPI as master
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);           // Send most significant bit first
  SPI.setDataMode(SPI_MODE0);          // Set SPI mode
  SPI.setClockDivider(SPI_CLOCK_DIV8); // Set SPI clock speed
  Serial.println("Teensy SPI Master initialized.");
}

void loop() {
  // Simulate sending some data over SPI to the ESP8266
  digitalWrite(slaveSelectPin, LOW);  // Select the slave (ESP8266)
  
  byte dataToSend = 0xA5;  // Example data (could be CAN log data in real implementation)
  byte response = SPI.transfer(dataToSend);  // Send the data

  digitalWrite(slaveSelectPin, HIGH);  // Deselect the slave

  // Print sent data and response
  Serial.print("Sent data: ");
  Serial.println(dataToSend, HEX);
  Serial.print("Received response: ");
  Serial.println(response, HEX);
  
  delay(1000);  // Wait before sending the next data
}
