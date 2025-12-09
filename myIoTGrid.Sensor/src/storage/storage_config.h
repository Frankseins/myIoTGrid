/**
 * myIoTGrid.Sensor - Storage Configuration
 *
 * Storage mode configuration for offline/online data handling.
 * Part of Sprint OS-01: Offline-Speicher Implementation
 */

#ifndef STORAGE_CONFIG_H
#define STORAGE_CONFIG_H

#include <Arduino.h>

/**
 * Storage Mode - Defines how sensor readings are handled
 */
enum class StorageMode {
    /**
     * REMOTE_ONLY: Send directly to API, no local storage
     * - SD card not used
     * - Readings lost if WiFi unavailable
     * - Lowest latency
     */
    REMOTE_ONLY,

    /**
     * LOCAL_AND_REMOTE: Store locally AND send to API
     * - Always stores on SD card
     * - Also sends to API if WiFi available
     * - Maximum data safety
     */
    LOCAL_AND_REMOTE,

    /**
     * LOCAL_ONLY: Store locally only, no API calls
     * - Only stores on SD card
     * - Manual export required
     * - For offline deployments
     */
    LOCAL_ONLY,

    /**
     * LOCAL_AUTOSYNC: Store locally, auto-sync when WiFi available (DEFAULT)
     * - Stores on SD card first
     * - Automatically syncs to API when WiFi connected
     * - Best for intermittent connectivity
     */
    LOCAL_AUTOSYNC
};

/**
 * Sync Strategy - When to sync pending data
 */
enum class SyncStrategy {
    /**
     * IMMEDIATE: Sync as soon as WiFi is available
     */
    IMMEDIATE,

    /**
     * BATCH: Sync when batch size reached
     */
    BATCH,

    /**
     * SCHEDULED: Sync at specific intervals
     */
    SCHEDULED,

    /**
     * MANUAL: Only sync on button press
     */
    MANUAL
};

/**
 * Storage Configuration
 */
struct StorageConfig {
    // Storage mode
    StorageMode mode = StorageMode::LOCAL_AUTOSYNC;

    // Sync settings
    SyncStrategy syncStrategy = SyncStrategy::IMMEDIATE;
    int batchSize = 50;                         // Readings per sync batch
    unsigned long syncIntervalMs = 60000;       // 1 minute for scheduled sync

    // Retry settings
    int maxRetries = 5;                         // Max retries before giving up
    unsigned long initialRetryDelayMs = 60000;  // 1 minute initial delay
    unsigned long maxRetryDelayMs = 900000;     // 15 minutes max delay

    // Cleanup settings
    bool autoCleanup = true;                    // Auto-delete old synced files
    int keepSyncedDays = 7;                     // Keep synced files for X days
    uint64_t minFreeBytes = 1048576;            // 1 MB minimum free space

    // Feature flags
    bool enableStatusLed = true;
    bool enableSyncButton = true;

    /**
     * Get mode as string
     */
    static const char* getModeString(StorageMode mode) {
        switch (mode) {
            case StorageMode::REMOTE_ONLY: return "REMOTE_ONLY";
            case StorageMode::LOCAL_AND_REMOTE: return "LOCAL_AND_REMOTE";
            case StorageMode::LOCAL_ONLY: return "LOCAL_ONLY";
            case StorageMode::LOCAL_AUTOSYNC: return "LOCAL_AUTOSYNC";
            default: return "UNKNOWN";
        }
    }

    /**
     * Parse mode from string
     */
    static StorageMode parseMode(const String& str) {
        if (str == "REMOTE_ONLY") return StorageMode::REMOTE_ONLY;
        if (str == "LOCAL_AND_REMOTE") return StorageMode::LOCAL_AND_REMOTE;
        if (str == "LOCAL_ONLY") return StorageMode::LOCAL_ONLY;
        if (str == "LOCAL_AUTOSYNC") return StorageMode::LOCAL_AUTOSYNC;
        return StorageMode::LOCAL_AUTOSYNC; // Default
    }

    /**
     * Get sync strategy as string
     */
    static const char* getSyncStrategyString(SyncStrategy strategy) {
        switch (strategy) {
            case SyncStrategy::IMMEDIATE: return "IMMEDIATE";
            case SyncStrategy::BATCH: return "BATCH";
            case SyncStrategy::SCHEDULED: return "SCHEDULED";
            case SyncStrategy::MANUAL: return "MANUAL";
            default: return "UNKNOWN";
        }
    }

    /**
     * Parse sync strategy from string
     */
    static SyncStrategy parseSyncStrategy(const String& str) {
        if (str == "IMMEDIATE") return SyncStrategy::IMMEDIATE;
        if (str == "BATCH") return SyncStrategy::BATCH;
        if (str == "SCHEDULED") return SyncStrategy::SCHEDULED;
        if (str == "MANUAL") return SyncStrategy::MANUAL;
        return SyncStrategy::IMMEDIATE; // Default
    }
};

/**
 * Storage Config Manager - Reads/writes config from SD card
 */
class StorageConfigManager {
public:
    StorageConfigManager();

    /**
     * Load configuration from SD card
     * @param sdManager reference to SD manager
     * @return true if loaded successfully
     */
    bool load(class SDManager& sdManager);

    /**
     * Save configuration to SD card
     * @param sdManager reference to SD manager
     * @return true if saved successfully
     */
    bool save(class SDManager& sdManager);

    /**
     * Get current configuration
     */
    StorageConfig& getConfig() { return _config; }
    const StorageConfig& getConfig() const { return _config; }

    /**
     * Set storage mode
     */
    void setMode(StorageMode mode) { _config.mode = mode; }

    /**
     * Get storage mode
     */
    StorageMode getMode() const { return _config.mode; }

    /**
     * Check if local storage is enabled
     */
    bool isLocalStorageEnabled() const {
        return _config.mode == StorageMode::LOCAL_ONLY ||
               _config.mode == StorageMode::LOCAL_AND_REMOTE ||
               _config.mode == StorageMode::LOCAL_AUTOSYNC;
    }

    /**
     * Check if remote sync is enabled
     */
    bool isRemoteSyncEnabled() const {
        return _config.mode == StorageMode::REMOTE_ONLY ||
               _config.mode == StorageMode::LOCAL_AND_REMOTE ||
               _config.mode == StorageMode::LOCAL_AUTOSYNC;
    }

    /**
     * Check if auto-sync is enabled
     */
    bool isAutoSyncEnabled() const {
        return _config.mode == StorageMode::LOCAL_AUTOSYNC;
    }

    /**
     * Print current configuration
     */
    void printConfig() const;

private:
    StorageConfig _config;
};

#endif // STORAGE_CONFIG_H
