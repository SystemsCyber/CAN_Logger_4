#include <SD.h>
#include <SPI.h>

const int chipSelect = BUILTIN_SDCARD;  // Use the built-in SD card slot
File logFile;

const unsigned long MAX_FILE_SIZE = 100UL * 1024UL * 1024UL; // 100MB max file size
unsigned long fileIndex = 0;  // To keep track of file numbers

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for the Serial Monitor to connect

  // Initialize the SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Start logging by opening a new file
  openNewLogFile();
}

void loop() {
  // Simulate logging data to the SD card
  logData("This is a sample log entry.\n");

  // Check if the current file size exceeds the limit
  if (logFile.size() >= MAX_FILE_SIZE) {
    Serial.println("Max file size reached, creating a new log file.");
    logFile.close();  // Close the current file
    openNewLogFile(); // Open a new log file
  }

  delay(500);  // Adjust the delay as needed for testing
}

void openNewLogFile() {
  String fileName = "log_" + String(fileIndex++) + ".txt";  // Create a unique file name
  logFile = SD.open(fileName.c_str(), FILE_WRITE);  // Open the file for writing

  if (logFile) {
    Serial.print("Opened log file: ");
    Serial.println(fileName);
  } else {
    Serial.println("Error opening log file!");
  }
}

void logData(const char* data) {
  if (logFile) {
    logFile.print(data);  // Write data to the log file
    logFile.flush();      // Ensure data is written to the SD card
  } else {
    Serial.println("Log file is not open!");
  }
}
