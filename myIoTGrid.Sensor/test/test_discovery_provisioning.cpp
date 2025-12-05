/**
 * Unit Tests for Discovery & Provisioning (Sprint 6)
 *
 * Tests:
 * - UDP Hub Discovery Protocol
 * - Sensor Simulator with 5 Profiles
 * - State Machine Transitions
 * - Configuration Management
 *
 * Run with: pio test -e native_test
 */

#include <unity.h>
#include "sensor_simulator.h"
#include "state_machine.h"
#include "discovery_client.h"
#include "config.h"
#include <cmath>
#include <cstring>

// ============================================
// Sensor Simulator Tests (Story 7)
// ============================================

void test_simulator_init_normal_profile() {
    SensorSimulator sim;
    sim.init(SimulationProfile::NORMAL);

    TEST_ASSERT_EQUAL(SimulationProfile::NORMAL, sim.getProfile());
    TEST_ASSERT_EQUAL_STRING("Normal", SensorSimulator::getProfileName(SimulationProfile::NORMAL));
}

void test_simulator_all_profiles() {
    TEST_ASSERT_EQUAL_STRING("Normal", SensorSimulator::getProfileName(SimulationProfile::NORMAL));
    TEST_ASSERT_EQUAL_STRING("Winter", SensorSimulator::getProfileName(SimulationProfile::WINTER));
    TEST_ASSERT_EQUAL_STRING("Summer", SensorSimulator::getProfileName(SimulationProfile::SUMMER));
    TEST_ASSERT_EQUAL_STRING("Storm", SensorSimulator::getProfileName(SimulationProfile::STORM));
    TEST_ASSERT_EQUAL_STRING("Stress", SensorSimulator::getProfileName(SimulationProfile::STRESS));
}

void test_simulator_normal_temperature_range() {
    SensorSimulator sim;
    sim.init(SimulationProfile::NORMAL);

    // Run a few updates
    for (int i = 0; i < 10; i++) {
        sim.update();
    }

    float temp = sim.getTemperature();
    // NORMAL profile: 18-25°C
    TEST_ASSERT_GREATER_OR_EQUAL(18.0f, temp);
    TEST_ASSERT_LESS_OR_EQUAL(25.0f, temp);
}

void test_simulator_winter_temperature_range() {
    SensorSimulator sim;
    sim.init(SimulationProfile::WINTER);

    for (int i = 0; i < 10; i++) {
        sim.update();
    }

    float temp = sim.getTemperature();
    // WINTER profile: -5 to 10°C
    TEST_ASSERT_GREATER_OR_EQUAL(-5.0f, temp);
    TEST_ASSERT_LESS_OR_EQUAL(10.0f, temp);
}

void test_simulator_summer_temperature_range() {
    SensorSimulator sim;
    sim.init(SimulationProfile::SUMMER);

    for (int i = 0; i < 10; i++) {
        sim.update();
    }

    float temp = sim.getTemperature();
    // SUMMER profile: 25-35°C
    TEST_ASSERT_GREATER_OR_EQUAL(25.0f, temp);
    TEST_ASSERT_LESS_OR_EQUAL(35.0f, temp);
}

void test_simulator_storm_humidity_range() {
    SensorSimulator sim;
    sim.init(SimulationProfile::STORM);

    for (int i = 0; i < 10; i++) {
        sim.update();
    }

    float humidity = sim.getHumidity();
    // STORM profile: 80-95% humidity
    TEST_ASSERT_GREATER_OR_EQUAL(80.0f, humidity);
    TEST_ASSERT_LESS_OR_EQUAL(95.0f, humidity);
}

void test_simulator_stress_extreme_ranges() {
    SensorSimulator sim;
    sim.init(SimulationProfile::STRESS);

    for (int i = 0; i < 20; i++) {
        sim.update();
    }

    float temp = sim.getTemperature();
    float humidity = sim.getHumidity();
    float co2 = sim.getCO2();

    // STRESS profile: extreme ranges
    TEST_ASSERT_GREATER_OR_EQUAL(0.0f, temp);
    TEST_ASSERT_LESS_OR_EQUAL(50.0f, temp);
    TEST_ASSERT_GREATER_OR_EQUAL(0.0f, humidity);
    TEST_ASSERT_LESS_OR_EQUAL(100.0f, humidity);
    TEST_ASSERT_GREATER_OR_EQUAL(300.0f, co2);
    TEST_ASSERT_LESS_OR_EQUAL(2000.0f, co2);
}

