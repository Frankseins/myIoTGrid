/**
 * myIoTGrid.Sensor - Serial Capture Implementation
 * Sprint 8: Remote Debug System
 *
 * Uses ESP32 low-level putc hook to capture ALL Serial output,
 * including from libraries and third-party code.
 */

#include "serial_capture.h"

#ifdef PLATFORM_ESP32
extern "C" {
    #include "driver/uart.h"
    #include "rom/ets_sys.h"
    void ets_install_putc1(void (*)(char));
}

// Forward declaration for the hook
static void serial_capture_putc_hook(char c);
static bool _hookInstalled = false;
#endif

// Global instance
SerialCapture& RemoteSerial = SerialCapture::getInstance();

SerialCapture& SerialCapture::getInstance() {
    static SerialCapture instance;
    return instance;
}

SerialCapture::SerialCapture()
    : _enabled(false)
    , _initialized(false)
    , _maxBufferSize(8192)
    , _lastCharTime(0) {
}

void SerialCapture::begin(size_t bufferSize) {
    _maxBufferSize = bufferSize;
    _initialized = true;
    _lineBuffer.reserve(256);

#ifdef PLATFORM_ESP32
    // Install low-level putc hook to capture ALL Serial output
    if (!_hookInstalled) {
        ets_install_putc1(serial_capture_putc_hook);
        _hookInstalled = true;
    }
#endif
}

#ifdef PLATFORM_ESP32
// Low-level character capture hook - called for every character sent to Serial
static void serial_capture_putc_hook(char c) {
    // Write directly to UART (avoiding recursion)
    uart_write_bytes(UART_NUM_0, &c, 1);

    // Capture if enabled
    SerialCapture& capture = SerialCapture::getInstance();
    if (capture.isEnabled()) {
        capture.captureChar(c);
    }
}
#endif

void SerialCapture::captureChar(char c) {
    if (!_enabled || !_initialized) return;

    _lastCharTime = millis();

    if (c == '\n') {
        // Line complete - check filter and store if matches
        if (_lineBuffer.length() > 0) {
            // Trim trailing \r if present
            if (_lineBuffer.endsWith("\r")) {
                _lineBuffer.remove(_lineBuffer.length() - 1);
            }

            // Only capture lines that match our filter criteria
            if (shouldCaptureLine(_lineBuffer)) {
                _completedLines.push_back(_lineBuffer);

                // Prevent memory overflow
                if (_completedLines.size() > 500) {
                    _completedLines.erase(_completedLines.begin());
                }
            }
            _lineBuffer = "";
        }
    } else if (c != '\r') {
        // Add character to current line (ignore \r)
        if (_lineBuffer.length() < 1024) {
            _lineBuffer += c;
        }
    }
}

bool SerialCapture::shouldCaptureLine(const String& line) const {
    // Capture [HW] hardware check messages - but NOT sensor readings that end with [HW]
    // Hardware check lines START with [HW], sensor readings END with [REMOTE] [HW]
    if (line.startsWith("[HW]")) return true;

    // Capture error messages (case insensitive patterns)
    if (line.indexOf("Error") >= 0) return true;
    if (line.indexOf("ERROR") >= 0) return true;
    if (line.indexOf("error") >= 0) return true;
    if (line.indexOf("Failed") >= 0) return true;
    if (line.indexOf("FAILED") >= 0) return true;
    if (line.indexOf("failed") >= 0) return true;
    if (line.indexOf("FAIL") >= 0) return true;

    // Capture warnings
    if (line.indexOf("Warning") >= 0) return true;
    if (line.indexOf("WARNING") >= 0) return true;
    if (line.indexOf("WARN") >= 0) return true;

    // Capture critical messages
    if (line.indexOf("CRITICAL") >= 0) return true;
    if (line.indexOf("Critical") >= 0) return true;

    // Capture exception/crash info
    if (line.indexOf("Exception") >= 0) return true;
    if (line.indexOf("Panic") >= 0) return true;
    if (line.indexOf("PANIC") >= 0) return true;
    if (line.indexOf("Backtrace") >= 0) return true;
    if (line.indexOf("Stack") >= 0) return true;
    if (line.indexOf("Guru Meditation") >= 0) return true;

    // Not a match - don't capture
    return false;
}

size_t SerialCapture::write(uint8_t c) {
    // Just forward to Serial - the low-level hook handles capture
    // This method is only used if someone calls RemoteSerial.print() directly
    Serial.write(c);
    return 1;
}

size_t SerialCapture::write(const uint8_t* buffer, size_t size) {
    // Just forward to Serial - the low-level hook handles capture
    Serial.write(buffer, size);
    return size;
}

std::vector<String> SerialCapture::getAndClearLines() {
    std::vector<String> result = std::move(_completedLines);
    _completedLines.clear();

    // If there's a partial line that's been sitting for a while, include it
    if (_lineBuffer.length() > 0 && (millis() - _lastCharTime) > 500) {
        result.push_back(_lineBuffer);
        _lineBuffer = "";
    }

    return result;
}

String SerialCapture::getAndClearBuffer() {
    String result;

    for (const auto& line : _completedLines) {
        result += line + "\n";
    }
    _completedLines.clear();

    // Include partial line if old enough
    if (_lineBuffer.length() > 0 && (millis() - _lastCharTime) > 500) {
        result += _lineBuffer + "\n";
        _lineBuffer = "";
    }

    return result;
}
