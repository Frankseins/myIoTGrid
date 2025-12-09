/**
 * myIoTGrid.Sensor - SD Card Logger Implementation
 * Sprint 8: Remote Debug System
 */

#include "sd_logger.h"
#include <ArduinoJson.h>

// Singleton instance
SDLogger& SDLogger::getInstance() {
    static SDLogger instance;
    return instance;
}

SDLogger::SDLogger()
    : _sdAvailable(false)
    , _currentFileNumber(1)
    , _currentFileSize(0)
    , _lastFlushTime(0) {
#ifdef PLATFORM_ESP32
    _logQueue = nullptr;
    _fileMutex = nullptr;
#endif
}

SDLogger::~SDLogger() {
    closeCurrentFile();
#ifdef PLATFORM_ESP32
    if (_logQueue) {
        vQueueDelete(_logQueue);
    }
    if (_fileMutex) {
        vSemaphoreDelete(_fileMutex);
    }
#endif
}

bool SDLogger::begin(int csPin) {
#ifdef PLATFORM_ESP32
    Serial.printf("[SDLogger] Initializing SD card on CS pin %d...\n", csPin);

    // Create mutex and queue
    _fileMutex = xSemaphoreCreateMutex();
    _logQueue = xQueueCreate(_config.maxQueueSize, sizeof(LogEntry*));

    if (!_fileMutex || !_logQueue) {
        Serial.println("[SDLogger] Failed to create mutex/queue");
        return false;
    }

    // Initialize SD card
    if (!SD.begin(csPin)) {
        Serial.println("[SDLogger] SD card mount failed - logging disabled");
        _sdAvailable = false;
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("[SDLogger] No SD card attached");
        _sdAvailable = false;
        return false;
    }

    // Print card info
    const char* typeStr = "UNKNOWN";
    switch (cardType) {
        case CARD_MMC: typeStr = "MMC"; break;
        case CARD_SD: typeStr = "SDSC"; break;
        case CARD_SDHC: typeStr = "SDHC"; break;
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("[SDLogger] SD card type: %s, Size: %lluMB\n", typeStr, cardSize);

    // Create log directory if needed
    if (!SD.exists(LOG_DIR)) {
        if (!SD.mkdir(LOG_DIR)) {
            Serial.println("[SDLogger] Failed to create log directory");
            _sdAvailable = false;
            return false;
        }
    }

    // Find current file number
    _currentFileNumber = getNextFileNumber();
    Serial.printf("[SDLogger] Starting with file number: %d\n", _currentFileNumber);

    _sdAvailable = true;
    _lastFlushTime = millis();

    // Register with DebugManager
    DebugManager::getInstance().onLog([this](const LogEntry& entry) {
        this->log(entry);
    });

    Serial.println("[SDLogger] SD card logging initialized");
    return true;

#else
    // Native build - no SD card support
    Serial.println("[SDLogger] SD card not available on this platform");
    _sdAvailable = false;
    return false;
#endif
}

void SDLogger::configure(const SDLoggerConfig& config) {
    _config = config;
}

void SDLogger::log(const LogEntry& entry) {
    if (!isEnabled()) return;

#ifdef PLATFORM_ESP32
    // Create a copy of the entry for the queue
    LogEntry* entryCopy = new LogEntry(entry);

    // Try to add to queue (non-blocking)
    if (xQueueSend(_logQueue, &entryCopy, 0) != pdTRUE) {
        // Queue full, drop the entry
        delete entryCopy;
        _stats.entriesDropped++;
    }
#endif
}

void SDLogger::loop() {
    if (!isEnabled()) return;

#ifdef PLATFORM_ESP32
    // Process queue
    LogEntry* entry;
    int processedCount = 0;
    const int maxPerLoop = 10;  // Process max 10 entries per loop

    while (processedCount < maxPerLoop && xQueueReceive(_logQueue, &entry, 0) == pdTRUE) {
        writeEntry(*entry);
        delete entry;
        processedCount++;
    }

    // Periodic flush
    if (millis() - _lastFlushTime >= _config.flushIntervalMs) {
        flush();
    }
#endif
}

void SDLogger::flush() {
#ifdef PLATFORM_ESP32
    if (!_sdAvailable || !_currentFile) return;

    if (xSemaphoreTake(_fileMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        _currentFile.flush();
        _lastFlushTime = millis();
        _stats.lastFlushTime = _lastFlushTime;
        xSemaphoreGive(_fileMutex);
    }
#endif
}

void SDLogger::writeEntry(const LogEntry& entry) {
#ifdef PLATFORM_ESP32
    if (xSemaphoreTake(_fileMutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return;
    }

    // Open file if not open
    if (!_currentFile) {
        openCurrentFile();
    }

    if (_currentFile) {
        String line = formatEntry(entry);
        size_t written = _currentFile.println(line);

        if (written > 0) {
            _currentFileSize += written + 1;  // +1 for newline
            _stats.bytesWritten += written + 1;
            _stats.entriesWritten++;

            // Check if rotation needed
            rotateFileIfNeeded();
        }
    }

    xSemaphoreGive(_fileMutex);
#endif
}

String SDLogger::formatEntry(const LogEntry& entry) const {
    // JSON-Lines format for easy parsing
    JsonDocument doc;

    doc["ts"] = entry.timestamp;
    doc["lvl"] = DebugManager::levelToString(entry.level);
    doc["cat"] = DebugManager::categoryToString(entry.category);
    doc["msg"] = entry.message;

    if (entry.stackTrace.length() > 0) {
        doc["stack"] = entry.stackTrace;
    }

    String output;
    serializeJson(doc, output);
    return output;
}

void SDLogger::rotateFileIfNeeded() {
#ifdef PLATFORM_ESP32
    if (_currentFileSize >= _config.maxFileSize) {
        closeCurrentFile();

        // Move to next file number (circular)
        _currentFileNumber++;
        if (_currentFileNumber > _config.maxFiles) {
            _currentFileNumber = 1;
        }

        // Delete old file if exists
        String filePath = getFilePath(_currentFileNumber);
        if (SD.exists(filePath)) {
            SD.remove(filePath);
        }

        _stats.filesRotated++;
        Serial.printf("[SDLogger] Rotated to file: %s\n", filePath.c_str());
    }
#endif
}

void SDLogger::openCurrentFile() {
#ifdef PLATFORM_ESP32
    String filePath = getFilePath(_currentFileNumber);
    _currentFile = SD.open(filePath, FILE_APPEND);

    if (_currentFile) {
        _currentFileSize = _currentFile.size();
    } else {
        Serial.printf("[SDLogger] Failed to open: %s\n", filePath.c_str());
    }
#endif
}

void SDLogger::closeCurrentFile() {
#ifdef PLATFORM_ESP32
    if (_currentFile) {
        _currentFile.close();
        _currentFileSize = 0;
    }
#endif
}

String SDLogger::getCurrentFilePath() const {
    return getFilePath(_currentFileNumber);
}

String SDLogger::getFilePath(int fileNumber) const {
    char filename[32];
    snprintf(filename, sizeof(filename), "%s/%s%03d%s",
             LOG_DIR, LOG_PREFIX, fileNumber, LOG_EXT);
    return String(filename);
}

int SDLogger::getNextFileNumber() const {
#ifdef PLATFORM_ESP32
    // Find the newest file
    int newest = 1;
    unsigned long newestTime = 0;

    for (int i = 1; i <= _config.maxFiles; i++) {
        String path = getFilePath(i);
        if (SD.exists(path)) {
            File f = SD.open(path, FILE_READ);
            if (f) {
                // Use file number as proxy for age (higher = newer)
                if (i > newest) {
                    newest = i;
                }
                f.close();
            }
        }
    }

    return newest;
#else
    return 1;
#endif
}

std::vector<String> SDLogger::getLogFiles() const {
    std::vector<String> files;

#ifdef PLATFORM_ESP32
    for (int i = 1; i <= _config.maxFiles; i++) {
        String path = getFilePath(i);
        if (SD.exists(path)) {
            files.push_back(path);
        }
    }
#endif

    return files;
}

String SDLogger::readLogFile(const String& filename, int maxLines) const {
    String content;

#ifdef PLATFORM_ESP32
    File file = SD.open(filename, FILE_READ);
    if (file) {
        int lineCount = 0;
        while (file.available()) {
            String line = file.readStringUntil('\n');
            content += line + "\n";

            if (maxLines > 0 && ++lineCount >= maxLines) {
                break;
            }
        }
        file.close();
    }
#endif

    return content;
}

String SDLogger::readRecentLogs(int maxLines) const {
    String combined;
    std::vector<String> allLines;

#ifdef PLATFORM_ESP32
    // Read all files in reverse order (newest first)
    for (int i = _currentFileNumber; allLines.size() < (size_t)maxLines; i--) {
        if (i < 1) i = _config.maxFiles;
        if (i == _currentFileNumber && allLines.size() > 0) break;  // Full circle

        String path = getFilePath(i);
        if (SD.exists(path)) {
            File file = SD.open(path, FILE_READ);
            if (file) {
                std::vector<String> fileLines;
                while (file.available()) {
                    fileLines.push_back(file.readStringUntil('\n'));
                }
                file.close();

                // Add in reverse order
                for (int j = fileLines.size() - 1; j >= 0 && allLines.size() < (size_t)maxLines; j--) {
                    allLines.push_back(fileLines[j]);
                }
            }
        }
    }

    // Build result
    for (const auto& line : allLines) {
        combined += line + "\n";
    }
#endif

    return combined;
}

void SDLogger::clearLogs() {
#ifdef PLATFORM_ESP32
    closeCurrentFile();

    for (int i = 1; i <= _config.maxFiles; i++) {
        String path = getFilePath(i);
        if (SD.exists(path)) {
            SD.remove(path);
        }
    }

    _currentFileNumber = 1;
    _currentFileSize = 0;
    _stats = SDLoggerStats();

    Serial.println("[SDLogger] All logs cleared");
#endif
}

uint64_t SDLogger::getCardSize() const {
#ifdef PLATFORM_ESP32
    return SD.cardSize();
#else
    return 0;
#endif
}

uint64_t SDLogger::getUsedSpace() const {
#ifdef PLATFORM_ESP32
    return SD.usedBytes();
#else
    return 0;
#endif
}

uint64_t SDLogger::getFreeSpace() const {
#ifdef PLATFORM_ESP32
    return SD.totalBytes() - SD.usedBytes();
#else
    return 0;
#endif
}
