#include "DataLogger.h"

DataLogger dataLogger;

void DataLogger::begin() {
    fileIndex = 0;
    currentFileSize = 0;
    queueHead = queueTail = 0;

    if (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("SD card init failed!");
        return;
    }

    openNextFile();
}

void DataLogger::loop() {
    writeNextBuffer();
}

void DataLogger::enqueueBuffer(const uint8_t* data, size_t len) {
    size_t nextTail = (queueTail + 1) % QUEUE_SIZE;

    if (nextTail == queueHead) {
        Serial.println("DataLogger queue full — dropping data!");
        return;
    }

    writeQueue[queueTail] = { data, len };
    queueTail = nextTail;
}

void DataLogger::writeNextBuffer() {
    if (queueHead == queueTail) return; 

    BufferChunk& chunk = writeQueue[queueHead];
    rotateFileIfNeeded(chunk.length);

    if (currentFile) {
        size_t written = currentFile.write(chunk.data, chunk.length);
        currentFile.flush(); 
        currentFileSize += written;
    }

    queueHead = (queueHead + 1) % QUEUE_SIZE;
}

void DataLogger::rotateFileIfNeeded(size_t incomingSize) {
    if (currentFileSize + incomingSize > MAX_FILE_SIZE) {
        if (currentFile) currentFile.close();
        fileIndex = (fileIndex + 1) % MAX_FILENAME_INDEX;
        openNextFile();
    }
}

void DataLogger::openNextFile() {
    char filename[20];
    for (int attempts = 0; attempts < MAX_FILENAME_INDEX; ++attempts) {
        snprintf(filename, sizeof(filename), "/log_%03d.bin", fileIndex);
        if (!SD.exists(filename)) break;
        fileIndex = (fileIndex + 1) % MAX_FILENAME_INDEX;
    }

    currentFile = SD.open(filename, FILE_WRITE);
    currentFileSize = 0;

    if (currentFile) {
        Serial.print("Opened log file: ");
        Serial.println(filename);
    } else {
        Serial.println("Failed to open new log file.");
    }
}
