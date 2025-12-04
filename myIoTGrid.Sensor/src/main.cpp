/**
 * myIoTGrid.Sensor - Main Entry Point
 *
 * Self-Provisioning ESP32 Firmware
 *
 * Flow:
 * 1. Boot → Check NVS for stored configuration
 * 2. If no config → Start BLE pairing mode
 * 3. Receive WiFi + API config via BLE
 * 4. Connect to WiFi
 * 5. Validate API key with Hub
 * 6. Enter operational mode (heartbeats + sensor readings)
 */

#include <Arduino.h>
#include <vector>
#include "config.h"
#include "state_machine.h"
#include "config_manager.h"
#include "wifi_manager.h"
#include "api_client.h"
#include "discovery_client.h"

#ifdef PLATFORM_ESP32
#include "ble_service.h"
#include <esp_mac.h>
#include <WiFi.h>
#endif

#ifdef PLATFORM_NATIVE
#include "hal/hal.h"
#endif

// ============================================================================
// Global Instances
// ============================================================================

StateMachine stateMachine;
ConfigManager configManager;
WiFiManager wifiManager;
ApiClient apiClient;
DiscoveryClient discoveryClient;

#ifdef PLATFORM_ESP32
BLEProvisioningService bleService;
#endif

// ============================================================================
// Configuration
// ============================================================================

static const unsigned long HEARTBEAT_INTERVAL_MS = 60000;   // 1 minute
static const unsigned long SENSOR_INTERVAL_MS = 60000;      // 1 minute
static const unsigned long WIFI_CHECK_INTERVAL_MS = 5000;   // 5 seconds
static const unsigned long CONFIG_CHECK_INTERVAL_MS = 60000; // 60 seconds

static unsigned long lastHeartbeat = 0;
static unsigned long lastSensorReading = 0;
static unsigned long lastWiFiCheck = 0;
static unsigned long lastConfigCheck = 0;

// Current sensor configuration from Hub
static NodeConfigurationResponse currentConfig;
static bool configLoaded = false;
static String currentSerial;

// ============================================================================
// BLE Callbacks (ESP32 only)
// ============================================================================

#ifdef PLATFORM_ESP32
void onBLEConfigReceived(const BLEConfig& config) {
    Serial.println("[Main] BLE configuration received!");
    Serial.printf("[Main] NodeID: %s\n", config.nodeId.c_str());
    Serial.printf("[Main] WiFi SSID: %s\n", config.wifiSsid.c_str());
    Serial.printf("[Main] Hub URL: %s\n", config.hubApiUrl.c_str());

    // Save configuration to NVS
    StoredConfig storedConfig;
    storedConfig.nodeId = config.nodeId;
    storedConfig.apiKey = config.apiKey;
    storedConfig.wifiSsid = config.wifiSsid;
    storedConfig.wifiPassword = config.wifiPassword;
    storedConfig.hubApiUrl = config.hubApiUrl;
    storedConfig.isValid = true;

    if (configManager.saveConfig(storedConfig)) {
        Serial.println("[Main] Configuration saved to NVS");

        // Stop BLE service
        bleService.stop();

        // Configure API client
        apiClient.configure(config.hubApiUrl, config.nodeId, config.apiKey);

        // Transition to CONFIGURED state (will attempt WiFi connection)
        stateMachine.processEvent(StateEvent::BLE_CONFIG_RECEIVED);
    } else {
        Serial.println("[Main] Failed to save configuration!");
        stateMachine.processEvent(StateEvent::ERROR_OCCURRED);
    }
}
#endif

// ============================================================================
// WiFi Callbacks
// ============================================================================

void onWiFiConnected(const String& ip) {
    Serial.printf("[Main] WiFi connected! IP: %s\n", ip.c_str());
    stateMachine.processEvent(StateEvent::WIFI_CONNECTED);
}

void onWiFiDisconnected() {
    Serial.println("[Main] WiFi disconnected!");
    stateMachine.processEvent(StateEvent::WIFI_FAILED);
}

void onWiFiFailed(const String& reason) {
    Serial.printf("[Main] WiFi connection failed: %s\n", reason.c_str());
    stateMachine.processEvent(StateEvent::WIFI_FAILED);
}

// ============================================================================
// Operational Functions
// ============================================================================

