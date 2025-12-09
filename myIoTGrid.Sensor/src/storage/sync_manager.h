/**
 * myIoTGrid.Sensor - Sync Manager
 *
 * Manages automatic synchronization of local readings to the Hub API.
 * Part of Sprint OS-01: Offline-Speicher Implementation
 */

#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#include <Arduino.h>
#include <functional>
#include "reading_storage.h"
#include "storage_config.h"

// Forward declarations
class ApiClient;
class WiFiManager;

/**
 * Sync State
 */
enum class SyncState {
    IDLE,           // Not syncing
    WAITING,        // Waiting for WiFi or retry delay
    SYNCING,        // Currently syncing
    ERROR           // Error occurred, will retry
};

/**
 * Sync Result
 */
struct SyncResult {
    bool success;
    int syncedCount;
    int failedCount;
    String error;
};

/**
 * Sync callbacks
 */
using OnSyncStartCallback = std::function<void()>;
using OnSyncProgressCallback = std::function<void(int synced, int total)>;
using OnSyncCompleteCallback = std::function<void(const SyncResult& result)>;
using OnSyncErrorCallback = std::function<void(const String& error)>;

/**
 * Sync Manager - Handles automatic data synchronization
 */
class SyncManager {
public:
    SyncManager();

    /**
     * Initialize the sync manager
     * @param storage reference to reading storage
     * @param configManager reference to config manager
     * @param apiClient reference to API client
     * @param wifiManager reference to WiFi manager
     */
    bool init(ReadingStorage& storage,
              StorageConfigManager& configManager,
              ApiClient& apiClient,
              WiFiManager& wifiManager);

    /**
     * Process sync operations (call in loop)
     */
    void loop();

    /**
     * Trigger immediate sync
     * @param forceAll sync all data, not just batch size
     */
    void triggerSync(bool forceAll = false);

    /**
     * Check if sync is currently in progress
     */
    bool isSyncing() const { return _state == SyncState::SYNCING; }

    /**
     * Get current sync state
     */
    SyncState getState() const { return _state; }

    /**
     * Get state as string
     */
    const char* getStateString() const;

    /**
     * Get pending count
     */
    unsigned long getPendingCount() const;

    /**
     * Check if there are pending readings
     */
    bool hasPendingReadings() const;

    /**
     * Get last sync result
     */
    const SyncResult& getLastResult() const { return _lastResult; }

    /**
     * Get next retry time (0 if not waiting)
     */
    unsigned long getNextRetryTime() const { return _nextRetryTime; }

    /**
     * Get current retry delay
     */
    unsigned long getCurrentRetryDelay() const { return _currentRetryDelay; }

    /**
     * Reset retry counter (e.g., after manual intervention)
     */
    void resetRetries();

    /**
     * Set callbacks
     */
    void onSyncStart(OnSyncStartCallback callback) { _onSyncStart = callback; }
    void onSyncProgress(OnSyncProgressCallback callback) { _onSyncProgress = callback; }
    void onSyncComplete(OnSyncCompleteCallback callback) { _onSyncComplete = callback; }
    void onSyncError(OnSyncErrorCallback callback) { _onSyncError = callback; }

    /**
     * Check if WiFi is available for sync
     */
    bool isWifiAvailable() const;

private:
    ReadingStorage* _storage;
    StorageConfigManager* _configManager;
    ApiClient* _apiClient;
    WiFiManager* _wifiManager;

    SyncState _state;
    SyncResult _lastResult;

    // Retry management
    int _retryCount;
    unsigned long _currentRetryDelay;
    unsigned long _nextRetryTime;

    // Sync timing
    unsigned long _lastSyncAttempt;
    unsigned long _lastScheduledSync;
    bool _forceSyncAll;

    // Callbacks
    OnSyncStartCallback _onSyncStart;
    OnSyncProgressCallback _onSyncProgress;
    OnSyncCompleteCallback _onSyncComplete;
    OnSyncErrorCallback _onSyncError;

    // WiFi state tracking
    bool _wasWifiConnected;

    /**
     * Perform the actual sync operation
     */
    SyncResult performSync();

    /**
     * Send batch to API
     * @param readings readings to send
     * @return sync result
     */
    SyncResult sendBatch(const std::vector<StoredReading>& readings);

    /**
     * Calculate next retry delay (exponential backoff)
     */
    unsigned long calculateRetryDelay();

    /**
     * Check if sync should be triggered based on strategy
     */
    bool shouldSync() const;

    /**
     * Handle sync error
     */
    void handleSyncError(const String& error);

    /**
     * Handle sync success
     */
    void handleSyncSuccess(int syncedCount);
};

#endif // SYNC_MANAGER_H
