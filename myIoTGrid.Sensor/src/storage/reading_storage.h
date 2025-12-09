/**
 * myIoTGrid.Sensor - Reading Storage
 *
 * Local storage of sensor readings on SD card.
 * Part of Sprint OS-01: Offline-Speicher Implementation
 */

#ifndef READING_STORAGE_H
#define READING_STORAGE_H

#include <Arduino.h>
#include <vector>
#include "sd_manager.h"
#include "storage_config.h"

/**
 * Stored Reading - Single sensor reading with sync status
 */
struct StoredReading {
    unsigned long timestamp;    // Unix timestamp
    String sensorType;          // e.g., "temperature", "humidity"
    double value;               // Sensor value
    String unit;                // Unit of measurement
    int endpointId;             // Endpoint ID from Hub
    bool synced;                // Has been synced to server

    /**
     * Convert to CSV line
     */
    String toCsv() const {
        char buf[256];
        snprintf(buf, sizeof(buf), "%lu,%s,%.4f,%s,%d,%d",
                 timestamp, sensorType.c_str(), value,
                 unit.c_str(), endpointId, synced ? 1 : 0);
        return String(buf);
    }

    /**
     * Parse from CSV line
     */
    static StoredReading fromCsv(const String& line) {
        StoredReading reading;
        reading.timestamp = 0;
        reading.value = 0;
        reading.endpointId = 0;
        reading.synced = false;

        int idx1 = line.indexOf(',');
        if (idx1 < 0) return reading;
        reading.timestamp = line.substring(0, idx1).toInt();

        int idx2 = line.indexOf(',', idx1 + 1);
        if (idx2 < 0) return reading;
        reading.sensorType = line.substring(idx1 + 1, idx2);

        int idx3 = line.indexOf(',', idx2 + 1);
        if (idx3 < 0) return reading;
        reading.value = line.substring(idx2 + 1, idx3).toDouble();

        int idx4 = line.indexOf(',', idx3 + 1);
        if (idx4 < 0) return reading;
        reading.unit = line.substring(idx3 + 1, idx4);

        int idx5 = line.indexOf(',', idx4 + 1);
        if (idx5 < 0) return reading;
        reading.endpointId = line.substring(idx4 + 1, idx5).toInt();

        reading.synced = (line.substring(idx5 + 1).toInt() == 1);

        return reading;
    }
};

/**
 * Sync Status - Overall sync statistics
 */
struct SyncStatus {
    unsigned long totalReadings = 0;
    unsigned long syncedReadings = 0;
    unsigned long pendingReadings = 0;
    unsigned long lastSyncTimestamp = 0;
    unsigned long lastReadingTimestamp = 0;
    int consecutiveFailures = 0;
    String lastError;

    /**
     * Get pending count
     */
    unsigned long getPendingCount() const {
        return totalReadings - syncedReadings;
    }

    /**
     * Check if all synced
     */
    bool isFullySynced() const {
        return pendingReadings == 0;
    }
};

/**
 * Reading Storage - Manages local reading storage on SD card
 */
class ReadingStorage {
public:
    ReadingStorage();

    /**
     * Initialize storage
     * @param sdManager reference to SD manager
     * @param configManager reference to config manager
     */
    bool init(SDManager& sdManager, StorageConfigManager& configManager);

    /**
     * Store a reading locally
     * @param reading the reading to store
     * @return true if stored successfully
     */
    bool storeReading(const StoredReading& reading);

    /**
     * Store a reading from sensor data
     * @param sensorType sensor type code
     * @param value reading value
     * @param unit unit of measurement
     * @param endpointId endpoint ID
     * @return true if stored successfully
     */
    bool storeReading(const String& sensorType, double value,
                      const String& unit, int endpointId);

    /**
     * Get pending readings for sync (oldest first)
     * @param maxCount maximum number to return
     * @return vector of pending readings
     */
    std::vector<StoredReading> getPendingReadings(int maxCount = 50);

    /**
     * Mark readings as synced
     * @param readings readings to mark
     * @return number marked as synced
     */
    int markAsSynced(const std::vector<StoredReading>& readings);

    /**
     * Get sync status
     */
    SyncStatus getSyncStatus() const { return _syncStatus; }

    /**
     * Get pending count
     */
    unsigned long getPendingCount() const { return _syncStatus.pendingReadings; }

    /**
     * Check if there are pending readings
     */
    bool hasPendingReadings() const { return _syncStatus.pendingReadings > 0; }

    /**
     * Record sync failure
     * @param error error message
     */
    void recordSyncFailure(const String& error);

    /**
     * Record sync success
     * @param syncedCount number synced
     */
    void recordSyncSuccess(int syncedCount);

    /**
     * Save sync status to SD card
     */
    bool saveSyncStatus();

    /**
     * Load sync status from SD card
     */
    bool loadSyncStatus();

    /**
     * Create a pending batch file for sync
     * @param readings readings to include in batch
     * @return batch filename or empty string on error
     */
    String createPendingBatch(const std::vector<StoredReading>& readings);

    /**
     * Delete a pending batch file (after successful sync)
     * @param batchFile batch filename
     */
    bool deletePendingBatch(const String& batchFile);

    /**
     * Get list of pending batch files
     */
    std::vector<String> getPendingBatchFiles();

    /**
     * Read readings from a batch file
     * @param batchFile batch filename
     */
    std::vector<StoredReading> readBatchFile(const String& batchFile);

    /**
     * Update pending count by scanning files
     */
    void updatePendingCount();

    /**
     * Get today's filename
     */
    String getTodayFilename() const;

private:
    SDManager* _sdManager;
    StorageConfigManager* _configManager;
    SyncStatus _syncStatus;
    String _currentDayFile;
    unsigned long _lastFlush;

    static const unsigned long FLUSH_INTERVAL_MS = 10000; // 10 seconds

    /**
     * Get filename for a specific date
     */
    String getFilenameForDate(int year, int month, int day) const;

    /**
     * Ensure current day's file is open
     */
    bool ensureDayFile();

    /**
     * Parse date from filename
     */
    bool parseDateFromFilename(const String& filename, int& year, int& month, int& day);
};

#endif // READING_STORAGE_H