void sendHeartbeat() {
    if (!apiClient.isConfigured()) {
        return;
    }

#ifndef PLATFORM_NATIVE
    // ESP32: Check WiFi connection
    if (!wifiManager.isConnected()) {
        return;
    }
#endif

    HeartbeatResponse response = apiClient.sendHeartbeat(FIRMWARE_VERSION);
    if (response.success) {
        Serial.printf("[Main] Heartbeat OK, next in %d seconds\n",
                      response.nextHeartbeatSeconds);
    } else {
        Serial.println("[Main] Heartbeat failed!");
    }
}

/**
 * Fetch or refresh sensor configuration from Hub
 */
void fetchSensorConfiguration() {
    if (!apiClient.isConfigured()) {
        return;
    }

    if (currentSerial.length() == 0) {
        Serial.println("[Main] Serial not set, cannot fetch configuration");
        return;
    }

    Serial.println("[Main] Fetching sensor configuration from Hub...");

    NodeConfigurationResponse response = apiClient.fetchConfiguration(currentSerial);

    if (response.success) {
        currentConfig = response;
        configLoaded = true;

        Serial.printf("[Main] Configuration updated: %d sensors\n",
                      (int)currentConfig.sensors.size());

        if (currentConfig.isSimulation) {
            Serial.println("[Main] Node is in SIMULATION mode");
        }
    } else {
        Serial.printf("[Main] Config fetch: %s\n", response.error.c_str());
        // Don't clear configLoaded - keep using last known config
    }
}

/**
 * Generate simulated value based on sensor code or unit
 */
double generateSimulatedValue(const String& sensorCode, const String& unit) {
    // Check by sensor code first (specific sensor types)
    String code = sensorCode;
    code.toLowerCase();

    // Temperature sensors
    if (code.indexOf("temp") >= 0 || code.indexOf("ds18b20") >= 0 ||
        code.indexOf("dht") >= 0 || code.indexOf("bme") >= 0 ||
        code.indexOf("sht") >= 0 || code.indexOf("lm35") >= 0 ||
        code.indexOf("ntc") >= 0 || code.indexOf("pt100") >= 0) {
        // Check unit to determine temperature or other value
        if (unit == "°C" || unit == "C" || unit.indexOf("Celsius") >= 0) {
            return 18.0 + (random(150) / 10.0);  // 18.0 - 33.0 °C
        }
        if (unit == "°F" || unit == "F" || unit.indexOf("Fahrenheit") >= 0) {
            return 64.0 + (random(270) / 10.0);  // 64.0 - 91.0 °F
        }
    }

    // Humidity sensors
    if (code.indexOf("humid") >= 0 || code.indexOf("hum") >= 0 ||
        code.indexOf("dht") >= 0 || code.indexOf("sht") >= 0 ||
        code.indexOf("bme") >= 0 || code.indexOf("hdc") >= 0) {
        if (unit == "%" || unit == "% RH" || unit.indexOf("Humidity") >= 0) {
            return 35.0 + (random(500) / 10.0);  // 35.0 - 85.0 %
        }
    }

    // Light sensors
    if (code.indexOf("light") >= 0 || code.indexOf("bh1750") >= 0 ||
        code.indexOf("tsl") >= 0 || code.indexOf("ldr") >= 0 ||
        code.indexOf("veml") >= 0 || code.indexOf("max44") >= 0) {
        if (unit == "lux" || unit == "Lux" || unit == "lx") {
            return random(15000);  // 0 - 15000 lux
        }
    }

    // Pressure sensors
    if (code.indexOf("pressure") >= 0 || code.indexOf("bmp") >= 0 ||
        code.indexOf("bme") >= 0 || code.indexOf("ms5611") >= 0) {
        if (unit == "hPa" || unit == "mbar") {
            return 980.0 + (random(500) / 10.0);  // 980.0 - 1030.0 hPa
        }
        if (unit == "Pa") {
            return 98000.0 + random(5000);  // 98000 - 103000 Pa
        }
    }

    // CO2 sensors
    if (code.indexOf("co2") >= 0 || code.indexOf("mh-z") >= 0 ||
        code.indexOf("scd") >= 0 || code.indexOf("ccs811") >= 0) {
        if (unit == "ppm") {
            return 400.0 + random(800);  // 400 - 1200 ppm
        }
    }

    // Soil moisture sensors
    if (code.indexOf("soil") >= 0 || code.indexOf("moisture") >= 0) {
        if (unit == "%") {
            return 20.0 + (random(600) / 10.0);  // 20.0 - 80.0 %
        }
    }

    // Distance sensors (ultrasonic)
    if (code.indexOf("distance") >= 0 || code.indexOf("hc-sr04") >= 0 ||
        code.indexOf("ultrasonic") >= 0) {
        if (unit == "cm") {
            return 5.0 + (random(2950) / 10.0);  // 5.0 - 300.0 cm
        }
        if (unit == "mm") {
            return 50.0 + random(2950);  // 50 - 3000 mm
        }
    }

    // Fallback: Generate value based on unit alone
    if (unit == "°C" || unit == "C") {
        return 18.0 + (random(150) / 10.0);
    }
    if (unit == "%" || unit == "% RH") {
        return 30.0 + (random(500) / 10.0);
    }
    if (unit == "hPa" || unit == "mbar") {
        return 980.0 + (random(500) / 10.0);
    }
    if (unit == "ppm") {
        return 400.0 + random(800);
    }
    if (unit == "lux" || unit == "lx") {
        return random(15000);
    }

    // Default: random value 0-100
    return random(10000) / 100.0;
}

