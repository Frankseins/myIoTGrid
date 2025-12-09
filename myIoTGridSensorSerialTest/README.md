# myIoTGridSensorSerialTest

**UART Serial Monitor Firmware** - ESP32 Sensor Testing Tool with Auto-Baudrate Detection

## Overview

A compact ESP32 firmware for testing and analyzing UART sensors. Automatically cycles through common baudrates when no data is received, making it easy to discover the correct settings for unknown sensors.

## Features

- Continuous UART data reception on UART2
- Automatic baudrate cycling (1200 - 115200 baud)
- 20-second timeout before trying next baudrate
- Hex dump display with ASCII representation
- Data statistics and rate monitoring
- Visual LED feedback
- Multiple display modes (Hex, ASCII, Raw)

## Hardware Setup

```
┌─────────────────┐          ┌─────────────────┐
│     SENSOR      │          │     ESP32       │
├─────────────────┤          ├─────────────────┤
│ TX  ──────────────────────> GPIO16 (RX)     │
│ RX  <────────────────────── GPIO17 (TX)     │
│ GND ──────────────────────> GND             │
│ VCC <────────── 3.3V/5V ─── 3.3V/5V         │
└─────────────────┘          └─────────────────┘
```

### Pin Configuration

| ESP32 Pin | Function | Connect To |
|-----------|----------|------------|
| GPIO16    | UART2 RX | Sensor TX  |
| GPIO17    | UART2 TX | Sensor RX  |
| GPIO2     | Status LED | (built-in) |
| GND       | Ground   | Sensor GND |
| 3.3V/5V   | Power    | Sensor VCC |

## Supported Baudrates

The firmware automatically cycles through these baudrates:

1. 1200 baud
2. 2400 baud
3. 4800 baud
4. **9600 baud** (default start)
5. 19200 baud
6. 38400 baud
7. 57600 baud
8. 115200 baud

## Installation

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- ESP32 development board
- USB cable

### Build & Upload

```bash
cd myIoTGridSensorSerialTest

# Build
pio run

# Upload to ESP32
pio run --target upload

# Open Serial Monitor
pio device monitor
```

## Usage

### Serial Monitor Commands

Connect to the ESP32 via USB Serial at **115200 baud** and use these commands:

| Key | Command | Description |
|-----|---------|-------------|
| `h` | HEX_DUMP | Full hex dump with ASCII (default) |
| `x` | HEX_ONLY | Compact hex bytes only |
| `a` | ASCII | ASCII text only |
| `r` | RAW | Raw binary data |
| `s` | STATS | Show statistics dashboard |
| `?` | HELP | Show help |

### Output Format

**Hex Dump Mode (default):**
```
Timestamp  | Offset | Hex Data                                      | ASCII
-----------+--------+-----------------------------------------------+----------------
00001234   | 0000:  24 47 50 47 47 41 2C 31  32 33 35 31 39 2E 30 30 | $GPGGA,123519.00
00001234   | 0010:  2C 34 38 30 37 2E 30 33  38 2C 4E 2C 30 31 31 33 | ,4807.038,N,0113
```

**Statistics Dashboard:**
```
========================================
       UART Serial Monitor Dashboard
========================================
| Baudrate:     9600                   |
| Total Bytes:  1234                   |
| Data Rate:    45.2 B/s               |
| Last Byte:    123 ms                 |
| Uptime:       300 s                  |
========================================
```

## LED Feedback

| LED Pattern | Meaning |
|-------------|---------|
| Quick flash | Data received |
| 3x blink | Baudrate changed |
| Off | Waiting for data |

## Testing Scenarios

### Test 1: Known Sensor (GPS Module)

1. Connect GPS module (usually 9600 baud)
2. Firmware starts at 9600 → immediate data reception
3. NMEA sentences displayed in hex dump

### Test 2: Unknown Baudrate

1. Connect sensor with 115200 baud
2. Firmware starts at 9600 → no data
3. After 20s → switches to next baudrate
4. Continues until 115200 → data detected!

### Test 3: Protocol Analysis

1. Connect unknown sensor
2. Let firmware find correct baudrate
3. Use `a` command for ASCII view
4. Analyze protocol structure

## Configuration

Edit `src/config.h` to customize:

```cpp
// UART Pins
#define UART_RX_PIN         16
#define UART_TX_PIN         17

// Timeout before baudrate switch
#define BAUDRATE_TIMEOUT_MS 20000   // 20 seconds

// Default starting baudrate
#define DEFAULT_BAUDRATE    9600
```

## Troubleshooting

### No Data Received

1. Check wiring (TX/RX may be swapped)
2. Verify sensor power supply
3. Confirm sensor is outputting data
4. Check ground connection

### Garbage Data

1. Wrong baudrate (let it auto-cycle)
2. Check for voltage level mismatch (3.3V vs 5V)
3. Verify serial settings (8N1 assumed)

### ESP32 Not Responding

1. Press RESET button
2. Check USB connection
3. Try different USB cable/port

## License

MIT License - Part of the myIoTGrid project.

## Author

myIoTGrid Team
