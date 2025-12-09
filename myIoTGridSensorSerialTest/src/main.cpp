/**
 * myIoTGridSensorSerialTest - UART Serial Monitor
 *
 * ESP32 Firmware for testing and analyzing UART sensors
 * with automatic baudrate detection.
 *
 * Features:
 * - Continuous UART data reception on UART2
 * - Automatic baudrate cycling when no data received
 * - Hex dump display with ASCII representation
 * - Data statistics and rate monitoring
 * - Visual LED feedback
 *
 * Hardware Setup:
 * - Sensor TX -> ESP32 GPIO16 (UART2 RX)
 * - Sensor RX -> ESP32 GPIO17 (UART2 TX) [optional]
 * - Sensor GND -> ESP32 GND
 * - Sensor VCC -> 3.3V or 5V (sensor dependent)
 *
 * USB Serial (115200 baud) is used for output display.
 */

#include <Arduino.h>
#include "config.h"
#include "uart_handler.h"
#include "baudrate_scanner.h"
#include "statistics.h"
#include "hex_dump.h"
#include "led_status.h"

// ============================================================================
// Global Instances
// ============================================================================

UARTHandler uartHandler;
BaudrateScanner scanner;
Statistics stats;
HexDump hexDump;
LEDStatus led;

// ============================================================================
// Configuration
// ============================================================================

// Display mode
enum class DisplayMode {
    HEX_DUMP,       // Full hex dump with ASCII
    HEX_ONLY,       // Hex bytes only (compact)
    ASCII_ONLY,     // ASCII only (for text protocols)
    RAW             // Raw bytes (binary)
};

static DisplayMode displayMode = DisplayMode::HEX_DUMP;

// Timing
static unsigned long lastStatsUpdate = 0;
static unsigned long lastDashboard = 0;
static const unsigned long DASHBOARD_INTERVAL_MS = 5000;  // Full dashboard every 5s

// ============================================================================
// Display Functions
// ============================================================================

void printHeader() {
    Serial.println();
    Serial.println("╔══════════════════════════════════════════════════════════╗");
    Serial.println("║           myIoTGrid UART Serial Monitor v1.0             ║");
    Serial.println("║                 Sensor Testing Tool                      ║");
    Serial.println("╠══════════════════════════════════════════════════════════╣");
    Serial.println("║ Hardware:                                                ║");
    Serial.printf("║   UART RX: GPIO%-2d (connect to Sensor TX)                 ║\n", UART_RX_PIN);
    Serial.printf("║   UART TX: GPIO%-2d (connect to Sensor RX)                 ║\n", UART_TX_PIN);
    Serial.println("║   LED:     GPIO2  (built-in status LED)                  ║");
    Serial.println("╠══════════════════════════════════════════════════════════╣");
    Serial.println("║ Baudrates: 1200 | 2400 | 4800 | 9600 | 19200 | 38400     ║");
    Serial.println("║            57600 | 115200                                ║");
    Serial.printf("║ Timeout:   %d seconds (auto-switch if no data)          ║\n", BAUDRATE_TIMEOUT_MS / 1000);
    Serial.println("╚══════════════════════════════════════════════════════════╝");
    Serial.println();
}

void printBaudrateChange(uint32_t newBaudrate, uint8_t index) {
    Serial.println();
    Serial.println("┌──────────────────────────────────────────┐");
    Serial.printf("│ TIMEOUT - Switching baudrate to %-8d │\n", newBaudrate);
    Serial.printf("│ Baudrate %d of %d in rotation             │\n", index + 1, BaudrateScanner::getBaudrateCount());
    Serial.println("└──────────────────────────────────────────┘");
    Serial.println();
}

void printDataReceived(uint32_t baudrate) {
    Serial.println();
    Serial.println("┌──────────────────────────────────────────┐");
    Serial.printf("│ DATA DETECTED at %-8d baud!          │\n", baudrate);
    Serial.println("│ Locking to this baudrate                 │");
    Serial.println("└──────────────────────────────────────────┘");
    Serial.println();
}

// ============================================================================
// Setup
// ============================================================================