void readAndSendSensors() {
    if (!apiClient.isConfigured()) {
        return;
    }

#ifndef PLATFORM_NATIVE
    // ESP32: Check WiFi connection
    if (!wifiManager.isConnected()) {
        Serial.println("[Main] WiFi not connected - skipping sensor readings");
        return;
    }
#endif

    // If we have configuration with sensors, use that
    if (configLoaded && currentConfig.sensors.size() > 0) {
        Serial.printf("[Main] Reading %d configured sensors...\n", (int)currentConfig.sensors.size());

        for (const auto& sensor : currentConfig.sensors) {
            if (!sensor.isActive) {
                Serial.printf("[Main] Skipping inactive sensor: %s\n", sensor.sensorName.c_str());
                continue;
            }

            // If sensor has capabilities, send one reading per capability
            if (sensor.capabilities.size() > 0) {
                for (const auto& cap : sensor.capabilities) {
                    // Generate simulated value based on measurement type and unit
                    double value = generateSimulatedValue(cap.measurementType, cap.unit);

                    // Apply calibration corrections
                    value = (value + sensor.offsetCorrection) * sensor.gainCorrection;

                    // Include endpointId to identify which sensor assignment this reading belongs to
                    if (apiClient.sendReading(cap.measurementType, value, cap.unit, sensor.endpointId)) {
                        Serial.printf("[Main] Sent %s/%s: %.2f %s (Endpoint %d)\n",
                                      sensor.sensorName.c_str(), cap.displayName.c_str(),
                                      value, cap.unit.c_str(), sensor.endpointId);
                    } else {
                        Serial.printf("[Main] Failed to send %s/%s reading\n",
                                      sensor.sensorName.c_str(), cap.measurementType.c_str());
                    }
                }
            } else {
                // Fallback: Send single reading with sensor code as measurement type
                double value = generateSimulatedValue(sensor.sensorCode, "");

                // Apply calibration corrections
                value = (value + sensor.offsetCorrection) * sensor.gainCorrection;

                // Include endpointId to identify which sensor assignment this reading belongs to
                if (apiClient.sendReading(sensor.sensorCode, value, "", sensor.endpointId)) {
                    Serial.printf("[Main] Sent %s: %.2f (Endpoint %d)\n",
                                  sensor.sensorName.c_str(), value, sensor.endpointId);
                } else {
                    Serial.printf("[Main] Failed to send %s reading\n", sensor.sensorName.c_str());
                }
            }
        }
    } else {
        // Fallback: Default simulated readings when no config
        Serial.println("[Main] No sensor configuration - sending default readings");

        float temperature = 20.0 + (random(100) / 10.0);  // 20.0 - 30.0°C
        float humidity = 40.0 + (random(400) / 10.0);     // 40.0 - 80.0%

        if (apiClient.sendReading("temperature", temperature, "°C")) {
            Serial.printf("[Main] Sent temperature: %.1f°C\n", temperature);
        }

        if (apiClient.sendReading("humidity", humidity, "%")) {
            Serial.printf("[Main] Sent humidity: %.1f%%\n", humidity);
        }
    }
}

