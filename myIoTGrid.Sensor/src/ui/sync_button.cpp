/**
 * myIoTGrid.Sensor - Sync Button Implementation
 */

#include "sync_button.h"

SyncButton::SyncButton()
    : _pin(SYNC_BUTTON_PIN)
    , _activeLow(SYNC_BUTTON_ACTIVE_LOW)
    , _enabled(true)
    , _initialized(false)
    , _isPressed(false)
    , _lastReading(false)
    , _lastDebounceTime(0)
    , _pressStartTime(0)
    , _longPressTriggered(false)
    , _lastHeldCallback(0)
{
}

void SyncButton::init(int pin, bool activeLow) {
    _pin = pin;
    _activeLow = activeLow;

    // Configure pin with internal pull-up if active low
    if (_activeLow) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT_PULLDOWN);
    }

    // Read initial state
    _lastReading = readButton();
    _isPressed = _lastReading;

    _initialized = true;

    Serial.printf("[SyncButton] Initialized on GPIO%d (active %s)\n",
                  _pin, _activeLow ? "LOW" : "HIGH");
}

ButtonEvent SyncButton::update() {
    if (!_initialized || !_enabled) {
        return ButtonEvent::NONE;
    }

    ButtonEvent event = processButton();

    // Call held callback periodically while button is held
    if (_isPressed && _onHeld) {
        unsigned long now = millis();
        if (now - _lastHeldCallback >= HELD_CALLBACK_INTERVAL) {
            _lastHeldCallback = now;
            _onHeld(getHeldTime());
        }
    }

    // Call press callback on events
    if (event != ButtonEvent::NONE && event != ButtonEvent::HELD && _onPress) {
        _onPress(event);
    }

    return event;
}

unsigned long SyncButton::getHeldTime() const {
    if (!_isPressed || _pressStartTime == 0) {
        return 0;
    }
    return millis() - _pressStartTime;
}

bool SyncButton::isWaitingForLongPress() const {
    if (!_isPressed) return false;
    unsigned long heldTime = getHeldTime();
    return heldTime >= SYNC_BUTTON_SHORT_PRESS_MS &&
           heldTime < SYNC_BUTTON_LONG_PRESS_MS &&
           !_longPressTriggered;
}

uint8_t SyncButton::getLongPressProgress() const {
    if (!_isPressed) return 0;

    unsigned long heldTime = getHeldTime();
    if (heldTime >= SYNC_BUTTON_LONG_PRESS_MS) return 100;

    return (uint8_t)((heldTime * 100) / SYNC_BUTTON_LONG_PRESS_MS);
}

bool SyncButton::readButton() const {
    bool reading = digitalRead(_pin);
    // Invert if active low
    return _activeLow ? !reading : reading;
}

ButtonEvent SyncButton::processButton() {
    bool reading = readButton();
    ButtonEvent event = ButtonEvent::NONE;
    unsigned long now = millis();

    // Check for state change (debounce)
    if (reading != _lastReading) {
        _lastDebounceTime = now;
    }

    // If reading has been stable for debounce period
    if ((now - _lastDebounceTime) > SYNC_BUTTON_DEBOUNCE_MS) {
        // State changed
        if (reading != _isPressed) {
            _isPressed = reading;

            if (_isPressed) {
                // Button just pressed
                _pressStartTime = now;
                _longPressTriggered = false;
                _lastHeldCallback = now;

                Serial.println("[SyncButton] Button pressed");
                Serial.println("[SyncButton]   Short press (<1s) = Normal sync");
                Serial.println("[SyncButton]   Long press (>=3s) = Force sync all");

            } else {
                // Button just released
                unsigned long heldTime = now - _pressStartTime;

                if (!_longPressTriggered) {
                    // Check press duration
                    if (heldTime < SYNC_BUTTON_SHORT_PRESS_MS) {
                        Serial.printf("[SyncButton] Short press detected (%lu ms)\n", heldTime);
                        event = ButtonEvent::SHORT_PRESS;
                    } else if (heldTime < SYNC_BUTTON_LONG_PRESS_MS) {
                        // Between short and long - treat as short
                        Serial.printf("[SyncButton] Medium press (%lu ms) - treating as short\n", heldTime);
                        event = ButtonEvent::SHORT_PRESS;
                    }
                    // If long press was already triggered, don't send another event
                }

                _pressStartTime = 0;
            }
        }

        // Check for long press while held
        if (_isPressed && !_longPressTriggered) {
            unsigned long heldTime = now - _pressStartTime;
            if (heldTime >= SYNC_BUTTON_LONG_PRESS_MS) {
                Serial.printf("[SyncButton] Long press detected (%lu ms)\n", heldTime);
                event = ButtonEvent::LONG_PRESS;
                _longPressTriggered = true;
            }
        }
    }

    _lastReading = reading;

    return event;
}
