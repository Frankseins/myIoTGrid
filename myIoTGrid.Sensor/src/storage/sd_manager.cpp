/**
 * myIoTGrid.Sensor - SD Card Manager Implementation
 */

#include "sd_manager.h"
#include <vector>

SDManager::SDManager()
    : _status(SDStatus::NOT_INITIALIZED)
    , _csPin(SD_PIN_CS)
    , _misoPin(SD_PIN_MISO)
    , _mosiPin(SD_PIN_MOSI)
    , _sckPin(SD_PIN_SCK)
#ifdef PLATFORM_ESP32
    , _spi(nullptr)
#endif
{
}

bool SDManager::init(int misoPin, int mosiPin, int sckPin, int csPin) {
#ifdef PLATFORM_ESP32
    _misoPin = misoPin;
    _mosiPin = mosiPin;
    _sckPin = sckPin;
    _csPin = csPin;

    Serial.println("[SDManager] Initializing SD card...");
    Serial.printf("[SDManager] Pins: MISO=%d, MOSI=%d, SCK=%d, CS=%d\n",
                  _misoPin, _mosiPin, _sckPin, _csPin);

    // Initialize SPI with custom pins
    // ESP32-S3 uses FSPI (SPI2_HOST), ESP32 uses VSPI (SPI3_HOST)
#if CONFIG_IDF_TARGET_ESP32S3
    _spi = new SPIClass(FSPI);
    Serial.println("[SDManager] Using FSPI for ESP32-S3");
#else
    _spi = new SPIClass(VSPI);
    Serial.println("[SDManager] Using VSPI for ESP32");
#endif
    _spi->begin(_sckPin, _misoPin, _mosiPin, _csPin);

    // Try to mount SD card
    if (!SD.begin(_csPin, *_spi)) {
        Serial.println("[SDManager] Card mount failed!");

        // Check if card is present
        uint8_t cardType = SD.cardType();
        if (cardType == CARD_NONE) {
            Serial.println("[SDManager] No SD card detected");
            _status = SDStatus::NO_CARD;
        } else {
            Serial.println("[SDManager] Mount failed - card may be incompatible");
            _status = SDStatus::MOUNT_FAILED;
        }
        return false;
    }

    // Check card type
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("[SDManager] No SD card detected");
        _status = SDStatus::NO_CARD;
        return false;
    }

    _status = SDStatus::MOUNTED;

    Serial.printf("[SDManager] Card type: %s\n", getCardTypeString());
    Serial.printf("[SDManager] Card size: %llu MB\n", getTotalBytes() / (1024 * 1024));
    Serial.printf("[SDManager] Free space: %llu MB\n", getFreeBytes() / (1024 * 1024));

    // Setup directory structure
    if (!setupDirectoryStructure()) {
        Serial.println("[SDManager] Warning: Could not create directory structure");
    }

    Serial.println("[SDManager] SD card initialized successfully");
    return true;
#else
    Serial.println("[SDManager] SD card not supported on this platform");
    _status = SDStatus::NOT_INITIALIZED;
    return false;
#endif
}

const char* SDManager::getStatusString() const {
    switch (_status) {
        case SDStatus::NOT_INITIALIZED: return "NOT_INITIALIZED";
        case SDStatus::MOUNTED: return "MOUNTED";
        case SDStatus::NO_CARD: return "NO_CARD";
        case SDStatus::MOUNT_FAILED: return "MOUNT_FAILED";
        case SDStatus::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

uint64_t SDManager::getTotalBytes() const {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return 0;
    return SD.totalBytes();
#else
    return 0;
#endif
}

uint64_t SDManager::getUsedBytes() const {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return 0;
    return SD.usedBytes();
#else
    return 0;
#endif
}

uint64_t SDManager::getFreeBytes() const {
    return getTotalBytes() - getUsedBytes();
}

bool SDManager::hasEnoughSpace(uint64_t requiredBytes) const {
    return getFreeBytes() >= requiredBytes;
}

bool SDManager::createDirectory(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    if (SD.exists(path)) {
        return true; // Already exists
    }

    if (SD.mkdir(path)) {
        Serial.printf("[SDManager] Created directory: %s\n", path);
        return true;
    }

    Serial.printf("[SDManager] Failed to create directory: %s\n", path);
    return false;
#else
    return false;
#endif
}

bool SDManager::createDirectoryRecursive(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    String pathStr = String(path);
    String currentPath = "";

    int startIdx = (pathStr[0] == '/') ? 1 : 0;

    while (true) {
        int idx = pathStr.indexOf('/', startIdx);
        if (idx == -1) {
            currentPath = pathStr;
        } else {
            currentPath = pathStr.substring(0, idx);
        }

        if (currentPath.length() > 0 && !SD.exists(currentPath.c_str())) {
            if (!SD.mkdir(currentPath.c_str())) {
                Serial.printf("[SDManager] Failed to create: %s\n", currentPath.c_str());
                return false;
            }
        }

        if (idx == -1) break;
        startIdx = idx + 1;
    }

    return true;
#else
    return false;
#endif
}

bool SDManager::fileExists(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;
    return SD.exists(path);
#else
    return false;
#endif
}

bool SDManager::directoryExists(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    File dir = SD.open(path);
    if (!dir) return false;

    bool isDir = dir.isDirectory();
    dir.close();
    return isDir;
#else
    return false;
#endif
}

bool SDManager::deleteFile(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    if (SD.remove(path)) {
        Serial.printf("[SDManager] Deleted file: %s\n", path);
        return true;
    }
    return false;
#else
    return false;
#endif
}

bool SDManager::deleteDirectory(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    if (SD.rmdir(path)) {
        Serial.printf("[SDManager] Deleted directory: %s\n", path);
        return true;
    }
    return false;
#else
    return false;
#endif
}

int64_t SDManager::getFileSize(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return -1;

    File file = SD.open(path);
    if (!file) return -1;

    int64_t size = file.size();
    file.close();
    return size;
#else
    return -1;
#endif
}

void SDManager::listDirectory(const char* path,
                              std::function<void(const String&, size_t, bool)> callback) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return;

    File root = SD.open(path);
    if (!root || !root.isDirectory()) {
        if (root) root.close();
        return;
    }

    File file = root.openNextFile();
    while (file) {
        callback(String(file.name()), file.size(), file.isDirectory());
        file = root.openNextFile();
    }
    root.close();
#endif
}

bool SDManager::writeFile(const char* path, const String& content) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("[SDManager] Failed to open file for writing: %s\n", path);
        return false;
    }

    size_t written = file.print(content);
    file.close();

    if (written != content.length()) {
        Serial.printf("[SDManager] Write incomplete: %d/%d bytes\n",
                      written, content.length());
        return false;
    }

    return true;
