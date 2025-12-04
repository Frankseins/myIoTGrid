#ifndef DISCOVERY_CLIENT_H
#define DISCOVERY_CLIENT_H

#include <Arduino.h>
#include <functional>

/**
 * Discovery response from Hub
 */
struct DiscoveryResponse {
    bool success;               // True if discovery succeeded
    String hubId;               // Hub identifier
    String hubName;             // Hub display name
    String apiUrl;              // Full API URL (e.g., "https://192.168.1.100:5001")
    String apiVersion;          // API version
    String protocolVersion;     // Discovery protocol version
    String errorMessage;        // Error message if failed
};

/**
 * Discovery Client for finding Hub on local network via UDP broadcast.
 *
 * Works on both Native (Linux/Mac) and ESP32 platforms.
 * Uses UDP broadcast to discover the Hub without requiring manual configuration.
 */
class DiscoveryClient {
public:
    DiscoveryClient();
    ~DiscoveryClient();

    /**
     * Configure the discovery client
     * @param discoveryPort UDP port to send discovery broadcasts (default: 5002)
     * @param timeoutMs Timeout for discovery in milliseconds (default: 5000)
     */
    void configure(int discoveryPort = 5002, int timeoutMs = 5000);

    /**
     * Discover Hub on the local network.
     * Sends UDP broadcast and waits for response.
     *
     * @param serial Sensor serial number
     * @param firmwareVersion Current firmware version
     * @param hardwareType Hardware type (e.g., "ESP32", "SIM")
     * @return Discovery response with Hub information
     */
    DiscoveryResponse discover(const String& serial,
                               const String& firmwareVersion,
                               const String& hardwareType);

    /**
     * Check if discovery is supported on this platform
     */
    bool isSupported() const;

    /**
     * Get the last error message
     */
    String getLastError() const { return _lastError; }

private:
    int _discoveryPort;
    int _timeoutMs;
    String _lastError;

    /**
     * Build the discovery request JSON
     */
    String buildRequestJson(const String& serial,
                           const String& firmwareVersion,
                           const String& hardwareType);

    /**
     * Parse the discovery response JSON
     */
    DiscoveryResponse parseResponse(const String& json);

#ifdef PLATFORM_NATIVE
    /**
     * Native platform implementation using BSD sockets
     */
    DiscoveryResponse discoverNative(const String& requestJson);
#endif

#ifdef PLATFORM_ESP32
    /**
     * ESP32 platform implementation using WiFiUDP
     */
    DiscoveryResponse discoverESP32(const String& requestJson);
#endif
};

#endif // DISCOVERY_CLIENT_H
