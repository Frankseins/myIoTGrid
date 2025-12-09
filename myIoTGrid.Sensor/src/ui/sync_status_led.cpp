/**
 * myIoTGrid.Sensor - Sync Status LED Implementation
 */

#include "sync_status_led.h"

SyncStatusLED::SyncStatusLED()
    : _pin(SYNC_STATUS_LED_PIN)
    , _activeHigh(SYNC_STATUS_LED_ACTIVE_HIGH)
    , _enabled(true)
    , _initialized(false)
    , _pattern(SyncLedPattern::OFF)
    , _ledState(false)
    , _lastToggle(0)
    , _blinkPhase(0)
    , _forceActive(false)
    , _forceState(false)
    , _forceEndTime(0)
{
}

void SyncStatusLED::init(int pin, bool activeHigh) {
    _pin = pin;
    _activeHigh = activeHigh;

    pinMode(_pin, OUTPUT);
    setLed(false);

    _initialized = true;

    Serial.printf("[SyncLED] Initialized on GPIO%d (active %s)\n",
                  _pin, _activeHigh ? "HIGH" : "LOW");
}

void SyncStatusLED::update() {
    if (!_initialized || !_enabled) {
        return;
    }

    // Check for force timeout
    if (_forceActive && _forceEndTime > 0 && millis() >= _forceEndTime) {
        releaseForce();
    }

    // If force is active, maintain force state
    if (_forceActive) {
        setLed(_forceState);
        return;
    }

    // Process blink pattern
    processPattern();
}

void SyncStatusLED::setPattern(SyncLedPattern pattern) {
    if (_pattern != pattern) {
        _pattern = pattern;
        _lastToggle = millis();
        _blinkPhase = 0;

        // Release any force state when pattern changes
        releaseForce();

        // Set initial state based on pattern
        switch (_pattern) {
            case SyncLedPattern::OFF:
                setLed(false);
                break;
            case SyncLedPattern::SOLID_ON:
                setLed(true);
                break;
            default:
                // Blink patterns start with LED on
                setLed(true);
                break;
        }
    }
}

void SyncStatusLED::forceOn() {
    _forceActive = true;
    _forceState = true;
    _forceEndTime = 0; // No timeout
    setLed(true);
}

void SyncStatusLED::forceOff() {
    _forceActive = true;
    _forceState = false;
    _forceEndTime = 0; // No timeout
    setLed(false);
}

void SyncStatusLED::releaseForce() {
    _forceActive = false;
    _forceEndTime = 0;
}

void SyncStatusLED::flash(int durationMs) {
    _forceActive = true;
    _forceState = true;
    _forceEndTime = millis() + durationMs;
    setLed(true);
}

void SyncStatusLED::setLed(bool on) {
    _ledState = on;
    digitalWrite(_pin, _activeHigh ? (on ? HIGH : LOW) : (on ? LOW : HIGH));
}

void SyncStatusLED::processPattern() {
    unsigned long now = millis();
    unsigned long elapsed = now - _lastToggle;

    switch (_pattern) {
        case SyncLedPattern::OFF:
            setLed(false);
            break;

        case SyncLedPattern::SOLID_ON:
            setLed(true);
            break;

        case SyncLedPattern::SLOW_BLINK:
            // 1Hz: 500ms on, 500ms off
            if (elapsed >= SLOW_BLINK_PERIOD / 2) {
                _lastToggle = now;
                setLed(!_ledState);
            }
            break;

        case SyncLedPattern::FAST_BLINK:
            // 4Hz: 125ms on, 125ms off
            if (elapsed >= FAST_BLINK_PERIOD / 2) {
                _lastToggle = now;
                setLed(!_ledState);
            }
            break;

        case SyncLedPattern::DOUBLE_BLINK:
            // Double blink pattern: ON-OFF-ON-OFF----ON-OFF-ON-OFF----
            // Phase 0: first on, 1: first off, 2: second on, 3: second off, 4: pause
            switch (_blinkPhase) {
                case 0: // First ON
                    setLed(true);
                    if (elapsed >= DOUBLE_BLINK_ON) {
                        _lastToggle = now;
                        _blinkPhase = 1;
                    }
                    break;
                case 1: // First OFF (between blinks)
                    setLed(false);
                    if (elapsed >= DOUBLE_BLINK_OFF) {
                        _lastToggle = now;
                        _blinkPhase = 2;
                    }
                    break;
                case 2: // Second ON
                    setLed(true);
                    if (elapsed >= DOUBLE_BLINK_ON) {
                        _lastToggle = now;
                        _blinkPhase = 3;
                    }
                    break;
                case 3: // Second OFF (start of pause)
                    setLed(false);
                    if (elapsed >= DOUBLE_BLINK_PAUSE) {
                        _lastToggle = now;
                        _blinkPhase = 0;
                    }
                    break;
            }
            break;

        case SyncLedPattern::ALTERNATING:
            // Alternating on/off pattern
            if (elapsed >= ALTERNATING_PERIOD) {
                _lastToggle = now;
                setLed(!_ledState);
            }
            break;
    }
}
