/**
 * myIoTGrid.Sensor - Hardware Validator Implementation
 * Sprint 8: Remote Debug System
 */

#include "hardware_validator.h"

#ifdef PLATFORM_ESP32
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

// Static member initialization
uint32_t HardwareValidator::crc32Table[256];
bool HardwareValidator::crc32TableInitialized = false;

// Singleton instance
HardwareValidator& HardwareValidator::getInstance() {
    static HardwareValidator instance;
    return instance;
}

HardwareValidator::HardwareValidator()
    : _initialValidationDone(false)
    , _lastConfigHash(0) {
}

void HardwareValidator::begin() {
    initCRC32Table();
    DBG_SYSTEM("HardwareValidator initialized");
}

bool HardwareValidator::shouldValidate(const NodeConfigurationResponse& config, bool force) {
    if (force) {
        DBG_HARDWARE("Validation forced");
        return true;
    }

    // Always validate on first run
    if (!_initialValidationDone) {
        DBG_HARDWARE("Initial validation required");
        return true;
    }

    // Check if config changed
    uint32_t newHash = calculateConfigHash(config);
    if (newHash != _lastConfigHash) {
        DBG_HARDWARE("Config hash changed: 0x%08X -> 0x%08X", _lastConfigHash, newHash);
        return true;
    }

    // In DEBUG mode, always validate
    if (DebugManager::getInstance().getLevel() == DebugLevel::DEBUG) {
        DBG_HARDWARE("DEBUG mode - validation enabled");
        return true;
    }

    DBG_HARDWARE("Skipping validation (cached, hash: 0x%08X)", _lastConfigHash);
    return false;
}

HardwareValidationResult HardwareValidator::validate(const NodeConfigurationResponse& config, bool force) {
    HardwareValidationResult result;
    result.overallValid = true;
    result.validatedAt = millis();
    result.validCount = 0;
    result.invalidCount = 0;
    result.skippedCount = 0;

    // Calculate config hash
    result.configHash = calculateConfigHash(config);

    DBG_SYSTEM("Starting hardware validation for %d sensors", (int)config.sensors.size());

    for (const auto& sensor : config.sensors) {
        if (!sensor.isActive) {
            result.skippedCount++;
            continue;
        }

        SensorValidationResult sensorResult = validateSensor(sensor);
        result.sensors.push_back(sensorResult);

        if (sensorResult.isValid) {
            result.validCount++;
            DBG_HARDWARE("  [OK] %s (Endpoint %d): %s",
                        sensor.sensorCode.c_str(),
                        sensor.endpointId,
                        sensorResult.status.c_str());
        } else {
            result.invalidCount++;
            result.overallValid = false;
            DBG_ERROR("  [FAIL] %s (Endpoint %d): %s",
                     sensor.sensorCode.c_str(),
                     sensor.endpointId,
                     sensorResult.status.c_str());
        }
    }

    DBG_SYSTEM("Validation complete: %d valid, %d invalid, %d skipped",
              result.validCount, result.invalidCount, result.skippedCount);

    // Update cache
    _lastConfigHash = result.configHash;
    _lastResult = result;
    _initialValidationDone = true;

    return result;
}

SensorValidationResult HardwareValidator::validateSensor(const SensorAssignmentConfig& sensor) {
    SensorValidationResult result;
    result.endpointId = sensor.endpointId;
    result.sensorCode = sensor.sensorCode;
    result.isValid = false;
    result.status = "UNKNOWN";
    result.i2cAddress = -1;

    // Determine sensor connection type and validate accordingly
    if (sensor.i2cAddress.length() > 0 && sensor.i2cAddress != "0" && sensor.i2cAddress != "0x00") {
        validateI2CSensor(sensor, result);
    } else if (sensor.oneWirePin >= 0) {
        validateOneWireSensor(sensor, result);
    } else if (sensor.analogPin >= 0) {
        validateAnalogSensor(sensor, result);
    } else if (sensor.digitalPin >= 0) {
        validateDigitalSensor(sensor, result);
    } else if (sensor.baudRate > 0) {
        validateUARTSensor(sensor, result);
    } else {
        // Simulated or unknown sensor type
        result.isValid = true;
        result.status = "SIMULATED";
        result.detectedType = sensor.sensorCode;
    }

    return result;
}

