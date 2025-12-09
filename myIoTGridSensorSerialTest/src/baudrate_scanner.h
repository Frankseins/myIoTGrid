/**
 * myIoTGridSensorSerialTest - Baudrate Scanner
 *
 * Automatic baudrate detection by cycling through common rates
 */

#ifndef BAUDRATE_SCANNER_H
#define BAUDRATE_SCANNER_H

#include <Arduino.h>
#include "config.h"

class BaudrateScanner {
public:
    /**
     * Initialize the scanner
     */
    void begin() {
        _lastByteTime = millis();
        _currentIndex = findBaudrateIndex(DEFAULT_BAUDRATE);
        _timeoutTriggered = false;
    }

    /**
     * Check if timeout has been reached (no data received)
     * @return true if timeout reached and baudrate should change
     */
    bool checkTimeout() {
        if (_timeoutTriggered) {
            return false; // Already handled
        }

        if ((millis() - _lastByteTime) > BAUDRATE_TIMEOUT_MS) {
            _timeoutTriggered = true;
            return true;
        }
        return false;
    }

    /**
     * Reset the timeout timer (call when data is received)
     */
    void resetTimeout() {
        _lastByteTime = millis();
        _timeoutTriggered = false;
    }

    /**
     * Get the next baudrate in the rotation
     * @return next baudrate value
     */
    uint32_t nextBaudrate() {
        _currentIndex = (_currentIndex + 1) % BAUDRATE_COUNT;
        _timeoutTriggered = false;
        _lastByteTime = millis();
        return _baudrateList[_currentIndex];
    }

    /**
     * Get the current baudrate
     * @return current baudrate value
     */
    uint32_t getCurrentBaudrate() const {
        return _baudrateList[_currentIndex];
    }

    /**
     * Get current baudrate index
     * @return index in baudrate list (0-7)
     */
    uint8_t getCurrentIndex() const {
        return _currentIndex;
    }

    /**
     * Get total number of baudrates
     * @return count of supported baudrates
     */
    static uint8_t getBaudrateCount() {
        return BAUDRATE_COUNT;
    }

    /**
     * Get time since last byte received
     * @return milliseconds since last byte
     */
    unsigned long getTimeSinceLastByte() const {
        return millis() - _lastByteTime;
    }

    /**
     * Get timeout duration
     * @return timeout in milliseconds
     */
    static unsigned long getTimeoutDuration() {
        return BAUDRATE_TIMEOUT_MS;
    }

    /**
     * Get progress towards timeout (0-100%)
     * @return percentage of timeout elapsed
     */
    uint8_t getTimeoutProgress() const {
        unsigned long elapsed = millis() - _lastByteTime;
        if (elapsed >= BAUDRATE_TIMEOUT_MS) return 100;
        return (uint8_t)((elapsed * 100) / BAUDRATE_TIMEOUT_MS);
    }

    /**
     * Get baudrate by index
     * @param index baudrate index
     * @return baudrate value or 0 if invalid index
     */
    uint32_t getBaudrateByIndex(uint8_t index) const {
        if (index >= BAUDRATE_COUNT) return 0;
        return _baudrateList[index];
    }

private:
    static const uint8_t BAUDRATE_COUNT = 8;

    const uint32_t _baudrateList[BAUDRATE_COUNT] = {
        1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
    };

    unsigned long _lastByteTime = 0;
    uint8_t _currentIndex = 3;  // Start at 9600 (index 3)
    bool _timeoutTriggered = false;

    /**
     * Find index of a specific baudrate
     * @param baudrate baudrate to find
     * @return index or 3 (9600) if not found
     */
    uint8_t findBaudrateIndex(uint32_t baudrate) {
        for (uint8_t i = 0; i < BAUDRATE_COUNT; i++) {
            if (_baudrateList[i] == baudrate) {
                return i;
            }
        }
        return 3; // Default to 9600
    }
};

#endif // BAUDRATE_SCANNER_H
