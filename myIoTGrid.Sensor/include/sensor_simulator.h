/**
 * myIoTGrid.Sensor - Sensor Simulator
 *
 * Generates realistic simulated sensor data with 5 profiles:
 * - Normal: Standard indoor conditions
 * - Winter: Cold outdoor conditions
 * - Summer: Hot summer conditions
 * - Storm: Thunderstorm/rain event
 * - Stress: Extreme values for testing
 */

#ifndef SENSOR_SIMULATOR_H
#define SENSOR_SIMULATOR_H

#include <Arduino.h>

/**
 * Simulation Profile enum
 */
enum class SimulationProfile {
    NORMAL,     // 18-25°C, 40-70% humidity
    WINTER,     // -5-10°C, 60-90% humidity
    SUMMER,     // 25-35°C, 30-50% humidity
    STORM,      // 18-22°C, 80-95% humidity
    STRESS      // 0-50°C, 0-100% - extreme values
};

/**
 * Sensor reading structure
 */
struct SimulatedReading {
    float temperature;      // °C
    float humidity;         // %
    float pressure;         // hPa
    float co2;              // ppm
    float light;            // lux
    float soilMoisture;     // %
    unsigned long timestamp;
};

/**
 * Sensor Simulator class
 *
 * Generates realistic sensor values with:
 * - Random walk algorithms for natural variation
 * - Daily cycle simulation (warmer during day)
 * - 5 predefined profiles for different scenarios
 */
class SensorSimulator {
public:
    SensorSimulator();

    /**
     * Initialize the simulator
     * @param profile Starting simulation profile
     */
    void init(SimulationProfile profile = SimulationProfile::NORMAL);

    /**
     * Set simulation profile
     */
    void setProfile(SimulationProfile profile);

    /**
     * Get current profile
     */
    SimulationProfile getProfile() const;

    /**
     * Get profile name as string
     */
    static const char* getProfileName(SimulationProfile profile);

    /**
     * Update all sensor values
     * Call periodically (e.g., every second)
     */
    void update();

    /**
     * Get current simulated values
     */
    SimulatedReading getReading() const;

    /**
     * Get individual sensor values
     */
    float getTemperature() const;
    float getHumidity() const;
    float getPressure() const;
    float getCO2() const;
    float getLight() const;
    float getSoilMoisture() const;

    /**
     * Enable/disable daily cycle simulation
     * When enabled, values change based on time of day
     */
    void setDailyCycleEnabled(bool enabled);
    bool isDailyCycleEnabled() const;

    /**
     * Set simulation time (for testing)
     * @param hour Hour of day (0-23)
     */
    void setSimulatedHour(int hour);

    /**
     * Reset all values to profile defaults
     */
    void reset();

private:
    SimulationProfile _profile;
    SimulatedReading _current;
    bool _dailyCycleEnabled;
    int _simulatedHour;
    unsigned long _lastUpdate;

    // Profile ranges
    struct ProfileRange {
        float tempMin, tempMax;
        float humidMin, humidMax;
        float pressMin, pressMax;
        float co2Min, co2Max;
        float lightMin, lightMax;
        float soilMin, soilMax;
    };

    ProfileRange getProfileRange() const;

    // Random walk helpers
    float randomWalk(float current, float min, float max, float maxStep);
    float applyDailyCycle(float value, float min, float max, float amplitude);
    int getCurrentHour() const;
};

#endif // SENSOR_SIMULATOR_H