void setup() {
    // Initialize USB Serial for output
    Serial.begin(USB_SERIAL_BAUDRATE);
    delay(1000);

    // Print startup header
    printHeader();

    // Initialize components
    led.begin();
    stats.begin();
    hexDump.begin();
    scanner.begin();

    // Initialize UART with default baudrate
    uartHandler.begin(DEFAULT_BAUDRATE);

    Serial.printf("[READY] Starting at %d baud. Waiting for sensor data...\n\n", DEFAULT_BAUDRATE);
    Serial.println("Timestamp  | Offset | Hex Data                                      | ASCII");
    Serial.println("-----------+--------+-----------------------------------------------+----------------");
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    unsigned long now = millis();
    static bool firstDataReceived = false;

    // Check for incoming UART data
    while (uartHandler.available()) {
        uint8_t data = uartHandler.read();

        // First data at this baudrate?
        if (!firstDataReceived) {
            firstDataReceived = true;
            printDataReceived(scanner.getCurrentBaudrate());
        }

        // Record statistics
        stats.addByte(data);

        // Reset baudrate timeout
        scanner.resetTimeout();

        // LED flash for data
        led.blinkData();

        // Display based on mode
        switch (displayMode) {
            case DisplayMode::HEX_DUMP:
                hexDump.addByte(data, now);
                break;

            case DisplayMode::HEX_ONLY:
                HexDump::printByte(data);
                if (stats.getTotalBytes() % HEX_DUMP_WIDTH == 0) {
                    Serial.println();
                }
                break;

            case DisplayMode::ASCII_ONLY:
                if (data >= 32 && data < 127) {
                    Serial.print((char)data);
                } else if (data == '\n') {
                    Serial.println();
                } else if (data == '\r') {
                    // Ignore CR
                } else {
                    Serial.print('.');
                }
                break;

            case DisplayMode::RAW:
                Serial.write(data);
                break;
        }
    }

    // Check for baudrate timeout (no data received)
    if (scanner.checkTimeout()) {
        // Flush any partial hex dump line
        hexDump.flush(now);

        // Switch to next baudrate
        uint32_t newBaudrate = scanner.nextBaudrate();
        printBaudrateChange(newBaudrate, scanner.getCurrentIndex());

        // LED feedback
        led.blinkBaudrateChange();

        // Change UART baudrate
        uartHandler.changeBaudrate(newBaudrate);

        // Reset first data flag
        firstDataReceived = false;

        // Print waiting message
        Serial.println("Timestamp  | Offset | Hex Data                                      | ASCII");
        Serial.println("-----------+--------+-----------------------------------------------+----------------");
    }

    // Update statistics
    if (now - lastStatsUpdate >= STATS_INTERVAL_MS) {
        stats.update();
        lastStatsUpdate = now;

        // Print status line (overwrites itself)
        if (!stats.hasReceivedData() || stats.getTimeSinceLastByte() > 2000) {
            stats.printStatusLine(scanner.getCurrentBaudrate(), scanner.getTimeoutProgress());
        }
    }

    // Print full dashboard periodically (if receiving data)
    if (now - lastDashboard >= DASHBOARD_INTERVAL_MS) {
        if (stats.hasReceivedData() && stats.getTimeSinceLastByte() < 2000) {
            hexDump.flush(now);
            stats.printDashboard(scanner.getCurrentBaudrate());
            Serial.println("Timestamp  | Offset | Hex Data                                      | ASCII");
            Serial.println("-----------+--------+-----------------------------------------------+----------------");
        }
        lastDashboard = now;
    }

    // Update LED state
    led.update();

    // Check for Serial commands (mode switching)
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'h':
            case 'H':
                displayMode = DisplayMode::HEX_DUMP;
                Serial.println("\n[MODE] Switched to HEX_DUMP mode\n");
                break;
            case 'x':
            case 'X':
                displayMode = DisplayMode::HEX_ONLY;
                Serial.println("\n[MODE] Switched to HEX_ONLY mode\n");
                break;
            case 'a':
            case 'A':
                displayMode = DisplayMode::ASCII_ONLY;
                Serial.println("\n[MODE] Switched to ASCII_ONLY mode\n");
                break;
            case 'r':
            case 'R':
                displayMode = DisplayMode::RAW;
                Serial.println("\n[MODE] Switched to RAW mode\n");
                break;
            case 's':
            case 'S':
                stats.printDashboard(scanner.getCurrentBaudrate());
                break;
            case '?':
                Serial.println("\n[HELP] Commands:");
                Serial.println("  h - Hex dump mode (default)");
                Serial.println("  x - Hex only mode (compact)");
                Serial.println("  a - ASCII only mode");
                Serial.println("  r - Raw mode (binary)");
                Serial.println("  s - Show statistics");
                Serial.println("  ? - Show this help\n");
                break;
        }
    }

    // Small delay to prevent busy-looping
    delay(1);
}