bool registerWithHub() {
    if (apiClient.getBaseUrl().length() == 0) {
        Serial.println("[Main] Base URL not set for registration");
        return false;
    }

    Serial.println("[Main] Registering with Hub...");

    // Get serial number
#ifdef PLATFORM_NATIVE
    String serial = hal::get_device_serial().c_str();
#else
    String serial = configManager.getSerial();
#endif

    // Store serial for later configuration fetches
    currentSerial = serial;

    // Define sensor capabilities
    std::vector<String> capabilities = {"temperature", "humidity", "pressure", "co2", "light"};

    RegistrationResponse response = apiClient.registerNode(
        serial,
        FIRMWARE_VERSION,
        HARDWARE_TYPE,
        capabilities
    );

    if (response.success) {
        Serial.printf("[Main] Registered as: %s\n", response.name.c_str());
        Serial.printf("[Main]   Node ID: %s\n", response.nodeId.c_str());
        Serial.printf("[Main]   Interval: %d seconds\n", response.intervalSeconds);
        Serial.printf("[Main]   New Node: %s\n", response.isNewNode ? "yes" : "no");

        // Immediately fetch configuration after successful registration
        Serial.println("[Main] Fetching initial sensor configuration...");
        fetchSensorConfiguration();

        return true;
    } else {
        Serial.printf("[Main] Registration failed: %s\n", response.error.c_str());
        return false;
    }
}

bool validateApiKeyWithHub() {
    if (!apiClient.isConfigured()) {
        return false;
    }

    Serial.println("[Main] Validating API key with Hub...");
    if (apiClient.validateApiKey()) {
        Serial.println("[Main] API key valid!");
        return true;
    } else {
        Serial.println("[Main] API key invalid or Hub unreachable!");
        return false;
    }
}

// ============================================================================
// State Handlers
// ============================================================================

/**
 * Attempt Hub Discovery via UDP broadcast.
 * Returns true if Hub was discovered and configuration is ready.
 */
bool attemptHubDiscovery() {
    Serial.println("[Main] Attempting Hub Discovery via UDP broadcast...");

    // Configure discovery client
    int discoveryPort = config::DISCOVERY_PORT;

#ifdef PLATFORM_NATIVE
    const char* portEnv = std::getenv(config::ENV_DISCOVERY_PORT);
    if (portEnv) {
        discoveryPort = atoi(portEnv);
    }
    // Get sensor serial from HAL (native only)
    String serial = hal::get_device_serial().c_str();
#else
    // ESP32: Generate serial from MAC address
    String serial = "ESP-UNKNOWN";
#ifdef PLATFORM_ESP32
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char serialBuf[20];
    snprintf(serialBuf, sizeof(serialBuf), "ESP-%02X%02X%02X%02X",
             mac[2], mac[3], mac[4], mac[5]);
    serial = String(serialBuf);
#endif
#endif

    discoveryClient.configure(discoveryPort, config::DISCOVERY_TIMEOUT_MS);

    // Attempt discovery with retries
    for (int attempt = 1; attempt <= config::DISCOVERY_RETRY_COUNT; attempt++) {
        Serial.printf("[Main] Discovery attempt %d/%d...\n", attempt, config::DISCOVERY_RETRY_COUNT);

        DiscoveryResponse response = discoveryClient.discover(
            serial,
            FIRMWARE_VERSION,
            HARDWARE_TYPE
        );

        if (response.success) {
            Serial.println("[Main] Hub discovered!");
            Serial.printf("[Main]   Hub ID: %s\n", response.hubId.c_str());
            Serial.printf("[Main]   Hub Name: %s\n", response.hubName.c_str());
            Serial.printf("[Main]   API URL: %s\n", response.apiUrl.c_str());

            // Configure API client with discovered URL
            // Use serial as nodeId and empty apiKey for now (will register)
            apiClient.configure(response.apiUrl, serial, "");

            return true;
        }

        Serial.printf("[Main] Discovery failed: %s\n", response.errorMessage.c_str());

        if (attempt < config::DISCOVERY_RETRY_COUNT) {
            Serial.printf("[Main] Retrying in %d ms...\n", config::DISCOVERY_RETRY_DELAY_MS);
            delay(config::DISCOVERY_RETRY_DELAY_MS);
        }
    }

    Serial.println("[Main] Hub Discovery failed after all attempts");
    return false;
}

