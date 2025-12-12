/**
 * DS18B20 OneWire Temperature Sensor Handler
 *
 * For waterproof DS18B20 sensors
 * Wiring:
 *   - Red:    VCC (3.3V or 5V)
 *   - Black:  GND
 *   - Yellow: Data -> GPIO15 (with 4.7k pullup to VCC)
 */

#ifndef DS18B20_HANDLER_H
#define DS18B20_HANDLER_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

class DS18B20Handler {
public:
    void begin() {
        _oneWire = new OneWire(ONEWIRE_PIN);
        _sensors = new DallasTemperature(_oneWire);

        _sensors->begin();

        // Get device count
        _deviceCount = _sensors->getDeviceCount();

        // Set resolution for all devices
        _sensors->setResolution(TEMP_RESOLUTION);

        // Use async mode for non-blocking reads
        _sensors->setWaitForConversion(false);

        Serial.printf("[DS18B20] Found %d sensor(s) on GPIO%d\n", _deviceCount, ONEWIRE_PIN);

        // Print addresses of all found sensors
        for (uint8_t i = 0; i < _deviceCount; i++) {
            DeviceAddress addr;
            if (_sensors->getAddress(addr, i)) {
                Serial.printf("[DS18B20] Sensor %d: ", i);
                printAddress(addr);
                Serial.println();
            }
        }
    }

    uint8_t getDeviceCount() const {
        return _deviceCount;
    }

    void requestTemperatures() {
        _sensors->requestTemperatures();
        _conversionStarted = millis();
        _conversionPending = true;
    }

    bool isConversionComplete() const {
        if (!_conversionPending) return true;

        // Check if enough time has passed for conversion
        // 12-bit resolution needs ~750ms
        unsigned long conversionTime = 750 / (1 << (12 - TEMP_RESOLUTION));
        return (millis() - _conversionStarted) >= conversionTime;
    }

    float getTemperatureC(uint8_t index = 0) {
        _conversionPending = false;
        return _sensors->getTempCByIndex(index);
    }

    float getTemperatureF(uint8_t index = 0) {
        _conversionPending = false;
        return _sensors->getTempFByIndex(index);
    }

    bool isValidReading(float temp) const {
        // DS18B20 returns -127 on error, valid range is -55 to +125
        return temp > -55.0 && temp < 125.0;
    }

    void printAddress(DeviceAddress addr) {
        for (uint8_t i = 0; i < 8; i++) {
            if (addr[i] < 16) Serial.print("0");
            Serial.print(addr[i], HEX);
        }
    }

    // Get address of sensor at index
    bool getAddress(DeviceAddress addr, uint8_t index) {
        return _sensors->getAddress(addr, index);
    }

private:
    OneWire* _oneWire = nullptr;
    DallasTemperature* _sensors = nullptr;
    uint8_t _deviceCount = 0;
    unsigned long _conversionStarted = 0;
    bool _conversionPending = false;
};

#endif // DS18B20_HANDLER_H
