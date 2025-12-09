/**
 * myIoTGrid.Sensor - Debug Manager
 * Sprint 8: Remote Debug System
 *
 * Provides configurable debug levels and log categories
 * for remote troubleshooting without impacting production performance.
 */

#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include <Arduino.h>
#include <functional>
#include <vector>

/**
 * Debug Level Enum
 * PRODUCTION = 0: Minimal logging, errors only
 * NORMAL = 1: Standard logging (default)
 * DEBUG = 2: Verbose logging for troubleshooting
 */
enum class DebugLevel : uint8_t {
    PRODUCTION = 0,
    NORMAL = 1,
    DEBUG = 2
};

/**
 * Log Category Enum
 * Used to filter logs by subsystem
 */
enum class LogCategory : uint8_t {
    SYSTEM = 0,     // Boot, state machine, general
    HARDWARE = 1,   // I2C, UART, GPIO, hardware scanning
    NETWORK = 2,    // WiFi, BLE, connectivity
    SENSOR = 3,     // Sensor readings, measurements
    GPS = 4,        // GPS/GNSS specific
    API = 5,        // HTTP API, Hub communication
    STORAGE = 6,    // SD card, NVS, data persistence
    ERROR = 7       // Error conditions (always logged)
};

/**
 * Log Entry structure for buffering
 */
struct LogEntry {
    unsigned long timestamp;
    DebugLevel level;
    LogCategory category;
    String message;
    String stackTrace;
};

/**
 * Callback for log entries (used by SD logger and Hub upload)
 */
using LogCallback = std::function<void(const LogEntry&)>;

/**
 * DebugManager - Singleton class for debug logging
 */
class DebugManager {
public:
    static DebugManager& getInstance();

    // Prevent copying
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    /**
     * Initialize debug manager
     * Loads settings from NVS
     */
    void begin();

    /**
     * Set debug level (persists to NVS)
     */
    void setLevel(DebugLevel level);

    /**
     * Get current debug level
     */
    DebugLevel getLevel() const { return _level; }

    /**
     * Get debug level as string
     */
    const char* getLevelString() const;
    static const char* levelToString(DebugLevel level);

    /**
     * Parse debug level from string
     */
    static DebugLevel parseLevel(const String& str);

    /**
     * Enable/disable remote logging
     */
    void setRemoteLogging(bool enabled);
    bool isRemoteLoggingEnabled() const { return _remoteLoggingEnabled; }

    /**
     * Check if a category should be logged at current level
     */
    bool shouldLog(LogCategory category) const;
    bool shouldLog(DebugLevel minLevel) const;

    /**
     * Enable/disable specific categories
     */
    void enableCategory(LogCategory category);
    void disableCategory(LogCategory category);
    bool isCategoryEnabled(LogCategory category) const;

    /**
     * Log methods with category and level checking
     */
    void log(LogCategory category, DebugLevel minLevel, const char* format, ...);
    void logSystem(const char* format, ...);
    void logHardware(const char* format, ...);
    void logNetwork(const char* format, ...);
    void logSensor(const char* format, ...);
    void logGPS(const char* format, ...);
    void logAPI(const char* format, ...);
    void logStorage(const char* format, ...);
    void logError(const char* format, ...);
    void logDebug(LogCategory category, const char* format, ...);

    /**
     * Register callback for log entries
     */
    void onLog(LogCallback callback);

    /**
     * Get category name as string
     */
    static const char* categoryToString(LogCategory category);

    /**
     * Get statistics
     */
    uint32_t getLogCount() const { return _logCount; }
    uint32_t getErrorCount() const { return _errorCount; }
    void resetStatistics();

    /**
     * Performance measurement
     */
    unsigned long getLoggingOverheadUs() const { return _totalLoggingTimeUs; }
    void resetOverheadMeasurement();

private:
    DebugManager();

    void logInternal(LogCategory category, DebugLevel minLevel, const char* format, va_list args);
    void saveToNVS();
    void loadFromNVS();
    void notifyCallbacks(const LogEntry& entry);

    DebugLevel _level;
    bool _remoteLoggingEnabled;
    uint8_t _enabledCategories;  // Bitmask for enabled categories

    std::vector<LogCallback> _callbacks;

    // Statistics
    uint32_t _logCount;
    uint32_t _errorCount;
    unsigned long _totalLoggingTimeUs;

    // Buffer for formatting
    static constexpr size_t LOG_BUFFER_SIZE = 512;
    char _logBuffer[LOG_BUFFER_SIZE];

    // NVS keys
    static constexpr const char* NVS_NAMESPACE = "debug";
    static constexpr const char* NVS_KEY_LEVEL = "level";
    static constexpr const char* NVS_KEY_REMOTE = "remote";
    static constexpr const char* NVS_KEY_CATEGORIES = "cats";
};

// Convenience macros for logging
#define DBG_SYSTEM(fmt, ...) DebugManager::getInstance().logSystem(fmt, ##__VA_ARGS__)
#define DBG_HARDWARE(fmt, ...) DebugManager::getInstance().logHardware(fmt, ##__VA_ARGS__)
#define DBG_NETWORK(fmt, ...) DebugManager::getInstance().logNetwork(fmt, ##__VA_ARGS__)
#define DBG_SENSOR(fmt, ...) DebugManager::getInstance().logSensor(fmt, ##__VA_ARGS__)
#define DBG_GPS(fmt, ...) DebugManager::getInstance().logGPS(fmt, ##__VA_ARGS__)
#define DBG_API(fmt, ...) DebugManager::getInstance().logAPI(fmt, ##__VA_ARGS__)
#define DBG_STORAGE(fmt, ...) DebugManager::getInstance().logStorage(fmt, ##__VA_ARGS__)
#define DBG_ERROR(fmt, ...) DebugManager::getInstance().logError(fmt, ##__VA_ARGS__)
#define DBG_DEBUG(cat, fmt, ...) DebugManager::getInstance().logDebug(cat, fmt, ##__VA_ARGS__)

// Check macros for early exit
#define DBG_SHOULD_LOG(cat) DebugManager::getInstance().shouldLog(cat)
#define DBG_LEVEL DebugManager::getInstance().getLevel()

#endif // DEBUG_MANAGER_H
