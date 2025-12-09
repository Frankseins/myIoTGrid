/**
 * myIoTGrid.Sensor - Sync Manager Implementation
 */

#include "sync_manager.h"
#include "api_client.h"
#include "wifi_manager.h"

SyncManager::SyncManager()
    : _storage(nullptr)
    , _configManager(nullptr)
    , _apiClient(nullptr)
    , _wifiManager(nullptr)
    , _state(SyncState::IDLE)
    , _retryCount(0)
    , _currentRetryDelay(0)
    , _nextRetryTime(0)
    , _lastSyncAttempt(0)
    , _lastScheduledSync(0)
    , _forceSyncAll(false)
    , _wasWifiConnected(false)
{
    _lastResult.success = false;
    _lastResult.syncedCount = 0;
    _lastResult.failedCount = 0;
}

bool SyncManager::init(ReadingStorage& storage,
                       StorageConfigManager& configManager,
                       ApiClient& apiClient,
                       WiFiManager& wifiManager) {
    _storage = &storage;
    _configManager = &configManager;
    _apiClient = &apiClient;
    _wifiManager = &wifiManager;

    _wasWifiConnected = isWifiAvailable();

    Serial.println("[SyncManager] Initialized");
    Serial.printf("[SyncManager] Mode: %s\n",
                  StorageConfig::getModeString(_configManager->getMode()));
    Serial.printf("[SyncManager] Strategy: %s\n",
                  StorageConfig::getSyncStrategyString(
                      _configManager->getConfig().syncStrategy));

    return true;
}

void SyncManager::loop() {
    if (!_storage || !_configManager) return;

    // Check if sync is enabled
    if (!_configManager->isRemoteSyncEnabled()) {
        return;
    }

    // Check WiFi state changes
    bool wifiConnected = isWifiAvailable();

    // WiFi just connected - trigger sync if auto-sync enabled
    if (wifiConnected && !_wasWifiConnected) {
        Serial.println("[SyncManager] WiFi connected - checking for pending data");
        if (_configManager->isAutoSyncEnabled() && hasPendingReadings()) {
            Serial.println("[SyncManager] Triggering auto-sync");
            triggerSync(false);
        }
    }
    _wasWifiConnected = wifiConnected;

    // Process based on current state
    switch (_state) {
        case SyncState::IDLE:
            // Check if we should sync
            if (shouldSync()) {
                _state = SyncState::SYNCING;
            }
            break;

        case SyncState::WAITING:
            // Waiting for retry
            if (millis() >= _nextRetryTime) {
                if (isWifiAvailable()) {
                    _state = SyncState::SYNCING;
                } else {
                    // Still no WiFi, wait more
                    _nextRetryTime = millis() + 5000; // Check again in 5s
                }
            }
            break;

        case SyncState::SYNCING:
            // Perform sync
            if (isWifiAvailable()) {
                if (_onSyncStart) _onSyncStart();

                _lastResult = performSync();
                _lastSyncAttempt = millis();

                if (_lastResult.success) {
                    handleSyncSuccess(_lastResult.syncedCount);
                } else {
                    handleSyncError(_lastResult.error);
                }
            } else {
                // No WiFi, go to waiting state
                _state = SyncState::WAITING;
                _nextRetryTime = millis() + 5000;
            }
            break;

        case SyncState::ERROR:
            // Wait for retry or manual trigger
            if (millis() >= _nextRetryTime) {
                if (isWifiAvailable() && hasPendingReadings()) {
                    _state = SyncState::SYNCING;
                } else {
                    _state = SyncState::IDLE;
                }
            }
            break;
    }
}

void SyncManager::triggerSync(bool forceAll) {
    Serial.printf("[SyncManager] Manual sync triggered (forceAll=%s)\n",
                  forceAll ? "true" : "false");
    _forceSyncAll = forceAll;
    _state = SyncState::SYNCING;
}

