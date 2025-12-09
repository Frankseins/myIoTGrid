/**
 * myIoTGrid.Sensor - SD Card Manager
 *
 * Handles SD card initialization, mounting, and file operations.
 * Part of Sprint OS-01: Offline-Speicher Implementation
 */

#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include <Arduino.h>

#ifdef PLATFORM_ESP32
#include <SD.h>
#include <SPI.h>
#endif

// ============================================================================
// SD Card Pin Configuration (configurable)
// ============================================================================

#ifndef SD_PIN_MISO
#define SD_PIN_MISO     19    // SPI Master In (default)
#endif

#ifndef SD_PIN_MOSI
#define SD_PIN_MOSI     23    // SPI Master Out (default)
#endif

#ifndef SD_PIN_SCK
#define SD_PIN_SCK      18    // SPI Clock (default)
#endif

#ifndef SD_PIN_CS
#define SD_PIN_CS       5     // Chip Select (default)
#endif

// ============================================================================
// SD Card Configuration
// ============================================================================

#define SD_BASE_DIR         "/iotgrid"
#define SD_READINGS_DIR     "/iotgrid/readings"
#define SD_PENDING_DIR      "/iotgrid/pending"
#define SD_CONFIG_FILE      "/iotgrid/config.json"
#define SD_SYNC_STATUS_FILE "/iotgrid/sync_status.json"

// Minimum free space to keep (bytes) - 1 MB
#define SD_MIN_FREE_SPACE   1048576

/**
 * SD Card Status
 */
enum class SDStatus {
    NOT_INITIALIZED,
    MOUNTED,
    NO_CARD,
    MOUNT_FAILED,
    ERROR
};

/**
 * SD Card Manager - Handles all SD card operations
 */
class SDManager {
public:
    SDManager();

    /**
     * Initialize the SD card with configurable pins
     * @param misoPin MISO pin (default: SD_PIN_MISO)
     * @param mosiPin MOSI pin (default: SD_PIN_MOSI)
     * @param sckPin SCK pin (default: SD_PIN_SCK)
     * @param csPin CS pin (default: SD_PIN_CS)
     * @return true if initialization successful
     */
    bool init(int misoPin = SD_PIN_MISO, int mosiPin = SD_PIN_MOSI,
              int sckPin = SD_PIN_SCK, int csPin = SD_PIN_CS);

    /**
     * Check if SD card is available and mounted
     */
    bool isAvailable() const { return _status == SDStatus::MOUNTED; }

    /**
     * Get current SD card status
     */
    SDStatus getStatus() const { return _status; }

    /**
     * Get status as string
     */
    const char* getStatusString() const;

    /**
     * Get total card size in bytes
     */
    uint64_t getTotalBytes() const;

    /**
     * Get used space in bytes
     */
    uint64_t getUsedBytes() const;

    /**
     * Get free space in bytes
     */
    uint64_t getFreeBytes() const;

    /**
     * Check if there's enough free space
     * @param requiredBytes bytes needed
     */
    bool hasEnoughSpace(uint64_t requiredBytes) const;

    /**
     * Create directory if it doesn't exist
     * @param path directory path
     */
    bool createDirectory(const char* path);

    /**
     * Check if file exists
     * @param path file path
     */
    bool fileExists(const char* path);

    /**
     * Check if directory exists
     * @param path directory path
     */
    bool directoryExists(const char* path);

    /**
     * Delete file
     * @param path file path
     */
    bool deleteFile(const char* path);

    /**
     * Delete directory (must be empty)
     * @param path directory path
     */
    bool deleteDirectory(const char* path);

    /**
     * Get file size
     * @param path file path
     * @return file size in bytes, or -1 if not found
     */
    int64_t getFileSize(const char* path);

    /**
     * List files in directory
     * @param path directory path
     * @param callback function to call for each file (name, size, isDir)
     */
    void listDirectory(const char* path,
                       std::function<void(const String&, size_t, bool)> callback);

    /**
     * Write string to file (creates or overwrites)
     * @param path file path
     * @param content content to write
     * @return true if successful
     */
    bool writeFile(const char* path, const String& content);

    /**
     * Append string to file
     * @param path file path
     * @param content content to append
     * @return true if successful
     */
    bool appendFile(const char* path, const String& content);

    /**
     * Read file content as string
     * @param path file path
     * @return file content or empty string on error
     */
    String readFile(const char* path);

    /**
     * Rename/move file
     * @param oldPath current path
     * @param newPath new path
     */
    bool renameFile(const char* oldPath, const char* newPath);

    /**
     * Setup directory structure for IoTGrid
     * Creates /iotgrid/readings/, /iotgrid/pending/, etc.
     */
    bool setupDirectoryStructure();

    /**
     * Clean up old synced files to free space
     * @param targetFreeBytes free up until this much space is available
     * @return bytes freed
     */
    uint64_t cleanupOldFiles(uint64_t targetFreeBytes = SD_MIN_FREE_SPACE);

    /**
     * Unmount SD card
     */
    void unmount();

    /**
     * Remount SD card (useful after card swap)
     */
    bool remount();

    /**
     * Get card type string (SD, SDHC, SDXC, etc.)
     */
    const char* getCardTypeString() const;

private:
    SDStatus _status;
    int _csPin;
    int _misoPin;
    int _mosiPin;
    int _sckPin;

#ifdef PLATFORM_ESP32
    SPIClass* _spi;
#endif

    /**
     * Internal: Create directory recursively
     */
    bool createDirectoryRecursive(const char* path);
};

#endif // SD_MANAGER_H
