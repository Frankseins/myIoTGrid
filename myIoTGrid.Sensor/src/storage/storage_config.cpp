/**
 * myIoTGrid.Sensor - Storage Configuration Implementation
 */

#include "storage_config.h"
#include "sd_manager.h"
#include <ArduinoJson.h>

StorageConfigManager::StorageConfigManager() {
    // Default configuration is set in struct initialization
}

bool StorageConfigManager::load(SDManager& sdManager) {
    if (!sdManager.isAvailable()) {
        Serial.println("[StorageConfig] SD card not available, using defaults");
        return false;
    }

    String content = sdManager.readFile(SD_CONFIG_FILE);
    if (content.length() == 0) {
        Serial.println("[StorageConfig] No config file found, using defaults");
        // Save default config
        save(sdManager);
        return true;
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, content);

    if (error) {
        Serial.printf("[StorageConfig] JSON parse error: %s\n", error.c_str());
        return false;
    }

    // Read values
    if (doc.containsKey("mode")) {
        const char* modeStr = doc["mode"].as<const char*>();
        if (modeStr) {
            _config.mode = StorageConfig::parseMode(String(modeStr));
        }
    }

    if (doc.containsKey("syncStrategy")) {
        const char* stratStr = doc["syncStrategy"].as<const char*>();
        if (stratStr) {
            _config.syncStrategy = StorageConfig::parseSyncStrategy(String(stratStr));
        }
    }

    if (doc.containsKey("batchSize")) {
        _config.batchSize = doc["batchSize"].as<int>();
    }

    if (doc.containsKey("syncIntervalMs")) {
        _config.syncIntervalMs = doc["syncIntervalMs"].as<unsigned long>();
    }

    if (doc.containsKey("maxRetries")) {
        _config.maxRetries = doc["maxRetries"].as<int>();
    }

    if (doc.containsKey("initialRetryDelayMs")) {
        _config.initialRetryDelayMs = doc["initialRetryDelayMs"].as<unsigned long>();
    }

    if (doc.containsKey("maxRetryDelayMs")) {
        _config.maxRetryDelayMs = doc["maxRetryDelayMs"].as<unsigned long>();
    }

    if (doc.containsKey("autoCleanup")) {
        _config.autoCleanup = doc["autoCleanup"].as<bool>();
    }

    if (doc.containsKey("keepSyncedDays")) {
        _config.keepSyncedDays = doc["keepSyncedDays"].as<int>();
    }

    if (doc.containsKey("minFreeBytes")) {
        _config.minFreeBytes = doc["minFreeBytes"].as<uint64_t>();
    }

    if (doc.containsKey("enableStatusLed")) {
        _config.enableStatusLed = doc["enableStatusLed"].as<bool>();
    }

    if (doc.containsKey("enableSyncButton")) {
        _config.enableSyncButton = doc["enableSyncButton"].as<bool>();
    }

    Serial.println("[StorageConfig] Configuration loaded from SD card");
    printConfig();
    return true;
}

bool StorageConfigManager::save(SDManager& sdManager) {
    if (!sdManager.isAvailable()) {
        Serial.println("[StorageConfig] SD card not available, cannot save");
        return false;
    }

    // Create JSON
    JsonDocument doc;

    doc["mode"] = StorageConfig::getModeString(_config.mode);
    doc["syncStrategy"] = StorageConfig::getSyncStrategyString(_config.syncStrategy);
    doc["batchSize"] = _config.batchSize;
    doc["syncIntervalMs"] = _config.syncIntervalMs;
    doc["maxRetries"] = _config.maxRetries;
    doc["initialRetryDelayMs"] = _config.initialRetryDelayMs;
    doc["maxRetryDelayMs"] = _config.maxRetryDelayMs;
    doc["autoCleanup"] = _config.autoCleanup;
    doc["keepSyncedDays"] = _config.keepSyncedDays;
    doc["minFreeBytes"] = _config.minFreeBytes;
    doc["enableStatusLed"] = _config.enableStatusLed;
    doc["enableSyncButton"] = _config.enableSyncButton;

    // Serialize to string
    String content;
    serializeJsonPretty(doc, content);

    // Write to file
    if (sdManager.writeFile(SD_CONFIG_FILE, content)) {
        Serial.println("[StorageConfig] Configuration saved to SD card");
        return true;
    }

    Serial.println("[StorageConfig] Failed to save configuration");
    return false;
}

void StorageConfigManager::printConfig() const {
    Serial.println("[StorageConfig] Current configuration:");
    Serial.printf("  Mode: %s\n", StorageConfig::getModeString(_config.mode));
    Serial.printf("  Sync Strategy: %s\n",
                  StorageConfig::getSyncStrategyString(_config.syncStrategy));
    Serial.printf("  Batch Size: %d\n", _config.batchSize);
    Serial.printf("  Sync Interval: %lu ms\n", _config.syncIntervalMs);
    Serial.printf("  Max Retries: %d\n", _config.maxRetries);
    Serial.printf("  Auto Cleanup: %s\n", _config.autoCleanup ? "yes" : "no");
    Serial.printf("  Keep Synced Days: %d\n", _config.keepSyncedDays);
    Serial.printf("  Status LED: %s\n", _config.enableStatusLed ? "enabled" : "disabled");
    Serial.printf("  Sync Button: %s\n", _config.enableSyncButton ? "enabled" : "disabled");
}
