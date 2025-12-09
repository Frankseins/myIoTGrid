/**
 * myIoTGrid.Sensor - Hardware Validator
 * Sprint 8: Remote Debug System
 *
 * Validates hardware only when needed:
 * - On boot
 * - On configuration change (detected via hash)
 * - When in DEBUG mode
 *
 * This reduces I2C/UART scans by ~80% in normal operation.
 */

#ifndef HARDWARE_VALIDATOR_H
#define HARDWARE_VALIDATOR_H

#include <Arduino.h>
#include <vector>
#include "api_client.h"
#include "debug_manager.h"

/**
 * Validation result for a single sensor
 */
struct SensorValidationResult {
    int endpointId;
    String sensorCode;
    bool isValid;
    String status;        // "OK", "NOT_FOUND", "COMM_ERROR", etc.
    String detectedType;  // Actual detected sensor type
    int i2cAddress;       // Detected I2C address (if applicable)
};

/**
 * Full validation result (Sprint 8 version with config hash)
 */
struct HardwareValidationResult {
    bool overallValid;
    uint32_t configHash;
    unsigned long validatedAt;
    std::vector<SensorValidationResult> sensors;
    int validCount;
    int invalidCount;
    int skippedCount;
};

/**
 * HardwareValidator - Smart hardware validation with caching
 */
class HardwareValidator {
public:
    static HardwareValidator& getInstance();

    // Prevent copying
    HardwareValidator(const HardwareValidator&) = delete;
    HardwareValidator& operator=(const HardwareValidator&) = delete;

    /**
     * Initialize validator
     */
    void begin();

    /**
     * Check if validation should run
     * Returns true if:
     * - Initial validation not done
     * - Configuration hash changed
     * - Debug mode is active
     * - Force flag is set
     */
    bool shouldValidate(const NodeConfigurationResponse& config, bool force = false);

    /**
     * Perform hardware validation
     * @param config Current node configuration
     * @param force Skip caching, always validate
     * @return Validation results
     */
    HardwareValidationResult validate(const NodeConfigurationResponse& config, bool force = false);

    /**
     * Get last validation result
     */
    HardwareValidationResult getLastResult() const { return _lastResult; }

    /**
     * Check if initial validation is complete
     */
    bool isInitialValidationDone() const { return _initialValidationDone; }

    /**
     * Get current config hash
     */
    uint32_t getCurrentConfigHash() const { return _lastConfigHash; }

    /**
     * Clear cached validation (forces revalidation)
     */
    void invalidateCache();

    /**
     * Calculate CRC32 hash of configuration
     */
    static uint32_t calculateConfigHash(const NodeConfigurationResponse& config);

private:
    HardwareValidator();

    SensorValidationResult validateSensor(const SensorAssignmentConfig& sensor);
    bool validateI2CSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result);
    bool validateOneWireSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result);
    bool validateAnalogSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result);
    bool validateDigitalSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result);
    bool validateUARTSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result);

    bool scanI2CAddress(uint8_t address, int sdaPin, int sclPin);

    // CRC32 table and calculation
    static uint32_t crc32Table[256];
    static bool crc32TableInitialized;
    static void initCRC32Table();
    static uint32_t updateCRC32(uint32_t crc, const uint8_t* data, size_t length);

    bool _initialValidationDone;
    uint32_t _lastConfigHash;
    HardwareValidationResult _lastResult;
};

#endif // HARDWARE_VALIDATOR_H
