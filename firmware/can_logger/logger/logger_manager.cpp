#include "logger_manager.h"
#include "data_logger.h"

LoggerManager loggerManager;

void LoggerManager::begin() {
    flushTimer = 0;
    currentOffset = 0;
    usingBufferA = true;
    state = LOGGER_IDLE;
}

void LoggerManager::loop() {
    if (state != LOGGER_LOGGING) return;

    if (currentOffset >= BUFFER_SIZE || flushTimer >= FLUSH_INTERVAL_MS) {
        swapBuffersIfNeeded();
    }
}

void LoggerManager::startLogging() {
    if (state == LOGGER_IDLE || state == LOGGER_STOPPED) {
        currentOffset = 0;
        flushTimer = 0;
        state = LOGGER_LOGGING;
    }
}

void LoggerManager::stopLogging() {
    if (state == LOGGER_LOGGING) {
        swapBuffersIfNeeded(); 
        state = LOGGER_STOPPED;
    }
}

void LoggerManager::enterTransferMode() {
    stopLogging();
    state = LOGGER_TRANSFER;
}

void LoggerManager::exitTransferMode() {
    state = LOGGER_IDLE;
}

bool LoggerManager::isLogging() const {
    return state == LOGGER_LOGGING;
}

bool LoggerManager::isTransferMode() const {
    return state == LOGGER_TRANSFER;
}

void LoggerManager::addLogEntry(const uint8_t* data, size_t len) {
    if (!isLogging() || len > BUFFER_SIZE) return;

    uint8_t* buffer = usingBufferA ? bufferA : bufferB;

    if (currentOffset + len > BUFFER_SIZE) {
        swapBuffersIfNeeded();
        buffer = usingBufferA ? bufferA : bufferB;
    }

    memcpy(buffer + currentOffset, data, len);
    currentOffset += len;
}

void LoggerManager::swapBuffersIfNeeded() {
    uint8_t* bufferToFlush = usingBufferA ? bufferA : bufferB;
    size_t flushLength = currentOffset;

    usingBufferA = !usingBufferA;
    currentOffset = 0;
    flushTimer = 0;

    dataLogger.enqueueBuffer(bufferToFlush, flushLength);
}
