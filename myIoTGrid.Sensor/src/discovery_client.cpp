#include "discovery_client.h"
#include <ArduinoJson.h>

#ifdef PLATFORM_NATIVE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#endif

#ifdef PLATFORM_ESP32
#include <WiFiUdp.h>
#endif

// Discovery protocol constants
static const char* DISCOVERY_MSG_TYPE = "MYIOTGRID_DISCOVER";
static const char* HUB_MSG_TYPE = "MYIOTGRID_HUB";
static const char* BROADCAST_ADDR = "255.255.255.255";

DiscoveryClient::DiscoveryClient()
    : _discoveryPort(5001), _timeoutMs(5000), _lastError("") {
}

DiscoveryClient::~DiscoveryClient() {
}

void DiscoveryClient::configure(int discoveryPort, int timeoutMs) {
    _discoveryPort = discoveryPort;
    _timeoutMs = timeoutMs;
}

bool DiscoveryClient::isSupported() const {
#if defined(PLATFORM_NATIVE) || defined(PLATFORM_ESP32)
    return true;
#else
    return false;
#endif
}

String DiscoveryClient::buildRequestJson(const String& serial,
                                         const String& firmwareVersion,
                                         const String& hardwareType) {
    JsonDocument doc;
    doc["messageType"] = DISCOVERY_MSG_TYPE;
    doc["serial"] = serial.c_str();
    doc["firmwareVersion"] = firmwareVersion.c_str();
    doc["hardwareType"] = hardwareType.c_str();

    String json;
    serializeJson(doc, json);
    return json;
}

DiscoveryResponse DiscoveryClient::parseResponse(const String& json) {
    DiscoveryResponse response;
    response.success = false;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        response.errorMessage = "JSON parse error: " + String(error.c_str());
        return response;
    }

    // Check message type
    const char* msgType = doc["messageType"] | "";
    if (strcmp(msgType, HUB_MSG_TYPE) != 0) {
        response.errorMessage = "Invalid message type: " + String(msgType);
        return response;
    }

    // Extract fields
    response.hubId = doc["hubId"] | "";
    response.hubName = doc["hubName"] | "";
    response.apiUrl = doc["apiUrl"] | "";
    response.apiVersion = doc["apiVersion"] | "";
    response.protocolVersion = doc["protocolVersion"] | "";

    if (response.apiUrl.length() == 0) {
        response.errorMessage = "Missing apiUrl in response";
        return response;
    }

    response.success = true;
    return response;
}

DiscoveryResponse DiscoveryClient::discover(const String& serial,
                                             const String& firmwareVersion,
                                             const String& hardwareType) {
    if (!isSupported()) {
        DiscoveryResponse response;
        response.success = false;
        response.errorMessage = "Discovery not supported on this platform";
        return response;
    }

    String requestJson = buildRequestJson(serial, firmwareVersion, hardwareType);

#ifdef PLATFORM_NATIVE
    return discoverNative(requestJson);
#elif defined(PLATFORM_ESP32)
    return discoverESP32(requestJson);
#else
    DiscoveryResponse response;
    response.success = false;
    response.errorMessage = "No discovery implementation for this platform";
    return response;
#endif
}

// ============================================================================
// Native Platform Implementation (Linux/Mac)
// ============================================================================

#ifdef PLATFORM_NATIVE