bool HardwareValidator::validateI2CSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result) {
#ifdef PLATFORM_ESP32
    // Parse I2C address
    uint8_t addr = 0;
    if (sensor.i2cAddress.startsWith("0x") || sensor.i2cAddress.startsWith("0X")) {
        addr = (uint8_t)strtol(sensor.i2cAddress.c_str(), nullptr, 16);
    } else {
        addr = (uint8_t)sensor.i2cAddress.toInt();
    }

    result.i2cAddress = addr;

    int sdaPin = sensor.sdaPin >= 0 ? sensor.sdaPin : 21;
    int sclPin = sensor.sclPin >= 0 ? sensor.sclPin : 22;

    DBG_DEBUG(LogCategory::HARDWARE, "Scanning I2C addr 0x%02X on SDA=%d SCL=%d",
             addr, sdaPin, sclPin);

    if (scanI2CAddress(addr, sdaPin, sclPin)) {
        result.isValid = true;
        result.status = "OK";
        result.detectedType = sensor.sensorCode;
        return true;
    } else {
        result.status = "NOT_FOUND";
        return false;
    }
#else
    // Simulation mode - always valid
    result.isValid = true;
    result.status = "SIMULATED";
    result.detectedType = sensor.sensorCode;
    return true;
#endif
}

bool HardwareValidator::validateOneWireSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result) {
#ifdef PLATFORM_ESP32
    // Check if pin is valid first
    if (sensor.oneWirePin < 0 || sensor.oneWirePin > 39) {
        result.status = "INVALID_PIN";
        return false;
    }

    // Actually scan the OneWire bus to verify DS18B20 presence
    DBG_DEBUG(LogCategory::HARDWARE, "Scanning OneWire on pin %d for DS18B20...", sensor.oneWirePin);

    OneWire oneWire(sensor.oneWirePin);
    DallasTemperature sensors(&oneWire);

    // Give the bus time to stabilize after creating the OneWire instance
    delay(10);

    sensors.begin();

    int deviceCount = sensors.getDeviceCount();
    DBG_DEBUG(LogCategory::HARDWARE, "OneWire pin %d: %d device(s) found", sensor.oneWirePin, deviceCount);

    if (deviceCount > 0) {
        // Verify we can get a valid address (family code 0x28 = DS18B20)
        DeviceAddress addr;
        if (sensors.getAddress(addr, 0)) {
            if (addr[0] == 0x28 || addr[0] == 0x10 || addr[0] == 0x22 || addr[0] == 0x3B) {
                result.isValid = true;
                result.status = "OK";
                result.detectedType = sensor.sensorCode;
                DBG_DEBUG(LogCategory::HARDWARE, "DS18B20 confirmed on pin %d (family: 0x%02X)", sensor.oneWirePin, addr[0]);
                return true;
            }
        }
    }

    // Device not found on this pin
    result.status = "NOT_FOUND";
    DBG_DEBUG(LogCategory::HARDWARE, "No DS18B20 found on pin %d", sensor.oneWirePin);
    return false;
#else
    result.isValid = true;
    result.status = "SIMULATED";
    result.detectedType = sensor.sensorCode;
    return true;
#endif
}

bool HardwareValidator::validateAnalogSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result) {
#ifdef PLATFORM_ESP32
    // Check if valid ADC pin on ESP32
    const int validAdcPins[] = {32, 33, 34, 35, 36, 39, 25, 26, 27, 14, 12, 13, 15, 2, 4};
    bool validPin = false;

    for (int pin : validAdcPins) {
        if (sensor.analogPin == pin) {
            validPin = true;
            break;
        }
    }

    if (validPin) {
        result.isValid = true;
        result.status = "OK";
        result.detectedType = sensor.sensorCode;
        return true;
    }

    result.status = "INVALID_ADC_PIN";
    return false;
#else
    result.isValid = true;
    result.status = "SIMULATED";
    result.detectedType = sensor.sensorCode;
    return true;
#endif
}

