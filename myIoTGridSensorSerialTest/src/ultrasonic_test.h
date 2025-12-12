/**
 * JSN-SR04T Ultrasonic Sensor Test Module
 *
 * Waterproof ultrasonic distance sensor
 *
 * IMPORTANT: ECHO pin outputs 5V! Use voltage divider to 3.3V for ESP32!
 * Example: 1kΩ + 2kΩ divider (5V → 3.3V)
 */

#ifndef ULTRASONIC_TEST_H
#define ULTRASONIC_TEST_H

#include <Arduino.h>

// JSN-SR04T Pins
#define ULTRASONIC_TRIG     25
#define ULTRASONIC_ECHO     26

class UltrasonicTest {
public:
    void begin() {
        pinMode(ULTRASONIC_TRIG, OUTPUT);
        pinMode(ULTRASONIC_ECHO, INPUT);
        digitalWrite(ULTRASONIC_TRIG, LOW);
    }

    float readDistanceCm() {
        // Send 10µs trigger pulse
        digitalWrite(ULTRASONIC_TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(ULTRASONIC_TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTRASONIC_TRIG, LOW);

        // Measure echo pulse duration (timeout 30ms = ~5m max)
        long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000);

        if (duration == 0) {
            return -1;  // Timeout - no echo received
        }

        // Calculate distance: speed of sound = 343 m/s = 0.0343 cm/µs
        // Distance = (duration * 0.0343) / 2 (divide by 2 for round trip)
        float distance = (duration * 0.0343) / 2.0;

        return distance;
    }

    void runTest() {
        Serial.println();
        Serial.println("========================================");
        Serial.println("    JSN-SR04T ULTRASONIC SENSOR TEST    ");
        Serial.println("========================================");
        Serial.println();

        Serial.println("[CONFIG] Pin Configuration:");
        Serial.printf("  TRIG : GPIO %d\n", ULTRASONIC_TRIG);
        Serial.printf("  ECHO : GPIO %d (needs voltage divider!)\n", ULTRASONIC_ECHO);
        Serial.println();

        Serial.println("[INFO] Wiring:");
        Serial.println("  VCC   -> 5V");
        Serial.println("  GND   -> GND");
        Serial.println("  TRIG  -> GPIO 25");
        Serial.println("  ECHO  -> GPIO 26 (via 1k+2k voltage divider!)");
        Serial.println();

        Serial.println("[TEST] Taking 5 measurements...");
        Serial.println();

        begin();
        delay(100);

        int validReadings = 0;
        float totalDistance = 0;
        float minDist = 9999;
        float maxDist = 0;

        for (int i = 0; i < 5; i++) {
            float distance = readDistanceCm();

            if (distance > 0 && distance < 400) {
                Serial.printf("  Reading %d: %.1f cm\n", i + 1, distance);
                validReadings++;
                totalDistance += distance;
                if (distance < minDist) minDist = distance;
                if (distance > maxDist) maxDist = distance;
            } else if (distance < 0) {
                Serial.printf("  Reading %d: TIMEOUT (no echo)\n", i + 1);
            } else {
                Serial.printf("  Reading %d: OUT OF RANGE (%.1f cm)\n", i + 1, distance);
            }

            delay(100);  // Wait between readings
        }

        Serial.println();

        if (validReadings > 0) {
            Serial.println("========================================");
            Serial.println("           TEST PASSED!                 ");
            Serial.println("========================================");
            Serial.printf("  Valid readings: %d/5\n", validReadings);
            Serial.printf("  Average: %.1f cm\n", totalDistance / validReadings);
            Serial.printf("  Min: %.1f cm\n", minDist);
            Serial.printf("  Max: %.1f cm\n", maxDist);
        } else {
            Serial.println("========================================");
            Serial.println("           TEST FAILED!                 ");
            Serial.println("========================================");
            Serial.println();
            Serial.println("  No valid readings received.");
            Serial.println();
            Serial.println("  Check:");
            Serial.println("  1. Wiring (VCC, GND, TRIG, ECHO)");
            Serial.println("  2. Voltage divider on ECHO pin!");
            Serial.println("  3. Sensor orientation (facing open space)");
            Serial.println("  4. Min distance is ~20cm for JSN-SR04T");
        }

        Serial.println();
    }

    void continuousRead() {
        float distance = readDistanceCm();

        if (distance > 0 && distance < 400) {
            Serial.printf("[DIST] %.1f cm\n", distance);
        } else if (distance < 0) {
            Serial.println("[DIST] TIMEOUT");
        } else {
            Serial.printf("[DIST] OUT OF RANGE (%.1f cm)\n", distance);
        }
    }
};

#endif // ULTRASONIC_TEST_H