void handleUnconfiguredState() {
#ifdef PLATFORM_ESP32
    // Start BLE pairing service
    if (!bleService.isAdvertising()) {
        Serial.println("[Main] Starting BLE pairing service...");
        bleService.setConfigCallback(onBLEConfigReceived);

        // Generate unique device name from MAC address
        uint8_t mac[6];
        esp_efuse_mac_get_default(mac);
        char deviceName[32];
        snprintf(deviceName, sizeof(deviceName), "myIoTGrid-%02X%02X",
                 mac[4], mac[5]);

        bleService.init(deviceName);
        bleService.startAdvertising();
        stateMachine.processEvent(StateEvent::BLE_PAIR_START);
    }
#else
    // Native/simulation mode - try Hub Discovery first
    static bool discoveryAttempted = false;

    // Check discovery configuration
    const char* discoveryEnabled = std::getenv(config::ENV_DISCOVERY_ENABLED);
    bool tryDiscovery = true;

    // Explicitly disabled?
    if (discoveryEnabled && strcmp(discoveryEnabled, "false") == 0) {
        tryDiscovery = false;
    }

    // Try Discovery first if enabled
    if (tryDiscovery && !discoveryAttempted) {
        discoveryAttempted = true;
        Serial.println("[Main] Attempting Hub Discovery via UDP broadcast...");

        if (attemptHubDiscovery()) {
            // Discovery succeeded - transition to configured state
            Serial.println("[Main] Hub discovered successfully!");
            stateMachine.processEvent(StateEvent::CONFIG_FOUND);
            return;
        }

        Serial.println("[Main] Discovery failed, checking for fallback configuration...");
    }

    // Fallback: Check if hub host is set via environment variable
    const char* hubHost = std::getenv(config::ENV_HUB_HOST);
    if (hubHost && strlen(hubHost) > 0) {
        // Manual configuration via environment variables
        Serial.println("[Main] Using fallback configuration from environment variables");

        const char* hubPort = std::getenv(config::ENV_HUB_PORT);
        const char* hubProtocol = std::getenv(config::ENV_HUB_PROTOCOL);

        String protocol = hubProtocol ? String(hubProtocol) : "https";
        int port = hubPort ? atoi(hubPort) : config::DEFAULT_HUB_PORT;

        String apiUrl = protocol + "://" + String(hubHost) + ":" + String(port);
        String serial = hal::get_device_serial().c_str();

        Serial.printf("[Main] API URL: %s\n", apiUrl.c_str());
        Serial.printf("[Main] Serial: %s\n", serial.c_str());

        apiClient.configure(apiUrl, serial, "");

        // Transition to configured state
        stateMachine.processEvent(StateEvent::CONFIG_FOUND);
        return;
    }

    // No discovery and no fallback - wait and retry
    if (!tryDiscovery) {
        Serial.println("[Main] Discovery disabled and no HUB_HOST set - please configure");
        delay(5000);
        return;
    }

    // Discovery failed and no fallback - wait and retry discovery
    Serial.println("[Main] Waiting before next discovery attempt...");
    delay(10000);
    discoveryAttempted = false;  // Allow retry
#endif
}

void handlePairingState() {
#ifdef PLATFORM_ESP32
    // Just wait for BLE callback - bleService handles everything
    bleService.loop();
#endif
}

void handleConfiguredState() {
    static bool nodeRegistered = false;

#ifdef PLATFORM_NATIVE
    // Native mode: Already "connected" via network, register with Hub
    if (!nodeRegistered) {
        if (apiClient.getBaseUrl().length() > 0) {
            if (registerWithHub()) {
                nodeRegistered = true;
                stateMachine.processEvent(StateEvent::API_VALIDATED);
            } else {
                // Registration failed - go to error state
                stateMachine.processEvent(StateEvent::API_FAILED);
            }
        } else {
            Serial.println("[Main] API base URL not set!");
            stateMachine.processEvent(StateEvent::ERROR_OCCURRED);
        }
    }
#else
    // ESP32 mode: Need WiFi connection
    static bool wifiConnecting = false;

    if (!wifiManager.isConnected() && !wifiConnecting) {
        // Load config and connect to WiFi
        StoredConfig config = configManager.loadConfig();
        if (config.isValid) {
            Serial.printf("[Main] Connecting to WiFi: %s\n", config.wifiSsid.c_str());

            // Configure API client
            apiClient.configure(config.hubApiUrl, config.nodeId, config.apiKey);

            // Start WiFi connection
            wifiManager.connect(config.wifiSsid, config.wifiPassword);
            wifiConnecting = true;
        } else {
            Serial.println("[Main] Invalid stored configuration!");
            stateMachine.processEvent(StateEvent::ERROR_OCCURRED);
        }
    }

    // Run WiFi manager loop
    wifiManager.loop();

    // If WiFi connected, register with Hub and start operation
    if (wifiManager.isConnected() && wifiConnecting) {
        wifiConnecting = false;

        // Register with Hub once after connection
        if (!nodeRegistered) {
            if (registerWithHub()) {
                nodeRegistered = true;
                stateMachine.processEvent(StateEvent::API_VALIDATED);
            } else {
                // Registration failed - go to error state
                stateMachine.processEvent(StateEvent::API_FAILED);
            }
        }
    }
#endif
}

