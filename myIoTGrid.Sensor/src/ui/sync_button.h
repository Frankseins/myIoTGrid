/**
 * myIoTGrid.Sensor - Sync Button
 *
 * Manual sync trigger via physical button.
 * Part of Sprint OS-01: Offline-Speicher Implementation
 */

#ifndef SYNC_BUTTON_H
#define SYNC_BUTTON_H

#include <Arduino.h>
#include <functional>

// ============================================================================
// Button Pin Configuration
// ============================================================================

#ifndef SYNC_BUTTON_PIN
#define SYNC_BUTTON_PIN         4       // Default GPIO4
#endif

#ifndef SYNC_BUTTON_ACTIVE_LOW
#define SYNC_BUTTON_ACTIVE_LOW  true    // Pull-up, button connects to GND
#endif

// Timing constants
#define SYNC_BUTTON_DEBOUNCE_MS     50      // Debounce time
#define SYNC_BUTTON_SHORT_PRESS_MS  1000    // Max time for short press
#define SYNC_BUTTON_LONG_PRESS_MS   3000    // Time for long press

/**
 * Button Event Type
 */
enum class ButtonEvent {
    NONE,
    SHORT_PRESS,    // < 1 second - normal sync
    LONG_PRESS,     // >= 3 seconds - force sync all
    HELD            // Button currently held (for feedback)
};

/**
 * Button callbacks
 */
using OnButtonPressCallback = std::function<void(ButtonEvent event)>;
using OnButtonHeldCallback = std::function<void(unsigned long heldMs)>;

/**
 * Sync Button - Manual sync trigger
 */
class SyncButton {
public:
    SyncButton();

    /**
     * Initialize the button
     * @param pin GPIO pin number
     * @param activeLow true if button connects to GND (internal pull-up used)
     */
    void init(int pin = SYNC_BUTTON_PIN, bool activeLow = SYNC_BUTTON_ACTIVE_LOW);

    /**
     * Enable/disable the button
     */
    void setEnabled(bool enabled) { _enabled = enabled; }
    bool isEnabled() const { return _enabled; }

    /**
     * Update button state (call in loop)
     * @return current button event (NONE if no event)
     */
    ButtonEvent update();

    /**
     * Check if button is currently pressed
     */
    bool isPressed() const { return _isPressed; }

    /**
     * Get how long button has been held (0 if not pressed)
     */
    unsigned long getHeldTime() const;

    /**
     * Check if we're waiting for a long press
     * (button is held but long press threshold not yet reached)
     */
    bool isWaitingForLongPress() const;

    /**
     * Set button press callback
     * @param callback function to call on button events
     */
    void onPress(OnButtonPressCallback callback) { _onPress = callback; }

    /**
     * Set button held callback (called periodically while held)
     * @param callback function to call with held duration
     */
    void onHeld(OnButtonHeldCallback callback) { _onHeld = callback; }

    /**
     * Get progress towards long press (0-100%)
     */
    uint8_t getLongPressProgress() const;

private:
    int _pin;
    bool _activeLow;
    bool _enabled;
    bool _initialized;

    // Button state
    bool _isPressed;
    bool _lastReading;
    unsigned long _lastDebounceTime;
    unsigned long _pressStartTime;
    bool _longPressTriggered;

    // Callbacks
    OnButtonPressCallback _onPress;
    OnButtonHeldCallback _onHeld;

    // Last held callback time
    unsigned long _lastHeldCallback;
    static const unsigned long HELD_CALLBACK_INTERVAL = 100; // 100ms

    /**
     * Read the physical button state
     */
    bool readButton() const;

    /**
     * Process debounced button state
     */
    ButtonEvent processButton();
};

#endif // SYNC_BUTTON_H