void test_simulator_profile_change() {
    SensorSimulator sim;
    sim.init(SimulationProfile::NORMAL);

    TEST_ASSERT_EQUAL(SimulationProfile::NORMAL, sim.getProfile());

    sim.setProfile(SimulationProfile::WINTER);
    TEST_ASSERT_EQUAL(SimulationProfile::WINTER, sim.getProfile());

    sim.setProfile(SimulationProfile::SUMMER);
    TEST_ASSERT_EQUAL(SimulationProfile::SUMMER, sim.getProfile());
}

void test_simulator_daily_cycle_toggle() {
    SensorSimulator sim;
    sim.init(SimulationProfile::NORMAL);

    // Daily cycle should be enabled by default
    TEST_ASSERT_TRUE(sim.isDailyCycleEnabled());

    sim.setDailyCycleEnabled(false);
    TEST_ASSERT_FALSE(sim.isDailyCycleEnabled());

    sim.setDailyCycleEnabled(true);
    TEST_ASSERT_TRUE(sim.isDailyCycleEnabled());
}

void test_simulator_simulated_hour() {
    SensorSimulator sim;
    sim.init(SimulationProfile::NORMAL);

    // Set simulated hour for testing
    sim.setSimulatedHour(14); // 2 PM - peak temperature hour

    // Run update to apply daily cycle
    sim.update();

    // Temperature at 14:00 should be higher due to daily cycle
    float temp_afternoon = sim.getTemperature();

    sim.setSimulatedHour(3); // 3 AM - coldest hour
    sim.update();
    float temp_night = sim.getTemperature();

    // Values will vary, but both should be within range
    TEST_ASSERT_GREATER_OR_EQUAL(18.0f, temp_afternoon);
    TEST_ASSERT_LESS_OR_EQUAL(25.0f, temp_afternoon);
    TEST_ASSERT_GREATER_OR_EQUAL(18.0f, temp_night);
    TEST_ASSERT_LESS_OR_EQUAL(25.0f, temp_night);
}

void test_simulator_get_reading() {
    SensorSimulator sim;
    sim.init(SimulationProfile::NORMAL);
    sim.update();

    SimulatedReading reading = sim.getReading();

    // All values should be within NORMAL profile ranges
    TEST_ASSERT_GREATER_OR_EQUAL(18.0f, reading.temperature);
    TEST_ASSERT_LESS_OR_EQUAL(25.0f, reading.temperature);
    TEST_ASSERT_GREATER_OR_EQUAL(40.0f, reading.humidity);
    TEST_ASSERT_LESS_OR_EQUAL(70.0f, reading.humidity);
    TEST_ASSERT_GREATER_OR_EQUAL(1010.0f, reading.pressure);
    TEST_ASSERT_LESS_OR_EQUAL(1025.0f, reading.pressure);
    TEST_ASSERT_GREATER_OR_EQUAL(400.0f, reading.co2);
    TEST_ASSERT_LESS_OR_EQUAL(800.0f, reading.co2);
    TEST_ASSERT_GREATER_OR_EQUAL(100.0f, reading.light);
    TEST_ASSERT_LESS_OR_EQUAL(500.0f, reading.light);
    TEST_ASSERT_GREATER_OR_EQUAL(30.0f, reading.soilMoisture);
    TEST_ASSERT_LESS_OR_EQUAL(70.0f, reading.soilMoisture);
}

void test_simulator_reset_values() {
    SensorSimulator sim;
    sim.init(SimulationProfile::STRESS);

    // Run many updates
    for (int i = 0; i < 100; i++) {
        sim.update();
    }

    // Reset to profile defaults
    sim.reset();

    SimulatedReading reading = sim.getReading();

    // After reset, values should be at center of STRESS profile ranges
    // STRESS temp range: 0-50°C, center = 25°C
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 25.0f, reading.temperature);
}

// ============================================
// State Machine Tests (Story 3)
// ============================================

void test_state_machine_initial_state() {
    StateMachine sm;
    TEST_ASSERT_EQUAL(NodeState::UNCONFIGURED, sm.getState());
}

