/**
 * myIoTGridSensorSerialTest - UART Handler
 *
 * UART2 management for sensor data reception
 */

#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "config.h"

class UARTHandler {
public:
    UARTHandler() : _serial(2) {}  // Use UART2

    /**
     * Initialize UART with specified baudrate
     * @param baudrate initial baudrate
     */
    void begin(uint32_t baudrate) {
        _currentBaudrate = baudrate;
        _serial.begin(baudrate, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
        Serial.printf("[UART] Initialized at %d baud (RX=%d, TX=%d)\n",
                     baudrate, UART_RX_PIN, UART_TX_PIN);
    }

    /**
     * Change baudrate (reinitializes UART)
     * @param baudrate new baudrate
     */
    void changeBaudrate(uint32_t baudrate) {
        _serial.end();
        delay(10);
        _currentBaudrate = baudrate;
        _serial.begin(baudrate, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
        Serial.printf("[UART] Changed baudrate to %d\n", baudrate);
    }

    /**
     * Check if data is available
     * @return number of bytes available
     */
    int available() {
        return _serial.available();
    }

    /**
     * Read a single byte
     * @return byte read or -1 if none available
     */
    int read() {
        return _serial.read();
    }

    /**
     * Read multiple bytes into buffer
     * @param buffer destination buffer
     * @param length maximum bytes to read
     * @return number of bytes actually read
     */
    size_t readBytes(uint8_t* buffer, size_t length) {
        return _serial.readBytes(buffer, length);
    }

    /**
     * Write a byte to sensor (if TX connected)
     * @param data byte to send
     */
    void write(uint8_t data) {
        _serial.write(data);
    }

    /**
     * Write multiple bytes to sensor
     * @param buffer data to send
     * @param length number of bytes
     */
    void write(const uint8_t* buffer, size_t length) {
        _serial.write(buffer, length);
    }

    /**
     * Flush UART buffers
     */
    void flush() {
        _serial.flush();
    }

    /**
     * Get current baudrate
     * @return current baudrate
     */
    uint32_t getCurrentBaudrate() const {
        return _currentBaudrate;
    }

    /**
     * Get underlying HardwareSerial reference
     * @return reference to HardwareSerial
     */
    HardwareSerial& getSerial() {
        return _serial;
    }

private:
    HardwareSerial _serial;
    uint32_t _currentBaudrate = DEFAULT_BAUDRATE;
};

#endif // UART_HANDLER_H
