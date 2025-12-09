/**
 * myIoTGrid.Sensor - SD Card Logger
 * Sprint 8: Remote Debug System
 *
 * Handles logging to SD card with circular file rotation
 * and JSON-Lines format for easy parsing.
 */

#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>
#include "debug_manager.h"

#ifdef PLATFORM_ESP32
#include <SD.h>
#include <SPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif

/**
 * SD Logger Configuration
 */
struct SDLoggerConfig {
    size_t maxFileSize = 1024 * 1024;      // 1 MB per file
    int maxFiles = 10;                      // 10 files = 10 MB max
    int maxQueueSize = 100;                 // Max entries in queue
    unsigned long flushIntervalMs = 5000;   // Flush every 5 seconds
    bool enabled = true;
};

/**
 * SD Logger Statistics
 */
struct SDLoggerStats {
    uint32_t entriesWritten = 0;
    uint32_t entriesDropped = 0;
    uint32_t filesRotated = 0;
    uint64_t bytesWritten = 0;
    unsigned long lastFlushTime = 0;
};

/**
 * SDLogger - Circular file logging to SD card
 */
class SDLogger {
public:
    static SDLogger& getInstance();

    // Prevent copying
    SDLogger(const SDLogger&) = delete;
    SDLogger& operator=(const SDLogger&) = delete;

    /**
     * Initialize SD logger
     * @param csPin SD card chip select pin
     * @return true if SD card initialized successfully
     */
    bool begin(int csPin = 5);

    /**
     * Check if SD card is available
     */
    bool isAvailable() const { return _sdAvailable; }

    /**
     * Check if logging is enabled
     */
    bool isEnabled() const { return _config.enabled && _sdAvailable; }

    /**
     * Enable/disable SD logging
     */
    void setEnabled(bool enabled) { _config.enabled = enabled; }

    /**
     * Configure logger
     */
    void configure(const SDLoggerConfig& config);

    /**
     * Log a debug entry (called from DebugManager)
     */
    void log(const LogEntry& entry);

    /**
     * Process queued logs (call from main loop)
     */
    void loop();

    /**
     * Flush all pending logs to SD card
     */
    void flush();

    /**
     * Get current log file path
     */
    String getCurrentFilePath() const;

    /**
     * Get all log files
     */
    std::vector<String> getLogFiles() const;

    /**
     * Read log file contents
     * @param filename File to read
     * @param maxLines Max lines to read (0 = all)
     * @return Log content
     */
    String readLogFile(const String& filename, int maxLines = 0) const;

    /**
     * Read recent logs from all files
     * @param maxLines Max lines to return
     * @return Combined log content (newest first)
     */
    String readRecentLogs(int maxLines = 100) const;

    /**
     * Clear all log files
     */
    void clearLogs();

    /**
     * Get statistics
     */
    SDLoggerStats getStats() const { return _stats; }

    /**
     * Get SD card info
     */
    uint64_t getCardSize() const;
    uint64_t getUsedSpace() const;
    uint64_t getFreeSpace() const;

private:
    SDLogger();
    ~SDLogger();

    void rotateFileIfNeeded();
    void openCurrentFile();
    void closeCurrentFile();
    void writeEntry(const LogEntry& entry);
    String formatEntry(const LogEntry& entry) const;
    int getNextFileNumber() const;
    String getFilePath(int fileNumber) const;

    bool _sdAvailable;
    SDLoggerConfig _config;
    SDLoggerStats _stats;

    int _currentFileNumber;
    size_t _currentFileSize;
    File _currentFile;

#ifdef PLATFORM_ESP32
    QueueHandle_t _logQueue;
    SemaphoreHandle_t _fileMutex;
#endif

    unsigned long _lastFlushTime;

    // Log directory
    static constexpr const char* LOG_DIR = "/logs";
    static constexpr const char* LOG_PREFIX = "debug_";
    static constexpr const char* LOG_EXT = ".log";
};

#endif // SD_LOGGER_H
