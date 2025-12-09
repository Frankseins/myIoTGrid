/**
 * myIoTGrid.Sensor - Serial Capture
 * Sprint 8: Remote Debug System
 *
 * Captures all Serial output for remote transmission.
 * Acts as a transparent proxy - forwards everything to real Serial
 * while also buffering for remote upload.
 */

#ifndef SERIAL_CAPTURE_H
#define SERIAL_CAPTURE_H

#include <Arduino.h>
#include <vector>
#include <functional>

/**
 * SerialCapture - Captures Serial output for remote transmission
 */
class SerialCapture : public Print {
public:
    static SerialCapture& getInstance();

    // Prevent copying
    SerialCapture(const SerialCapture&) = delete;
    SerialCapture& operator=(const SerialCapture&) = delete;

    /**
     * Initialize capture
     * @param bufferSize Max characters to buffer before forcing upload
     */
    void begin(size_t bufferSize = 8192);

    /**
     * Enable/disable capture
     */
    void setEnabled(bool enabled) { _enabled = enabled; }
    bool isEnabled() const { return _enabled; }

    /**
     * Print implementation - forwards to Serial (legacy, hook handles capture)
     */
    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t size) override;

    /**
     * Capture a character (called by low-level hook)
     */
    void captureChar(char c);

    /**
     * Get captured lines since last call
     * Clears the internal buffer after returning
     */
    std::vector<String> getAndClearLines();

    /**
     * Get current buffer as single string (for batch upload)
     * Clears the internal buffer after returning
     */
    String getAndClearBuffer();

    /**
     * Check if there's data to send
     */
    bool hasData() const { return _lineBuffer.length() > 0 || !_completedLines.empty(); }

    /**
     * Get line count waiting to be sent
     */
    size_t getLineCount() const { return _completedLines.size(); }

    /**
     * Check if a line should be captured (filter mode)
     * Only captures: [HW] hardware check, errors, warnings, critical
     */
    bool shouldCaptureLine(const String& line) const;

private:
    SerialCapture();

    bool _enabled;
    bool _initialized;
    size_t _maxBufferSize;

    String _lineBuffer;                    // Current line being built
    std::vector<String> _completedLines;   // Completed lines ready for upload
    unsigned long _lastCharTime;           // For detecting end of partial lines
};

// Global instance for easy access
extern SerialCapture& RemoteSerial;

#endif // SERIAL_CAPTURE_H