DiscoveryResponse DiscoveryClient::discoverNative(const String& requestJson) {
    DiscoveryResponse response;
    response.success = false;

    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        response.errorMessage = "Failed to create socket";
        _lastError = response.errorMessage;
        return response;
    }

    // Enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        response.errorMessage = "Failed to enable broadcast";
        _lastError = response.errorMessage;
        close(sock);
        return response;
    }

    // Set socket to non-blocking for timeout handling
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    // Setup broadcast address
    struct sockaddr_in broadcastAddr;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(_discoveryPort);
    broadcastAddr.sin_addr.s_addr = inet_addr(BROADCAST_ADDR);

    // Send discovery request
    Serial.println("[Discovery] Sending UDP broadcast to " + String(BROADCAST_ADDR) +
                   ":" + String(_discoveryPort));

    const char* data = requestJson.c_str();
    ssize_t sent = sendto(sock, data, strlen(data), 0,
                          (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    if (sent < 0) {
        response.errorMessage = "Failed to send broadcast";
        _lastError = response.errorMessage;
        close(sock);
        return response;
    }

    Serial.println("[Discovery] Waiting for response (timeout: " + String(_timeoutMs) + "ms)...");

    // Wait for response with timeout
    struct pollfd pfd;
    pfd.fd = sock;
    pfd.events = POLLIN;

    int pollResult = poll(&pfd, 1, _timeoutMs);

    if (pollResult < 0) {
        response.errorMessage = "Poll error";
        _lastError = response.errorMessage;
        close(sock);
        return response;
    }

    if (pollResult == 0) {
        response.errorMessage = "Discovery timeout - no Hub found";
        _lastError = response.errorMessage;
        close(sock);
        return response;
    }

    // Receive response
    char buffer[1024];
    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);

    ssize_t received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                                (struct sockaddr*)&senderAddr, &senderLen);

    close(sock);

    if (received < 0) {
        response.errorMessage = "Failed to receive response";
        _lastError = response.errorMessage;
        return response;
    }

    buffer[received] = '\0';
    String responseJson = String(buffer);

    Serial.println("[Discovery] Received response from " +
                   String(inet_ntoa(senderAddr.sin_addr)));

    // Parse response
    response = parseResponse(responseJson);

    if (response.success) {
        Serial.println("[Discovery] Found Hub: " + response.hubName +
                       " at " + response.apiUrl);
    } else {
        _lastError = response.errorMessage;
    }

    return response;
}

#endif // PLATFORM_NATIVE

// ============================================================================
// ESP32 Platform Implementation
// ============================================================================

#ifdef PLATFORM_ESP32

DiscoveryResponse DiscoveryClient::discoverESP32(const String& requestJson) {
    DiscoveryResponse response;
    response.success = false;

    WiFiUDP udp;

    // Begin UDP on any available port
    if (!udp.begin(0)) {
        response.errorMessage = "Failed to start UDP";
        _lastError = response.errorMessage;
        return response;
    }

    Serial.println("[Discovery] Sending UDP broadcast to " + String(BROADCAST_ADDR) +
                   ":" + String(_discoveryPort));

    // Send broadcast
    udp.beginPacket(BROADCAST_ADDR, _discoveryPort);
    udp.write((const uint8_t*)requestJson.c_str(), requestJson.length());
    if (!udp.endPacket()) {
        response.errorMessage = "Failed to send broadcast";
        _lastError = response.errorMessage;
        udp.stop();
        return response;
    }

    Serial.println("[Discovery] Waiting for response (timeout: " + String(_timeoutMs) + "ms)...");

    // Wait for response with timeout
    unsigned long startTime = millis();
    while (millis() - startTime < (unsigned long)_timeoutMs) {
        int packetSize = udp.parsePacket();
        if (packetSize > 0) {
            char buffer[1024];
            int len = udp.read(buffer, sizeof(buffer) - 1);
            if (len > 0) {
                buffer[len] = '\0';
                String responseJson = String(buffer);

                Serial.println("[Discovery] Received response from " +
                               udp.remoteIP().toString());

                udp.stop();

                // Parse response
                response = parseResponse(responseJson);

                if (response.success) {
                    Serial.println("[Discovery] Found Hub: " + response.hubName +
                                   " at " + response.apiUrl);
                } else {
                    _lastError = response.errorMessage;
                }

                return response;
            }
        }
        delay(10);
    }

    udp.stop();

    response.errorMessage = "Discovery timeout - no Hub found";
    _lastError = response.errorMessage;
    return response;
}

#endif // PLATFORM_ESP32
