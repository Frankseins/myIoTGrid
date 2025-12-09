/**
 * myIoTGrid.Sensor - Sync Status LED
 *
 * Visual feedback for sync status via LED.
 * Part of Sprint OS-01: Offline-Speicher Implementation
 */

#ifndef SYNC_STATUS_LED_H
#define SYNC_STATUS_LED_H

#include <Arduino.h>

// ============================================================================
// LED Pin Configuration
// ============================================================================

#ifndef SYNC_STATUS_LED_PIN
#define SYNC_STATUS_LED_PIN     2       // Onboard LED (most ESP32 boards)
#endif

#ifndef SYNC_STATUS_LED_ACTIVE_HIGH
#define SYNC_STATUS_LED_ACTIVE_HIGH true  // HIGH = LED on
#endif

/**
 * LED Pattern - Different blink patterns for status indication
 */
enum class SyncLedPattern {
    OFF,            // LED off - all synced
    SLOW_BLINK,     // 1Hz blink - pending data exists
    FAST_BLINK,     // 4Hz blink - sync in progress
    DOUBLE_BLINK,   // Double blink - sync error
    SOLID_ON,       // Solid on - no WiFi
    ALTERNATING     // Alternating pattern - SD card error
};

/**
 * Sync Status LED - Visual feedback for sync status
 */
class SyncStatusLED {
public:
    SyncStatusLED();

    /**
     * Initialize the LED
     * @param pin GPIO pin number
     * @param activeHigh true if HIGH turns LED on
     */
    void init(int pin = SYNC_STATUS_LED_PIN, bool activeHigh = SYNC_STATUS_LED_ACTIVE_HIGH);

    /**
     * Enable/disable the LED
     */
    void setEnabled(bool enabled) { _enabled = enabled; }
    bool isEnabled() const { return _enabled; }

    /**
     * Update LED state (call in loop)
     */
    void update();

    /**
     * Set the current pattern
     * @param pattern pattern to display
     */
    void setPattern(SyncLedPattern pattern);

    /**
     * Get current pattern
     */
    SyncLedPattern getPattern() const { return _pattern; }

    /**
     * Convenience methods for common states
     */
    void setAllSynced() { setPattern(SyncLedPattern::OFF); }
    void setPendingData() { setPattern(SyncLedPattern::SLOW_BLINK); }
    void setSyncing() { setPattern(SyncLedPattern::FAST_BLINK); }
    void setSyncError() { setPattern(SyncLedPattern::DOUBLE_BLINK); }
    void setNoWifi() { setPattern(SyncLedPattern::SOLID_ON); }
    void setSdCardError() { setPattern(SyncLedPattern::ALTERNATING); }

    /**
     * Force LED on/off (overrides pattern temporarily)
     */
    void forceOn();
    void forceOff();
    void releaseForce();

    /**
     * Flash LED briefly (for data received indication)
     * @param durationMs flash duration in milliseconds
     */
    void flash(int durationMs = 50);

private:
    int _pin;
    bool _activeHigh;
    bool _enabled;
    bool _initialized;

    SyncLedPattern _pattern;
    bool _ledState;
    unsigned long _lastToggle;
    int _blinkPhase;

    // Force override
    bool _forceActive;
    bool _forceState;
    unsigned long _forceEndTime;

    // Timing constants (milliseconds)
    static const int SLOW_BLINK_PERIOD = 1000;      // 1Hz = 500ms on, 500ms off
    static const int FAST_BLINK_PERIOD = 250;       // 4Hz = 125ms on, 125ms off
    static const int DOUBLE_BLINK_ON = 100;         // Double blink on time
    static const int DOUBLE_BLINK_OFF = 100;        // Double blink off time (between blinks)
    static const int DOUBLE_BLINK_PAUSE = 1000;     // Pause between double blinks
    static const int ALTERNATING_PERIOD = 300;      // Alternating pattern period

    /**
     * Set physical LED state
     */
    void setLed(bool on);

    /**
     * Process blink pattern
     */
    void processPattern();
};

#endif // SYNC_STATUS_LED_H