void handleOperationalState() {
    unsigned long now = millis();

#ifndef PLATFORM_NATIVE
    // ESP32: Check WiFi connection periodically
    if (now - lastWiFiCheck >= WIFI_CHECK_INTERVAL_MS) {
        lastWiFiCheck = now;
        wifiManager.loop();

        if (!wifiManager.isConnected()) {
            Serial.println("[Main] WiFi lost in operational mode");
            stateMachine.processEvent(StateEvent::WIFI_FAILED);
            return;
        }
    }
#endif

    // Check for configuration updates periodically
    if (now - lastConfigCheck >= CONFIG_CHECK_INTERVAL_MS) {
        lastConfigCheck = now;
        fetchSensorConfiguration();
    }

    // Send heartbeat periodically
    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = now;
        sendHeartbeat();
    }

    // Read and send sensor data periodically
    // Use configured interval if available
    unsigned long sensorInterval = SENSOR_INTERVAL_MS;
    if (configLoaded && currentConfig.defaultIntervalSeconds > 0) {
        sensorInterval = currentConfig.defaultIntervalSeconds * 1000UL;
    }

    if (now - lastSensorReading >= sensorInterval) {
        lastSensorReading = now;
        readAndSendSensors();
    }
}

void handleErrorState() {
    Serial.println("[Main] In error state - checking for recovery...");

    // Check if we have valid config
    if (configManager.hasConfig()) {
        Serial.println("[Main] Config exists, attempting recovery...");
        delay(stateMachine.getRetryDelay());
        stateMachine.processEvent(StateEvent::RETRY_TIMEOUT);
    } else {
        Serial.println("[Main] No config, need BLE pairing...");
        delay(5000);
        // Clear any partial config and restart pairing
        configManager.clearConfig();
        stateMachine.processEvent(StateEvent::RESET_REQUESTED);
    }
}

// ============================================================================
// Arduino Setup & Loop
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("  myIoTGrid Sensor - Self-Provisioning");
    Serial.printf("  Firmware: %s\n", FIRMWARE_VERSION);
    Serial.println("========================================");
    Serial.println();

    // Initialize configuration manager (NVS)
    if (!configManager.init()) {
        Serial.println("[Main] Failed to initialize NVS!");
    }

    // Setup WiFi callbacks
    wifiManager.onConnected(onWiFiConnected);
    wifiManager.onDisconnected(onWiFiDisconnected);
    wifiManager.onFailed(onWiFiFailed);

    // Check for existing configuration
    if (configManager.hasConfig()) {
        Serial.println("[Main] Found stored configuration");
        StoredConfig config = configManager.loadConfig();
        if (config.isValid) {
            Serial.printf("[Main] NodeID: %s\n", config.nodeId.c_str());
            Serial.printf("[Main] Hub URL: %s\n", config.hubApiUrl.c_str());

            // We have config - go directly to CONFIGURED state
            stateMachine.processEvent(StateEvent::CONFIG_FOUND);
        } else {
            Serial.println("[Main] Stored config invalid - need pairing");
            stateMachine.processEvent(StateEvent::NO_CONFIG);
        }
    } else {
        Serial.println("[Main] No stored configuration - need pairing");
        stateMachine.processEvent(StateEvent::NO_CONFIG);
    }

    Serial.printf("[Main] Initial state: %s\n",
                  StateMachine::getStateName(stateMachine.getState()));
}

void loop() {
    NodeState currentState = stateMachine.getState();

    switch (currentState) {
        case NodeState::UNCONFIGURED:
            handleUnconfiguredState();
            break;

        case NodeState::PAIRING:
            handlePairingState();
            break;

        case NodeState::CONFIGURED:
            handleConfiguredState();
            break;

        case NodeState::OPERATIONAL:
            handleOperationalState();
            break;

        case NodeState::ERROR:
            handleErrorState();
            break;
    }

    // Small delay to prevent busy-looping
    delay(10);
}

// ============================================================================
// Native Platform Entry Point
// ============================================================================

#ifdef PLATFORM_NATIVE
int main() {
    setup();
    while (true) {
        loop();
    }
    return 0;
}
#endif
