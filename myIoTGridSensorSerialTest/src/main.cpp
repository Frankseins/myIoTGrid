/**
 * myIoTGridSensorSerialTest - Hardware Test Tool
 *
 * Tests:
 * - SD Card (SPI)
 * - DS18B20 Temperature Sensor (OneWire)
 *
 * Commands via Serial:
 *   'c' - Run SD Card test
 *   't' - Read temperature
 *   's' - Show statistics
 *   'r' - Reset statistics
 *   '?' - Help
 */

#include <Arduino.h>
#include "config.h"
#include "sd_card_test.h"
#include "ds18b20_handler.h"
#include "ultrasonic_test.h"
#include "led_status.h"

// ============================================================================
// Global Instances
// ============================================================================

SDCardTest sdTest;
DS18B20Handler tempSensor;
UltrasonicTest ultrasonicSensor;
LEDStatus led;

// Timing
unsigned long lastTempRequest = 0;
bool waitingForConversion = false;
bool tempSensorFound = false;

// Statistics
float minTemp = 999.0;
float maxTemp = -999.0;
uint32_t readingCount = 0;
float tempSum = 0.0;

// ============================================================================
// Display Functions
// ============================================================================

void printHeader() {
    Serial.println();
    Serial.println("========================================");
    Serial.println("   myIoTGrid Hardware Test Tool v1.2    ");
    Serial.println("========================================");
    Serial.println();
    Serial.println("Commands:");
    Serial.println("  c - Run SD Card test");
    Serial.println("  t - Read temperature (DS18B20)");
    Serial.println("  u - Run ultrasonic sensor test (JSN-SR04T)");
    Serial.println("  d - Continuous distance reading");
    Serial.println("  s - Show temperature statistics");
    Serial.println("  r - Reset statistics");
    Serial.println("  ? - Show this help");
    Serial.println();
    Serial.println("========================================");
    Serial.println();
}

void printTemperature(float tempC, uint8_t sensorIndex) {
    float tempF = tempC * 9.0 / 5.0 + 32.0;

    Serial.printf("[TEMP] Sensor %d: %.2f C (%.2f F)\n", sensorIndex, tempC, tempF);
}

void printStatistics() {
    if (readingCount == 0) {
        Serial.println("[STATS] No readings yet");
        return;
    }

    float avgTemp = tempSum / readingCount;

    Serial.println();
    Serial.println("[STATISTICS]");
    Serial.printf("  Readings: %lu\n", readingCount);
    Serial.printf("  Min:      %.2f C\n", minTemp);
    Serial.printf("  Max:      %.2f C\n", maxTemp);
    Serial.printf("  Avg:      %.2f C\n", avgTemp);
    Serial.printf("  Range:    %.2f C\n", maxTemp - minTemp);
    Serial.println();
}

void initTempSensor() {
    Serial.println("[INIT] Scanning for DS18B20 sensor...");
    tempSensor.begin();

    if (tempSensor.getDeviceCount() == 0) {
        Serial.println("[WARN] No DS18B20 sensor found on GPIO15");
        Serial.println("       (Temperature readings disabled)");
        tempSensorFound = false;
    } else {
        Serial.printf("[OK] Found %d DS18B20 sensor(s)\n", tempSensor.getDeviceCount());
        tempSensorFound = true;

        // Request first temperature
        tempSensor.requestTemperatures();
        lastTempRequest = millis();
        waitingForConversion = true;
    }
}

void readTemperature() {
    if (!tempSensorFound) {
        Serial.println("[ERROR] No DS18B20 sensor available");
        return;
    }

    // Request new reading
    tempSensor.requestTemperatures();
    delay(750);  // Wait for conversion (12-bit)

    for (uint8_t i = 0; i < tempSensor.getDeviceCount(); i++) {
        float tempC = tempSensor.getTemperatureC(i);

        if (tempSensor.isValidReading(tempC)) {
            // Update statistics
            readingCount++;
            tempSum += tempC;
            if (tempC < minTemp) minTemp = tempC;
            if (tempC > maxTemp) maxTemp = tempC;

            printTemperature(tempC, i);
            led.blinkData();
        } else {
            Serial.printf("[ERROR] Invalid reading from sensor %d: %.2f\n", i, tempC);
            led.blinkError();
        }
    }
}

// ============================================================================
// Setup
// ============================================================================

void setup() {
    Serial.begin(USB_SERIAL_BAUDRATE);
    delay(2000);  // Wait for Serial

    printHeader();

    // Initialize LED
    led.begin();

    // Initialize temperature sensor
    initTempSensor();

    Serial.println();
    Serial.println("[READY] Type 'c' to test SD card, 't' for temperature");
    Serial.println();
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    // Auto temperature reading (if sensor found)
    if (tempSensorFound) {
        unsigned long now = millis();

        if (waitingForConversion && tempSensor.isConversionComplete()) {
            waitingForConversion = false;

            for (uint8_t i = 0; i < tempSensor.getDeviceCount(); i++) {
                float tempC = tempSensor.getTemperatureC(i);

                if (tempSensor.isValidReading(tempC)) {
                    readingCount++;
                    tempSum += tempC;
                    if (tempC < minTemp) minTemp = tempC;
                    if (tempC > maxTemp) maxTemp = tempC;

                    printTemperature(tempC, i);
                    led.blinkData();
                }
            }
        }

        if (!waitingForConversion && (now - lastTempRequest >= TEMP_READ_INTERVAL_MS)) {
            tempSensor.requestTemperatures();
            lastTempRequest = now;
            waitingForConversion = true;
        }
    }

    // Check for Serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'c':
            case 'C':
                sdTest.runFullTest();
                break;
            case 't':
            case 'T':
                readTemperature();
                break;
            case 'u':
            case 'U':
                ultrasonicSensor.runTest();
                break;
            case 'd':
            case 'D':
                ultrasonicSensor.continuousRead();
                break;
            case 's':
            case 'S':
                printStatistics();
                break;
            case 'r':
            case 'R':
                minTemp = 999.0;
                maxTemp = -999.0;
                readingCount = 0;
                tempSum = 0.0;
                Serial.println("[INFO] Statistics reset");
                break;
            case '?':
                printHeader();
                break;
            case '\n':
            case '\r':
                break;
            default:
                Serial.printf("[?] Unknown command: '%c' (type '?' for help)\n", cmd);
        }
    }

    led.update();
    delay(10);
}