const char* SyncManager::getStateString() const {
    switch (_state) {
        case SyncState::IDLE: return "IDLE";
        case SyncState::WAITING: return "WAITING";
        case SyncState::SYNCING: return "SYNCING";
        case SyncState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

unsigned long SyncManager::getPendingCount() const {
    if (!_storage) return 0;
    return _storage->getPendingCount();
}

bool SyncManager::hasPendingReadings() const {
    if (!_storage) return false;
    return _storage->hasPendingReadings();
}

void SyncManager::resetRetries() {
    _retryCount = 0;
    _currentRetryDelay = 0;
    _nextRetryTime = 0;
    if (_state == SyncState::ERROR || _state == SyncState::WAITING) {
        _state = SyncState::IDLE;
    }
}

bool SyncManager::isWifiAvailable() const {
    if (!_wifiManager) return false;
    return _wifiManager->isConnected();
}

SyncResult SyncManager::performSync() {
    SyncResult result;
    result.success = false;
    result.syncedCount = 0;
    result.failedCount = 0;

    if (!_storage || !_apiClient) {
        result.error = "Not initialized";
        return result;
    }

    if (!isWifiAvailable()) {
        result.error = "WiFi not available";
        return result;
    }

    // Get pending readings
    int batchSize = _forceSyncAll ? 1000 : _configManager->getConfig().batchSize;
    std::vector<StoredReading> pendingReadings = _storage->getPendingReadings(batchSize);

    if (pendingReadings.empty()) {
        result.success = true;
        result.error = "No pending readings";
        Serial.println("[SyncManager] No pending readings to sync");
        return result;
    }

    Serial.printf("[SyncManager] Syncing %d readings...\n", pendingReadings.size());

    // Send batch
    result = sendBatch(pendingReadings);

    // Reset force flag
    _forceSyncAll = false;

    return result;
}

SyncResult SyncManager::sendBatch(const std::vector<StoredReading>& readings) {
    SyncResult result;
    result.success = false;
    result.syncedCount = 0;
    result.failedCount = 0;

    if (!_apiClient || !_apiClient->isConfigured()) {
        result.error = "API client not configured";
        return result;
    }

    // Send each reading
    // TODO: Implement batch API endpoint for efficiency
    int successCount = 0;
    int failCount = 0;

    for (size_t i = 0; i < readings.size(); i++) {
        const auto& reading = readings[i];

        // Report progress
        if (_onSyncProgress) {
            _onSyncProgress(i + 1, readings.size());
        }

        // Send reading
        bool sent = _apiClient->sendReading(
            reading.sensorType,
            reading.value,
            reading.unit,
            reading.endpointId
        );

        if (sent) {
            successCount++;
        } else {
            failCount++;
            // Stop on first failure for now
            // Could implement partial sync later
            if (failCount >= 3) {
                result.error = "Multiple send failures";
                break;
            }
        }

        // Small delay between requests to avoid overwhelming the server
        delay(50);
    }

    result.syncedCount = successCount;
    result.failedCount = failCount;
    result.success = (failCount == 0 || successCount > 0);

    if (result.success && successCount > 0) {
        // Mark readings as synced
        std::vector<StoredReading> syncedReadings(readings.begin(),
                                                   readings.begin() + successCount);
        _storage->markAsSynced(syncedReadings);
    }

    Serial.printf("[SyncManager] Batch result: %d synced, %d failed\n",
                  successCount, failCount);

    return result;
}

unsigned long SyncManager::calculateRetryDelay() {
    const StorageConfig& config = _configManager->getConfig();

    // Exponential backoff
    unsigned long delay = config.initialRetryDelayMs * (1 << _retryCount);

    // Cap at max delay
    if (delay > config.maxRetryDelayMs) {
        delay = config.maxRetryDelayMs;
    }

    return delay;
}

bool SyncManager::shouldSync() const {
    if (!_configManager->isRemoteSyncEnabled()) {
        return false;
    }

    if (!hasPendingReadings()) {
        return false;
    }

    if (!isWifiAvailable()) {
        return false;
    }

    const StorageConfig& config = _configManager->getConfig();

    switch (config.syncStrategy) {
        case SyncStrategy::IMMEDIATE:
            // Sync whenever there's pending data and WiFi
            return true;

        case SyncStrategy::BATCH:
            // Sync when batch size reached
            return getPendingCount() >= (unsigned long)config.batchSize;

        case SyncStrategy::SCHEDULED:
            // Sync at intervals
            return (millis() - _lastScheduledSync) >= config.syncIntervalMs;

        case SyncStrategy::MANUAL:
            // Only sync on manual trigger (handled separately)
            return false;

        default:
            return false;
    }
}

void SyncManager::handleSyncError(const String& error) {
    _retryCount++;
    _currentRetryDelay = calculateRetryDelay();
    _nextRetryTime = millis() + _currentRetryDelay;

    const StorageConfig& config = _configManager->getConfig();

    if (_retryCount >= config.maxRetries) {
        Serial.printf("[SyncManager] Max retries (%d) reached, giving up\n",
                      config.maxRetries);
        _state = SyncState::ERROR;
        _storage->recordSyncFailure(error);
    } else {
        Serial.printf("[SyncManager] Sync failed, retry %d/%d in %lu ms\n",
                      _retryCount, config.maxRetries, _currentRetryDelay);
        _state = SyncState::WAITING;
    }

    if (_onSyncError) {
        _onSyncError(error);
    }

    if (_onSyncComplete) {
        _onSyncComplete(_lastResult);
    }
}

void SyncManager::handleSyncSuccess(int syncedCount) {
    _retryCount = 0;
    _currentRetryDelay = 0;
    _lastScheduledSync = millis();

    _storage->recordSyncSuccess(syncedCount);

    // Check if more data to sync
    if (hasPendingReadings() && isWifiAvailable()) {
        // Continue syncing
        Serial.println("[SyncManager] More pending data, continuing sync...");
        // Stay in SYNCING state
    } else {
        _state = SyncState::IDLE;
    }

    if (_onSyncComplete) {
        _onSyncComplete(_lastResult);
    }
}
