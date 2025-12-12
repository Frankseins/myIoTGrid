/**
 * myIoTGridSensorSerialTest - LED Status
 *
 * Visual feedback via built-in LED
 */

#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>
#include "config.h"

class LEDStatus {
public:
    /**
     * Initialize LED
     */
    void begin() {
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, LOW);
        _ledState = false;
    }

    /**
     * Turn LED on
     */
    void on() {
        digitalWrite(LED_PIN, HIGH);
        _ledState = true;
    }

    /**
     * Turn LED off
     */
    void off() {
        digitalWrite(LED_PIN, LOW);
        _ledState = false;
    }

    /**
     * Toggle LED state
     */
    void toggle() {
        _ledState = !_ledState;
        digitalWrite(LED_PIN, _ledState ? HIGH : LOW);
    }

    /**
     * Blink LED for data reception (quick flash)
     */
    void blinkData() {
        on();
        _blinkOffTime = millis() + 50;
        _autoOff = true;
    }

    /**
     * Blink pattern for baudrate change
     */
    void blinkBaudrateChange() {
        for (int i = 0; i < BAUDRATE_CHANGE_BLINKS; i++) {
            on();
            delay(LED_BLINK_ON_MS);
            off();
            delay(LED_BLINK_OFF_MS);
        }
    }

    /**
     * Blink pattern for error (rapid double blink)
     */
    void blinkError() {
        on();
        delay(100);
        off();
        delay(100);
        on();
        delay(100);
        off();
    }

    /**
     * Update LED state (call in loop)
     */
    void update() {
        if (_autoOff && millis() >= _blinkOffTime) {
            off();
            _autoOff = false;
        }
    }

    /**
     * Set LED brightness using PWM (0-255)
     * Note: Only works if LED_PIN supports PWM
     * @param brightness 0-255
     */
    void setBrightness(uint8_t brightness) {
        analogWrite(LED_PIN, brightness);
        _ledState = (brightness > 0);
    }

    /**
     * Get current LED state
     * @return true if LED is on
     */
    bool isOn() const {
        return _ledState;
    }

private:
    bool _ledState = false;
    bool _autoOff = false;
    unsigned long _blinkOffTime = 0;
};

#endif // LED_STATUS_H
