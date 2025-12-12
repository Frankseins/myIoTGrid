/**
 * myIoTGridSensorSerialTest - Configuration
 *
 * UART Serial Monitor for Sensor Testing
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// DS18B20 OneWire Configuration
// ============================================================================

// OneWire Pin for DS18B20 Temperature Sensor
#define ONEWIRE_PIN         15    // GPIO15 - connect to DS18B20 Data (yellow wire)

// Temperature reading interval (milliseconds)
#define TEMP_READ_INTERVAL_MS   1000    // 1 second

// Temperature resolution (9-12 bits)
// 9 bit  = 0.5°C,    93.75ms conversion time
// 10 bit = 0.25°C,   187.5ms conversion time
// 11 bit = 0.125°C,  375ms conversion time
// 12 bit = 0.0625°C, 750ms conversion time
#define TEMP_RESOLUTION     12

// ============================================================================
// UART Configuration (legacy - for other sensors)
// ============================================================================

// UART2 Pins (connect sensor here)
#define UART_RX_PIN         16    // GPIO16 - connect to Sensor TX
#define UART_TX_PIN         17     // GPIO17 - connect to Sensor RX (optional)

// Default starting baudrate
#define DEFAULT_BAUDRATE    9600

// Timeout before switching baudrate (milliseconds)
#define BAUDRATE_TIMEOUT_MS 20000   // 20 seconds

// ============================================================================
// Display Configuration
// ============================================================================

// USB Serial baudrate (for output to PC)
#define USB_SERIAL_BAUDRATE 115200

// Bytes per line in hex dump
#define HEX_DUMP_WIDTH      16

// Statistics update interval (milliseconds)
#define STATS_INTERVAL_MS   1000    // 1 second

// ============================================================================
// Buffer Configuration
// ============================================================================

// Ring buffer size for received data
#define RING_BUFFER_SIZE    4096    // 4KB

// ============================================================================
// LED Configuration
// ============================================================================

// Status LED pin (built-in LED on most ESP32 boards)
#define LED_PIN             2

// LED blink count when changing baudrate
#define BAUDRATE_CHANGE_BLINKS  3

// LED blink duration (milliseconds)
#define LED_BLINK_ON_MS     100
#define LED_BLINK_OFF_MS    100

// ============================================================================
// SD Card Configuration (optional)
// ============================================================================

// SD Card CS pin (if using SD card logging)
#define SD_CS_PIN           5

// Log file auto-flush interval (milliseconds)
#define SD_FLUSH_INTERVAL_MS 10000  // 10 seconds

// Enable SD card logging (set to 0 to disable)
#define ENABLE_SD_LOGGING   0

#endif // CONFIG_H