bool HardwareValidator::validateDigitalSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result) {
#ifdef PLATFORM_ESP32
    // Most GPIO pins are valid for digital
    if (sensor.digitalPin >= 0 && sensor.digitalPin <= 39) {
        // Avoid strapping pins and flash pins
        const int invalidPins[] = {0, 6, 7, 8, 9, 10, 11};
        for (int pin : invalidPins) {
            if (sensor.digitalPin == pin) {
                result.status = "RESERVED_PIN";
                return false;
            }
        }

        result.isValid = true;
        result.status = "OK";
        result.detectedType = sensor.sensorCode;
        return true;
    }

    result.status = "INVALID_PIN";
    return false;
#else
    result.isValid = true;
    result.status = "SIMULATED";
    result.detectedType = sensor.sensorCode;
    return true;
#endif
}

bool HardwareValidator::validateUARTSensor(const SensorAssignmentConfig& sensor, SensorValidationResult& result) {
#ifdef PLATFORM_ESP32
    // UART validation is complex - just validate baud rate is reasonable
    const int validBaudRates[] = {9600, 19200, 38400, 57600, 115200};
    bool validBaud = false;

    for (int baud : validBaudRates) {
        if (sensor.baudRate == baud) {
            validBaud = true;
            break;
        }
    }

    if (validBaud) {
        result.isValid = true;
        result.status = "BAUD_VALID";
        result.detectedType = sensor.sensorCode;
        return true;
    }

    result.status = "INVALID_BAUD";
    return false;
#else
    result.isValid = true;
    result.status = "SIMULATED";
    result.detectedType = sensor.sensorCode;
    return true;
#endif
}

bool HardwareValidator::scanI2CAddress(uint8_t address, int sdaPin, int sclPin) {
#ifdef PLATFORM_ESP32
    Wire.begin(sdaPin, sclPin);
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    return (error == 0);
#else
    return true;  // Simulation
#endif
}

void HardwareValidator::invalidateCache() {
    _lastConfigHash = 0;
    _initialValidationDone = false;
    DBG_HARDWARE("Validation cache invalidated");
}

uint32_t HardwareValidator::calculateConfigHash(const NodeConfigurationResponse& config) {
    initCRC32Table();

    uint32_t crc = 0xFFFFFFFF;

    // Hash node ID
    crc = updateCRC32(crc, (const uint8_t*)config.nodeId.c_str(), config.nodeId.length());

    // Hash each sensor configuration
    for (const auto& sensor : config.sensors) {
        crc = updateCRC32(crc, (const uint8_t*)&sensor.endpointId, sizeof(sensor.endpointId));
        crc = updateCRC32(crc, (const uint8_t*)sensor.sensorCode.c_str(), sensor.sensorCode.length());
        crc = updateCRC32(crc, (const uint8_t*)sensor.i2cAddress.c_str(), sensor.i2cAddress.length());
        crc = updateCRC32(crc, (const uint8_t*)&sensor.sdaPin, sizeof(sensor.sdaPin));
        crc = updateCRC32(crc, (const uint8_t*)&sensor.sclPin, sizeof(sensor.sclPin));
        crc = updateCRC32(crc, (const uint8_t*)&sensor.oneWirePin, sizeof(sensor.oneWirePin));
        crc = updateCRC32(crc, (const uint8_t*)&sensor.analogPin, sizeof(sensor.analogPin));
        crc = updateCRC32(crc, (const uint8_t*)&sensor.digitalPin, sizeof(sensor.digitalPin));
        crc = updateCRC32(crc, (const uint8_t*)&sensor.baudRate, sizeof(sensor.baudRate));
        crc = updateCRC32(crc, (const uint8_t*)&sensor.isActive, sizeof(sensor.isActive));
    }

    return crc ^ 0xFFFFFFFF;
}

void HardwareValidator::initCRC32Table() {
    if (crc32TableInitialized) return;

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        crc32Table[i] = crc;
    }

    crc32TableInitialized = true;
}

uint32_t HardwareValidator::updateCRC32(uint32_t crc, const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        crc = crc32Table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}
