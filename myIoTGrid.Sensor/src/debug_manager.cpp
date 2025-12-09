/**
 * myIoTGrid.Sensor - Debug Manager Implementation
 * Sprint 8: Remote Debug System
 */

#include "debug_manager.h"

#ifdef PLATFORM_ESP32
#include <Preferences.h>
#endif

// Singleton instance
DebugManager& DebugManager::getInstance() {
    static DebugManager instance;
    return instance;
}

DebugManager::DebugManager()
    : _level(DebugLevel::NORMAL)
    , _remoteLoggingEnabled(false)
    , _enabledCategories(0xFF)  // All categories enabled by default
    , _logCount(0)
    , _errorCount(0)
    , _totalLoggingTimeUs(0) {
}

void DebugManager::begin() {
    loadFromNVS();
    Serial.printf("[Debug] Manager initialized - Level: %s, Remote: %s\n",
                  getLevelString(),
                  _remoteLoggingEnabled ? "enabled" : "disabled");
}

void DebugManager::setLevel(DebugLevel level) {
    if (_level != level) {
        _level = level;
        saveToNVS();
        Serial.printf("[Debug] Level changed to: %s\n", getLevelString());
    }
}

const char* DebugManager::getLevelString() const {
    return levelToString(_level);
}

const char* DebugManager::levelToString(DebugLevel level) {
    switch (level) {
        case DebugLevel::PRODUCTION: return "PRODUCTION";
        case DebugLevel::NORMAL: return "NORMAL";
        case DebugLevel::DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

DebugLevel DebugManager::parseLevel(const String& str) {
    if (str.equalsIgnoreCase("PRODUCTION") || str == "0") {
        return DebugLevel::PRODUCTION;
    } else if (str.equalsIgnoreCase("NORMAL") || str == "1") {
        return DebugLevel::NORMAL;
    } else if (str.equalsIgnoreCase("DEBUG") || str == "2") {
        return DebugLevel::DEBUG;
    }
    return DebugLevel::NORMAL;  // Default
}

void DebugManager::setRemoteLogging(bool enabled) {
    if (_remoteLoggingEnabled != enabled) {
        _remoteLoggingEnabled = enabled;
        saveToNVS();
        Serial.printf("[Debug] Remote logging: %s\n", enabled ? "enabled" : "disabled");
    }
}

bool DebugManager::shouldLog(LogCategory category) const {
    // Errors always logged
    if (category == LogCategory::ERROR) {
        return true;
    }

    // Check if category is enabled
    if (!isCategoryEnabled(category)) {
        return false;
    }

    // Level-based filtering
    switch (_level) {
        case DebugLevel::PRODUCTION:
            // Only errors in production
            return false;

        case DebugLevel::NORMAL:
            // System, Network, API, Error, and Storage (important subsystems)
            return category == LogCategory::SYSTEM ||
                   category == LogCategory::NETWORK ||
                   category == LogCategory::API ||
                   category == LogCategory::STORAGE;

        case DebugLevel::DEBUG:
            // Everything
            return true;
    }
    return false;
}

bool DebugManager::shouldLog(DebugLevel minLevel) const {
    return static_cast<uint8_t>(_level) >= static_cast<uint8_t>(minLevel);
}

void DebugManager::enableCategory(LogCategory category) {
    _enabledCategories |= (1 << static_cast<uint8_t>(category));
}

void DebugManager::disableCategory(LogCategory category) {
    _enabledCategories &= ~(1 << static_cast<uint8_t>(category));
}

bool DebugManager::isCategoryEnabled(LogCategory category) const {
    return (_enabledCategories & (1 << static_cast<uint8_t>(category))) != 0;
}

void DebugManager::log(LogCategory category, DebugLevel minLevel, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(category, minLevel, format, args);
    va_end(args);
}

void DebugManager::logSystem(const char* format, ...) {
    if (!shouldLog(LogCategory::SYSTEM)) return;
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::SYSTEM, DebugLevel::NORMAL, format, args);
    va_end(args);
}

void DebugManager::logHardware(const char* format, ...) {
    if (!shouldLog(LogCategory::HARDWARE)) return;
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::HARDWARE, DebugLevel::DEBUG, format, args);
    va_end(args);
}

void DebugManager::logNetwork(const char* format, ...) {
    if (!shouldLog(LogCategory::NETWORK)) return;
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::NETWORK, DebugLevel::NORMAL, format, args);
    va_end(args);
}

