/**
 * SD Card Test Module
 *
 * Simple test to verify SD card hardware and wiring
 */

#ifndef SD_CARD_TEST_H
#define SD_CARD_TEST_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// SD Card Pins (Standard VSPI)
#define SD_MISO     19
#define SD_MOSI     23
#define SD_SCK      18
#define SD_CS       5

class SDCardTest {
public:
    bool runFullTest() {
        Serial.println();
        Serial.println("========================================");
        Serial.println("       SD CARD DIAGNOSTIC TEST          ");
        Serial.println("========================================");
        Serial.println();

        // Print pin configuration
        Serial.println("[CONFIG] Pin Configuration:");
        Serial.printf("  MISO (DO)  : GPIO %d\n", SD_MISO);
        Serial.printf("  MOSI (DI)  : GPIO %d\n", SD_MOSI);
        Serial.printf("  SCK  (CLK) : GPIO %d\n", SD_SCK);
        Serial.printf("  CS   (SS)  : GPIO %d\n", SD_CS);
        Serial.println();

        // Initialize SPI
        Serial.println("[TEST 1] Initializing SPI bus...");
        SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
        Serial.println("  SPI initialized OK");
        Serial.println();

        // Try to initialize SD card
        Serial.println("[TEST 2] Mounting SD card...");

        if (!SD.begin(SD_CS)) {
            Serial.println("  FAILED: SD.begin() returned false");
            Serial.println();
            printTroubleshooting();
            return false;
        }

        Serial.println("  SD card mounted OK!");
        Serial.println();

        // Get card type
        Serial.println("[TEST 3] Reading card info...");
        uint8_t cardType = SD.cardType();

        Serial.print("  Card Type: ");
        switch (cardType) {
            case CARD_NONE:
                Serial.println("NONE (no card detected)");
                printTroubleshooting();
                return false;
            case CARD_MMC:
                Serial.println("MMC");
                break;
            case CARD_SD:
                Serial.println("SD");
                break;
            case CARD_SDHC:
                Serial.println("SDHC");
                break;
            default:
                Serial.println("UNKNOWN");
        }

        // Get card size
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
        uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);

        Serial.printf("  Card Size:  %llu MB\n", cardSize);
        Serial.printf("  Total:      %llu MB\n", totalBytes);
        Serial.printf("  Used:       %llu MB\n", usedBytes);
        Serial.printf("  Free:       %llu MB\n", totalBytes - usedBytes);
        Serial.println();

        // Test write
        Serial.println("[TEST 4] Testing write...");
        File testFile = SD.open("/test.txt", FILE_WRITE);
        if (!testFile) {
            Serial.println("  FAILED: Could not create test file");
            return false;
        }

        testFile.println("myIoTGrid SD Card Test");
        testFile.printf("Timestamp: %lu\n", millis());
        testFile.close();
        Serial.println("  Write test OK!");
        Serial.println();

        // Test read
        Serial.println("[TEST 5] Testing read...");
        testFile = SD.open("/test.txt", FILE_READ);
        if (!testFile) {
            Serial.println("  FAILED: Could not open test file for reading");
            return false;
        }

        Serial.println("  File contents:");
        while (testFile.available()) {
            Serial.print("    ");
            Serial.println(testFile.readStringUntil('\n'));
        }
        testFile.close();
        Serial.println("  Read test OK!");
        Serial.println();

        // List root directory
        Serial.println("[TEST 6] Listing root directory...");
        File root = SD.open("/");
        if (root) {
            File file = root.openNextFile();
            int fileCount = 0;
            while (file) {
                Serial.printf("  %s%s (%d bytes)\n",
                    file.isDirectory() ? "[DIR] " : "      ",
                    file.name(),
                    file.size());
                file = root.openNextFile();
                fileCount++;
            }
            root.close();
            Serial.printf("  Total: %d items\n", fileCount);
        }
        Serial.println();

        // Cleanup
        SD.remove("/test.txt");

        // Success!
        Serial.println("========================================");
        Serial.println("     ALL TESTS PASSED!                ");
        Serial.println("========================================");
        Serial.println();

        return true;
    }

private:
    void printTroubleshooting() {
        Serial.println();
        Serial.println("========================================");
        Serial.println("        TROUBLESHOOTING GUIDE           ");
        Serial.println("========================================");
        Serial.println();
        Serial.println("1. CHECK WIRING:");
        Serial.println("   SD Module    ESP32");
        Serial.println("   ---------    -----");
        Serial.println("   VCC      ->  3.3V (or 5V if module has regulator)");
        Serial.println("   GND      ->  GND");
        Serial.printf("   MISO/DO  ->  GPIO %d\n", SD_MISO);
        Serial.printf("   MOSI/DI  ->  GPIO %d\n", SD_MOSI);
        Serial.printf("   SCK/CLK  ->  GPIO %d\n", SD_SCK);
        Serial.printf("   CS/SS    ->  GPIO %d\n", SD_CS);
        Serial.println();
        Serial.println("2. CHECK SD CARD:");
        Serial.println("   - Is the card inserted?");
        Serial.println("   - Is it formatted as FAT32?");
        Serial.println("   - Cards > 32GB need manual FAT32 format");
        Serial.println("   - Try a different/smaller SD card");
        Serial.println();
        Serial.println("3. CHECK HARDWARE:");
        Serial.println("   - Are all connections secure?");
        Serial.println("   - Is the SD module getting power?");
        Serial.println("   - Try different jumper wires");
        Serial.println();
        Serial.println("4. COMMON ISSUES:");
        Serial.println("   - MISO/MOSI swapped (DO=MISO, DI=MOSI)");
        Serial.println("   - Wrong voltage (some modules need 5V)");
        Serial.println("   - Loose breadboard connections");
        Serial.println("   - Defective SD card or module");
        Serial.println();
        Serial.println("========================================");
    }
};

#endif // SD_CARD_TEST_H
