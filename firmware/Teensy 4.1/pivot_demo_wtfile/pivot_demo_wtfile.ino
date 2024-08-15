#include <FlexCAN_T4.h>
#include <SD.h>
#include <SPI.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
CAN_message_t msg;

const int chipSelect = BUILTIN_SDCARD;  // Use BUILTIN_SDCARD for the chip select
File dataFile;
uint32_t fileSize = 0;
uint32_t maxFileSize = 1 * 1024 * 1024; // 1MB
uint8_t fileIndex = 0;
char currentFileName[15];

void setup(void) {
  can1.begin();
  can1.setBaudRate(250000);
  can2.begin();
  can2.setBaudRate(250000);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }

  createNewFile();
}

void loop() {
  if (can1.read(msg)) {
    writeToFile(msg, 1);  // Write CAN1 data with bus identifier 1
  }
  else if (can2.read(msg)) {
    writeToFile(msg, 2);  // Write CAN2 data with bus identifier 2
  }
}

void createNewFile() {
  do {
    snprintf(currentFileName, sizeof(currentFileName), "log_%03d.bin", fileIndex++);
  } while (SD.exists(currentFileName));

  dataFile = SD.open(currentFileName, FILE_WRITE);

  if (!dataFile) {
    Serial.println("Error opening file");
    while (1);
  }

  fileSize = 0;
}

void writeToFile(CAN_message_t &msg, uint8_t bus) {
  if (fileSize >= maxFileSize) {
    dataFile.close();
    createNewFile();
  }

  dataFile.write(bus);  // Write the bus identifier (1 for CAN1, 2 for CAN2)
  dataFile.write((uint8_t*)&msg.id, sizeof(msg.id));
  dataFile.write((uint8_t*)&msg.flags.extended, sizeof(msg.flags.extended));
  dataFile.write((uint8_t*)&msg.len, sizeof(msg.len));
  dataFile.write(msg.buf, sizeof(msg.buf));
  fileSize += sizeof(bus) + sizeof(msg.id) + sizeof(msg.flags.extended) + sizeof(msg.len) + sizeof(msg.buf);

  dataFile.flush();  // Ensures data is written to the SD card immediately
}