void test_state_machine_unconfigured_to_pairing() {
    StateMachine sm;

    sm.processEvent(StateEvent::BLE_PAIR_START);
    TEST_ASSERT_EQUAL(NodeState::PAIRING, sm.getState());
}

void test_state_machine_unconfigured_to_configured() {
    StateMachine sm;

    sm.processEvent(StateEvent::CONFIG_FOUND);
    TEST_ASSERT_EQUAL(NodeState::CONFIGURED, sm.getState());
}

void test_state_machine_pairing_to_configured() {
    StateMachine sm;
    sm.processEvent(StateEvent::BLE_PAIR_START);  // UNCONFIGURED -> PAIRING

    // BLE_CONFIG_RECEIVED stays in PAIRING, WIFI_CONNECTED transitions to CONFIGURED
    sm.processEvent(StateEvent::BLE_CONFIG_RECEIVED);  // Stays in PAIRING
    sm.processEvent(StateEvent::WIFI_CONNECTED);       // PAIRING -> CONFIGURED
    TEST_ASSERT_EQUAL(NodeState::CONFIGURED, sm.getState());
}

void test_state_machine_configured_to_operational() {
    StateMachine sm;
    sm.processEvent(StateEvent::CONFIG_FOUND);  // UNCONFIGURED -> CONFIGURED
    sm.processEvent(StateEvent::WIFI_CONNECTED);  // May be needed
    sm.processEvent(StateEvent::API_VALIDATED);  // CONFIGURED -> OPERATIONAL

    TEST_ASSERT_EQUAL(NodeState::OPERATIONAL, sm.getState());
}

void test_state_machine_wifi_failure() {
    StateMachine sm;
    sm.processEvent(StateEvent::CONFIG_FOUND);  // UNCONFIGURED -> CONFIGURED

    sm.processEvent(StateEvent::WIFI_FAILED);  // CONFIGURED -> ERROR
    TEST_ASSERT_EQUAL(NodeState::ERROR, sm.getState());
}

void test_state_machine_error_recovery() {
    StateMachine sm;
    sm.processEvent(StateEvent::CONFIG_FOUND);  // UNCONFIGURED -> CONFIGURED
    sm.processEvent(StateEvent::WIFI_FAILED);   // CONFIGURED -> ERROR

    TEST_ASSERT_EQUAL(NodeState::ERROR, sm.getState());

    // RETRY_TIMEOUT transitions to PAIRING, WIFI_CONNECTED transitions to CONFIGURED
    sm.processEvent(StateEvent::RETRY_TIMEOUT);   // ERROR -> PAIRING
    TEST_ASSERT_EQUAL(NodeState::PAIRING, sm.getState());

    sm.processEvent(StateEvent::WIFI_CONNECTED);  // PAIRING -> CONFIGURED
    TEST_ASSERT_EQUAL(NodeState::CONFIGURED, sm.getState());
}

void test_state_machine_state_names() {
    TEST_ASSERT_EQUAL_STRING("UNCONFIGURED", StateMachine::getStateName(NodeState::UNCONFIGURED));
    TEST_ASSERT_EQUAL_STRING("PAIRING", StateMachine::getStateName(NodeState::PAIRING));
    TEST_ASSERT_EQUAL_STRING("CONFIGURED", StateMachine::getStateName(NodeState::CONFIGURED));
    TEST_ASSERT_EQUAL_STRING("OPERATIONAL", StateMachine::getStateName(NodeState::OPERATIONAL));
    TEST_ASSERT_EQUAL_STRING("ERROR", StateMachine::getStateName(NodeState::ERROR));
}

void test_state_machine_retry_delay_increases() {
    StateMachine sm;
    sm.processEvent(StateEvent::CONFIG_FOUND);  // UNCONFIGURED -> CONFIGURED
    sm.processEvent(StateEvent::ERROR_OCCURRED);  // -> ERROR

    unsigned long delay1 = sm.getRetryDelay();

    sm.processEvent(StateEvent::RETRY_TIMEOUT);  // ERROR -> PAIRING
    sm.processEvent(StateEvent::ERROR_OCCURRED);  // PAIRING -> ERROR again

    unsigned long delay2 = sm.getRetryDelay();

    // Second delay should be greater (exponential backoff)
    TEST_ASSERT_GREATER_THAN(delay1, delay2);
}

