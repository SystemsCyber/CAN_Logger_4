#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include <SD.h>

class DataLogger {
public:
    void begin();
    void loop(); 

    void enqueueBuffer(const uint8_t* data, size_t len);

private:
    static const size_t MAX_FILE_SIZE = 512UL * 1024UL * 1024UL;
    static const size_t MAX_FILENAME_INDEX = 999;

    File currentFile;
    uint32_t currentFileSize = 0;
    uint16_t fileIndex = 0;

    struct BufferChunk {
        const uint8_t* data;
        size_t length;
    };

    static const size_t QUEUE_SIZE = 4;
    BufferChunk writeQueue[QUEUE_SIZE];
    volatile size_t queueHead = 0;
    volatile size_t queueTail = 0;

    void rotateFileIfNeeded(size_t incomingSize);
    void openNextFile();
    void writeNextBuffer();
};

extern DataLogger dataLogger;

#endif
