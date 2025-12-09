/**
 * myIoTGrid.Sensor - Debug Log Uploader
 * Sprint 8: Remote Debug System
 *
 * Handles batch upload of debug logs to the Hub API.
 */

#ifndef DEBUG_LOG_UPLOADER_H
#define DEBUG_LOG_UPLOADER_H

#include <Arduino.h>
#include <vector>
#include <functional>
#include "debug_manager.h"

/**
 * Upload configuration
 */
struct DebugLogUploaderConfig {
    int maxQueueSize = 200;               // Max entries to buffer
    unsigned long uploadIntervalMs = 10000;  // Upload every 10 seconds (was 60s)
    int maxRetries = 3;
    unsigned long retryDelayMs = 5000;
    int batchSize = 50;                   // Max entries per upload
};

/**
 * Upload statistics
 */
struct DebugLogUploaderStats {
    uint32_t entriesUploaded = 0;
    uint32_t entriesDropped = 0;
    uint32_t uploadAttempts = 0;
    uint32_t uploadFailures = 0;
    unsigned long lastUploadTime = 0;
};

/**
 * DebugLogUploader - Batch upload of logs to Hub
 */
class DebugLogUploader {
public:
    static DebugLogUploader& getInstance();

    // Prevent copying
    DebugLogUploader(const DebugLogUploader&) = delete;
    DebugLogUploader& operator=(const DebugLogUploader&) = delete;

    /**
     * Initialize uploader
     * @param baseUrl Hub API base URL
     * @param serialNumber Node serial number
     */
    void begin(const String& baseUrl, const String& serialNumber);

    /**
     * Configure uploader
     */
    void configure(const DebugLogUploaderConfig& config);

    /**
     * Set API key (if required)
     */
    void setApiKey(const String& apiKey) { _apiKey = apiKey; }

    /**
     * Enable/disable uploading
     */
    void setEnabled(bool enabled) { _enabled = enabled; }
    bool isEnabled() const { return _enabled; }

    /**
     * Queue a log entry for upload
     */
    void queueLog(const LogEntry& entry);

    /**
     * Process queue and upload if needed (call from main loop)
     */
    void loop();

    /**
     * Force immediate upload
     */
    bool uploadNow();

    /**
     * Get queued entry count
     */
    int getQueuedCount() const { return _queue.size(); }

    /**
     * Get statistics
     */
    DebugLogUploaderStats getStats() const { return _stats; }

    /**
     * Clear queue
     */
    void clearQueue();

private:
    DebugLogUploader();

    bool uploadBatch();
    bool uploadSerialLines();
    String buildUploadPayload();

    String _baseUrl;
    String _serialNumber;
    String _apiKey;
    bool _enabled;
    bool _initialized;

    DebugLogUploaderConfig _config;
    DebugLogUploaderStats _stats;

    std::vector<LogEntry> _queue;
    unsigned long _lastUploadTime;
    int _currentRetry;
};

#endif // DEBUG_LOG_UPLOADER_H