// ============================================
// Discovery Protocol Tests (Story 1 & 8)
// ============================================

void test_discovery_client_configuration() {
    DiscoveryClient client;

    client.configure(5001, 5000);  // Port 5001, 5 second timeout

    // No direct way to test config, but it shouldn't crash
    TEST_ASSERT_TRUE(true);
}

void test_discovery_message_format() {
    // Verify the discovery message type constant
    TEST_ASSERT_EQUAL_STRING("MYIOTGRID_DISCOVER", config::DISCOVERY_MESSAGE_TYPE);
}

void test_discovery_response_message_format() {
    // Verify the discovery response message type constant
    TEST_ASSERT_EQUAL_STRING("MYIOTGRID_HUB", config::DISCOVERY_RESPONSE_TYPE);
}

void test_discovery_default_port() {
    TEST_ASSERT_EQUAL(5001, config::DISCOVERY_PORT);
}

void test_discovery_timeout_config() {
    TEST_ASSERT_EQUAL(5000, config::DISCOVERY_TIMEOUT_MS);
}

void test_discovery_retry_count() {
    TEST_ASSERT_EQUAL(3, config::DISCOVERY_RETRY_COUNT);
}

// ============================================
// Configuration Constants Tests
// ============================================

void test_env_variable_names() {
    TEST_ASSERT_EQUAL_STRING("HUB_HOST", config::ENV_HUB_HOST);
    TEST_ASSERT_EQUAL_STRING("HUB_PORT", config::ENV_HUB_PORT);
    TEST_ASSERT_EQUAL_STRING("HUB_PROTOCOL", config::ENV_HUB_PROTOCOL);
    TEST_ASSERT_EQUAL_STRING("DISCOVERY_PORT", config::ENV_DISCOVERY_PORT);
    TEST_ASSERT_EQUAL_STRING("DISCOVERY_ENABLED", config::ENV_DISCOVERY_ENABLED);
}

void test_default_hub_port() {
    TEST_ASSERT_EQUAL(5001, config::DEFAULT_HUB_PORT);
}

// ============================================
// Test Runner
// ============================================

void setUp(void) {
    // Called before each test
}

void tearDown(void) {
    // Called after each test
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Sensor Simulator Tests (Story 7)
    RUN_TEST(test_simulator_init_normal_profile);
    RUN_TEST(test_simulator_all_profiles);
    RUN_TEST(test_simulator_normal_temperature_range);
    RUN_TEST(test_simulator_winter_temperature_range);
    RUN_TEST(test_simulator_summer_temperature_range);
    RUN_TEST(test_simulator_storm_humidity_range);
    RUN_TEST(test_simulator_stress_extreme_ranges);
    RUN_TEST(test_simulator_profile_change);
    RUN_TEST(test_simulator_daily_cycle_toggle);
    RUN_TEST(test_simulator_simulated_hour);
    RUN_TEST(test_simulator_get_reading);
    RUN_TEST(test_simulator_reset_values);

    // State Machine Tests (Story 3)
    RUN_TEST(test_state_machine_initial_state);
    RUN_TEST(test_state_machine_unconfigured_to_pairing);
    RUN_TEST(test_state_machine_unconfigured_to_configured);
    RUN_TEST(test_state_machine_pairing_to_configured);
    RUN_TEST(test_state_machine_configured_to_operational);
    RUN_TEST(test_state_machine_wifi_failure);
    RUN_TEST(test_state_machine_error_recovery);
    RUN_TEST(test_state_machine_state_names);
    RUN_TEST(test_state_machine_retry_delay_increases);

    // Discovery Protocol Tests (Story 1 & 8)
    RUN_TEST(test_discovery_client_configuration);
    RUN_TEST(test_discovery_message_format);
    RUN_TEST(test_discovery_response_message_format);
    RUN_TEST(test_discovery_default_port);
    RUN_TEST(test_discovery_timeout_config);
    RUN_TEST(test_discovery_retry_count);

    // Configuration Constants Tests
    RUN_TEST(test_env_variable_names);
    RUN_TEST(test_default_hub_port);

    return UNITY_END();
}
