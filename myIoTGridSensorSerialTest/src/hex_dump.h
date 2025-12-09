/**
 * myIoTGridSensorSerialTest - Hex Dump Formatter
 *
 * Format received bytes as hex dump with ASCII representation
 */

#ifndef HEX_DUMP_H
#define HEX_DUMP_H

#include <Arduino.h>
#include "config.h"

class HexDump {
public:
    /**
     * Initialize hex dump formatter
     */
    void begin() {
        _bufferPos = 0;
        _lineOffset = 0;
        memset(_lineBuffer, 0, sizeof(_lineBuffer));
    }

    /**
     * Add a byte to the hex dump
     * Automatically prints when line is full
     * @param data byte to add
     * @param timestamp timestamp for this byte
     */
    void addByte(uint8_t data, unsigned long timestamp) {
        _lineBuffer[_bufferPos] = data;
        _bufferPos++;

        if (_bufferPos >= HEX_DUMP_WIDTH) {
            printLine(timestamp);
            _bufferPos = 0;
            _lineOffset += HEX_DUMP_WIDTH;
        }
    }

    /**
     * Flush any remaining bytes (partial line)
     * @param timestamp current timestamp
     */
    void flush(unsigned long timestamp) {
        if (_bufferPos > 0) {
            printLine(timestamp);
            _bufferPos = 0;
            _lineOffset += HEX_DUMP_WIDTH;
        }
    }

    /**
     * Reset the formatter
     */
    void reset() {
        _bufferPos = 0;
        _lineOffset = 0;
    }

    /**
     * Print raw byte in both hex and ASCII
     * @param data byte to print
     */
    static void printByte(uint8_t data) {
        Serial.printf("%02X ", data);
    }

    /**
     * Print byte with ASCII if printable
     * @param data byte to print
     */
    static void printByteWithAscii(uint8_t data) {
        Serial.printf("%02X", data);
        if (data >= 32 && data < 127) {
            Serial.printf("(%c) ", (char)data);
        } else {
            Serial.print("    ");
        }
    }

private:
    uint8_t _lineBuffer[HEX_DUMP_WIDTH];
    uint8_t _bufferPos = 0;
    unsigned long _lineOffset = 0;

    /**
     * Print a complete line with offset, hex bytes, and ASCII
     * @param timestamp timestamp for this line
     */
    void printLine(unsigned long timestamp) {
        // Print timestamp and offset
        Serial.printf("%08lu | %04lX: ", timestamp, _lineOffset);

        // Print hex bytes
        for (uint8_t i = 0; i < HEX_DUMP_WIDTH; i++) {
            if (i < _bufferPos) {
                Serial.printf("%02X ", _lineBuffer[i]);
            } else {
                Serial.print("   ");
            }
            // Add extra space in middle
            if (i == 7) Serial.print(" ");
        }

        Serial.print("| ");

        // Print ASCII representation
        for (uint8_t i = 0; i < _bufferPos; i++) {
            uint8_t c = _lineBuffer[i];
            if (c >= 32 && c < 127) {
                Serial.print((char)c);
            } else {
                Serial.print('.');
            }
        }

        Serial.println();
    }
};

#endif // HEX_DUMP_H