void DebugManager::logSensor(const char* format, ...) {
    if (!shouldLog(LogCategory::SENSOR)) return;
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::SENSOR, DebugLevel::DEBUG, format, args);
    va_end(args);
}

void DebugManager::logGPS(const char* format, ...) {
    if (!shouldLog(LogCategory::GPS)) return;
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::GPS, DebugLevel::DEBUG, format, args);
    va_end(args);
}

void DebugManager::logAPI(const char* format, ...) {
    if (!shouldLog(LogCategory::API)) return;
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::API, DebugLevel::NORMAL, format, args);
    va_end(args);
}

void DebugManager::logStorage(const char* format, ...) {
    if (!shouldLog(LogCategory::STORAGE)) return;
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::STORAGE, DebugLevel::NORMAL, format, args);
    va_end(args);
}

void DebugManager::logError(const char* format, ...) {
    // Errors are always logged
    va_list args;
    va_start(args, format);
    logInternal(LogCategory::ERROR, DebugLevel::PRODUCTION, format, args);
    va_end(args);
    _errorCount++;
}

void DebugManager::logDebug(LogCategory category, const char* format, ...) {
    if (_level != DebugLevel::DEBUG) return;
    va_list args;
    va_start(args, format);
    logInternal(category, DebugLevel::DEBUG, format, args);
    va_end(args);
}

void DebugManager::logInternal(LogCategory category, DebugLevel minLevel, const char* format, va_list args) {
    unsigned long startTime = micros();

    // Format the message
    vsnprintf(_logBuffer, LOG_BUFFER_SIZE, format, args);

    // Get category prefix
    const char* prefix = categoryToString(category);

    // Print to Serial
    Serial.printf("[%s] %s\n", prefix, _logBuffer);

    // Increment log count
    _logCount++;

    // Create log entry and notify callbacks (for SD logger, Hub upload)
    if (_remoteLoggingEnabled && !_callbacks.empty()) {
        LogEntry entry;
        entry.timestamp = millis();
        entry.level = minLevel;
        entry.category = category;
        entry.message = String(_logBuffer);
        notifyCallbacks(entry);
    }

    // Track overhead
    _totalLoggingTimeUs += (micros() - startTime);
}

void DebugManager::onLog(LogCallback callback) {
    _callbacks.push_back(callback);
}

void DebugManager::notifyCallbacks(const LogEntry& entry) {
    for (auto& callback : _callbacks) {
        callback(entry);
    }
}

const char* DebugManager::categoryToString(LogCategory category) {
    switch (category) {
        case LogCategory::SYSTEM: return "System";
        case LogCategory::HARDWARE: return "Hardware";
        case LogCategory::NETWORK: return "Network";
        case LogCategory::SENSOR: return "Sensor";
        case LogCategory::GPS: return "GPS";
        case LogCategory::API: return "API";
        case LogCategory::STORAGE: return "Storage";
        case LogCategory::ERROR: return "ERROR";
        default: return "Unknown";
    }
}

void DebugManager::resetStatistics() {
    _logCount = 0;
    _errorCount = 0;
}

void DebugManager::resetOverheadMeasurement() {
    _totalLoggingTimeUs = 0;
}

void DebugManager::saveToNVS() {
#ifdef PLATFORM_ESP32
    Preferences prefs;
    if (prefs.begin(NVS_NAMESPACE, false)) {
        prefs.putUChar(NVS_KEY_LEVEL, static_cast<uint8_t>(_level));
        prefs.putBool(NVS_KEY_REMOTE, _remoteLoggingEnabled);
        prefs.putUChar(NVS_KEY_CATEGORIES, _enabledCategories);
        prefs.end();
    }
#endif
}

void DebugManager::loadFromNVS() {
#ifdef PLATFORM_ESP32
    Preferences prefs;
    if (prefs.begin(NVS_NAMESPACE, true)) {
        _level = static_cast<DebugLevel>(prefs.getUChar(NVS_KEY_LEVEL, static_cast<uint8_t>(DebugLevel::NORMAL)));
        _remoteLoggingEnabled = prefs.getBool(NVS_KEY_REMOTE, false);
        _enabledCategories = prefs.getUChar(NVS_KEY_CATEGORIES, 0xFF);
        prefs.end();
    }
#endif
}