#else
    return false;
#endif
}

bool SDManager::appendFile(const char* path, const String& content) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.printf("[SDManager] Failed to open file for appending: %s\n", path);
        return false;
    }

    size_t written = file.print(content);
    file.close();

    return written == content.length();
#else
    return false;
#endif
}

String SDManager::readFile(const char* path) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return "";

    File file = SD.open(path, FILE_READ);
    if (!file) {
        return "";
    }

    String content = file.readString();
    file.close();
    return content;
#else
    return "";
#endif
}

bool SDManager::renameFile(const char* oldPath, const char* newPath) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;
    return SD.rename(oldPath, newPath);
#else
    return false;
#endif
}

bool SDManager::setupDirectoryStructure() {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return false;

    Serial.println("[SDManager] Setting up directory structure...");

    bool success = true;

    // Create base directory
    if (!createDirectoryRecursive(SD_BASE_DIR)) {
        Serial.println("[SDManager] Failed to create base directory");
        success = false;
    }

    // Create readings directory
    if (!createDirectoryRecursive(SD_READINGS_DIR)) {
        Serial.println("[SDManager] Failed to create readings directory");
        success = false;
    }

    // Create pending directory
    if (!createDirectoryRecursive(SD_PENDING_DIR)) {
        Serial.println("[SDManager] Failed to create pending directory");
        success = false;
    }

    if (success) {
        Serial.println("[SDManager] Directory structure created successfully");
    }

    return success;
#else
    return false;
#endif
}

uint64_t SDManager::cleanupOldFiles(uint64_t targetFreeBytes) {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return 0;

    uint64_t freedBytes = 0;
    uint64_t currentFree = getFreeBytes();

    if (currentFree >= targetFreeBytes) {
        return 0; // Already have enough space
    }

    Serial.printf("[SDManager] Cleanup needed: have %llu bytes, need %llu bytes\n",
                  currentFree, targetFreeBytes);

    // Collect files with dates from readings directory
    struct FileInfo {
        String path;
        size_t size;
        String date; // YYYYMMDD format from filename
    };
    std::vector<FileInfo> files;

    File root = SD.open(SD_READINGS_DIR);
    if (root && root.isDirectory()) {
        File file = root.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                String name = String(file.name());
                // Only consider synced CSV files (readings_YYYYMMDD_synced.csv)
                if (name.endsWith("_synced.csv")) {
                    FileInfo info;
                    info.path = String(SD_READINGS_DIR) + "/" + name;
                    info.size = file.size();
                    // Extract date from filename (readings_YYYYMMDD_synced.csv)
                    int idx = name.indexOf('_');
                    if (idx > 0 && name.length() > idx + 9) {
                        info.date = name.substring(idx + 1, idx + 9);
                    }
                    files.push_back(info);
                }
            }
            file = root.openNextFile();
        }
        root.close();
    }

    // Sort by date (oldest first)
    std::sort(files.begin(), files.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.date < b.date;
    });

    // Delete oldest files until we have enough space
    for (const auto& fileInfo : files) {
        if (getFreeBytes() >= targetFreeBytes) {
            break;
        }

        Serial.printf("[SDManager] Deleting old file: %s (%d bytes)\n",
                      fileInfo.path.c_str(), fileInfo.size);

        if (SD.remove(fileInfo.path.c_str())) {
            freedBytes += fileInfo.size;
        }
    }

    Serial.printf("[SDManager] Cleanup complete: freed %llu bytes\n", freedBytes);
    return freedBytes;
#else
    return 0;
#endif
}

void SDManager::unmount() {
#ifdef PLATFORM_ESP32
    if (_status == SDStatus::MOUNTED) {
        SD.end();
        _status = SDStatus::NOT_INITIALIZED;
        Serial.println("[SDManager] SD card unmounted");
    }
#endif
}

bool SDManager::remount() {
    unmount();
    return init(_misoPin, _mosiPin, _sckPin, _csPin);
}

const char* SDManager::getCardTypeString() const {
#ifdef PLATFORM_ESP32
    if (_status != SDStatus::MOUNTED) return "NONE";

    uint8_t cardType = SD.cardType();
    switch (cardType) {
        case CARD_MMC: return "MMC";
        case CARD_SD: return "SD";
        case CARD_SDHC: return "SDHC";
        default: return "UNKNOWN";
    }
#else
    return "NONE";
#endif
}
