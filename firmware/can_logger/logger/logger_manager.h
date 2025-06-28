#ifndef LOGGER_MANAGER_H
#define LOGGER_MANAGER_H

#include <Arduino.h>

enum LoggerState {
    LOGGER_IDLE,
    LOGGER_LOGGING,
    LOGGER_STOPPED,
    LOGGER_TRANSFER
};

class LoggerManager {
public:
    void begin();
    void loop();

    void startLogging();
    void stopLogging();
    void enterTransferMode();
    void exitTransferMode();

    bool isLogging() const;
    bool isTransferMode() const;

    void addLogEntry(const uint8_t* data, size_t len);

private:
    void swapBuffersIfNeeded();
    void flushInactiveBuffer();

    static const size_t BUFFER_SIZE = 8192;

    uint8_t bufferA[BUFFER_SIZE];
    uint8_t bufferB[BUFFER_SIZE];
    volatile bool usingBufferA = true;

    volatile size_t currentOffset = 0;
    LoggerState state = LOGGER_IDLE;

    elapsedMillis flushTimer;
    const uint32_t FLUSH_INTERVAL_MS = 100;
};

extern LoggerManager loggerManager;

#endif // LOGGER_MANAGER_H
