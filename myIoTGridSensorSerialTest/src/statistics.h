/**
 * myIoTGridSensorSerialTest - Statistics
 *
 * Data reception statistics and dashboard display
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include <Arduino.h>
#include "config.h"

class Statistics {
public:
    /**
     * Initialize statistics
     */
    void begin() {
        _totalBytes = 0;
        _lastByteTime = 0;
        _startTime = millis();
        _bytesInLastSecond = 0;
        _lastSecondStart = millis();
        _currentDataRate = 0;
    }

    /**
     * Record a received byte
     * @param data the received byte
     */
    void addByte(uint8_t data) {
        _totalBytes++;
        _lastByteTime = millis();
        _bytesInLastSecond++;

        // Track byte frequency distribution
        _byteHistogram[data]++;
    }

    /**
     * Update statistics (call periodically)
     */
    void update() {
        unsigned long now = millis();
        if (now - _lastSecondStart >= 1000) {
            _currentDataRate = _bytesInLastSecond;
            _bytesInLastSecond = 0;
            _lastSecondStart = now;
        }
    }

    /**
     * Print dashboard to Serial
     * @param currentBaudrate current UART baudrate
     */
    void printDashboard(uint32_t currentBaudrate) {
        Serial.println();
        Serial.println("========================================");
        Serial.println("       UART Serial Monitor Dashboard    ");
        Serial.println("========================================");
        Serial.printf("| Baudrate:     %-22d |\n", currentBaudrate);
        Serial.printf("| Total Bytes:  %-22lu |\n", _totalBytes);
        Serial.printf("| Data Rate:    %-18.1f B/s |\n", (float)_currentDataRate);
        Serial.printf("| Last Byte:    %-18lu ms |\n", getTimeSinceLastByte());
        Serial.printf("| Uptime:       %-18lu s  |\n", getUptimeSeconds());
        Serial.println("========================================");
    }

    /**
     * Print compact status line
     * @param currentBaudrate current UART baudrate
     * @param timeoutProgress percentage of timeout elapsed
     */
    void printStatusLine(uint32_t currentBaudrate, uint8_t timeoutProgress) {
        Serial.printf("[%6d baud] Bytes: %lu | Rate: %d B/s | Timeout: %d%%\r",
                     currentBaudrate, _totalBytes, _currentDataRate, timeoutProgress);
    }

    /**
     * Get total bytes received
     * @return total byte count
     */
    unsigned long getTotalBytes() const {
        return _totalBytes;
    }

    /**
     * Get current data rate (bytes per second)
     * @return bytes per second
     */
    unsigned long getDataRate() const {
        return _currentDataRate;
    }

    /**
     * Get average data rate since start
     * @return average bytes per second
     */
    float getAverageDataRate() const {
        unsigned long elapsed = millis() - _startTime;
        if (elapsed == 0) return 0;
        return (_totalBytes * 1000.0f) / elapsed;
    }

    /**
     * Get time since last byte received
     * @return milliseconds since last byte
     */
    unsigned long getTimeSinceLastByte() const {
        if (_lastByteTime == 0) return 0;
        return millis() - _lastByteTime;
    }

    /**
     * Get uptime in seconds
     * @return seconds since start
     */
    unsigned long getUptimeSeconds() const {
        return (millis() - _startTime) / 1000;
    }

    /**
     * Check if any data has been received
     * @return true if at least one byte received
     */
    bool hasReceivedData() const {
        return _totalBytes > 0;
    }

    /**
     * Get most common byte value (for protocol analysis)
     * @return most frequently received byte value
     */
    uint8_t getMostCommonByte() const {
        uint8_t mostCommon = 0;
        uint32_t maxCount = 0;
        for (int i = 0; i < 256; i++) {
            if (_byteHistogram[i] > maxCount) {
                maxCount = _byteHistogram[i];
                mostCommon = (uint8_t)i;
            }
        }
        return mostCommon;
    }

private:
    unsigned long _totalBytes = 0;
    unsigned long _lastByteTime = 0;
    unsigned long _startTime = 0;
    unsigned long _bytesInLastSecond = 0;
    unsigned long _lastSecondStart = 0;
    unsigned long _currentDataRate = 0;
    uint32_t _byteHistogram[256] = {0};
};

#endif // STATISTICS_H
