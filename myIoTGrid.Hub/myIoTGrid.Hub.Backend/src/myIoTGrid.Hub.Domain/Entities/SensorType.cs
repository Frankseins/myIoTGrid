using myIoTGrid.Hub.Domain.Enums;
using myIoTGrid.Hub.Domain.Interfaces;

namespace myIoTGrid.Hub.Domain.Entities;

/// <summary>
/// Hardware sensor type definition (e.g., DHT22, BME280, DS18B20).
/// This is the "library" level - defines what a sensor CAN do.
/// Matter-konform: Defines available clusters/capabilities.
/// </summary>
public class SensorType : IEntity
{
    /// <summary>Primary key</summary>
    public Guid Id { get; set; }

    // === Identification ===

    /// <summary>Unique code (e.g., "dht22", "bme280", "ds18b20")</summary>
    public string Code { get; set; } = string.Empty;

    /// <summary>Display name (e.g., "DHT22 (AM2302)")</summary>
    public string Name { get; set; } = string.Empty;

    /// <summary>Manufacturer (e.g., "Bosch", "Aosong")</summary>
    public string? Manufacturer { get; set; }

    /// <summary>Link to datasheet</summary>
    public string? DatasheetUrl { get; set; }

    /// <summary>Description of the sensor</summary>
    public string? Description { get; set; }

    // === Communication Protocol ===

    /// <summary>How the sensor communicates (I2C, SPI, OneWire, etc.)</summary>
    public CommunicationProtocol Protocol { get; set; }

    // === Default Pin Configuration ===

    /// <summary>Default I2C address (e.g., "0x76")</summary>
    public string? DefaultI2CAddress { get; set; }

    /// <summary>Default SDA pin for I2C</summary>
    public int? DefaultSdaPin { get; set; }

    /// <summary>Default SCL pin for I2C</summary>
    public int? DefaultSclPin { get; set; }

    /// <summary>Default OneWire data pin</summary>
    public int? DefaultOneWirePin { get; set; }

    /// <summary>Default analog input pin</summary>
    public int? DefaultAnalogPin { get; set; }

    /// <summary>Default digital GPIO pin</summary>
    public int? DefaultDigitalPin { get; set; }

    /// <summary>Default trigger pin for ultrasonic sensors</summary>
    public int? DefaultTriggerPin { get; set; }

    /// <summary>Default echo pin for ultrasonic sensors</summary>
    public int? DefaultEchoPin { get; set; }

    // === Timing Configuration ===

    /// <summary>Default measurement interval in seconds</summary>
    public int DefaultIntervalSeconds { get; set; } = 60;

    /// <summary>Minimum allowed interval in seconds</summary>
    public int MinIntervalSeconds { get; set; } = 1;

    /// <summary>Warmup time in milliseconds before first reading</summary>
    public int WarmupTimeMs { get; set; }

    // === Factory Calibration ===

    /// <summary>Default offset correction from factory</summary>
    public double DefaultOffsetCorrection { get; set; }

    /// <summary>Default gain correction from factory</summary>
    public double DefaultGainCorrection { get; set; } = 1.0;

    // === Categorization ===

    /// <summary>Category (climate, water, air, soil, location, custom)</summary>
    public string Category { get; set; } = string.Empty;

    /// <summary>Material icon name for UI</summary>
    public string? Icon { get; set; }

    /// <summary>Hex color for UI (e.g., "#FF5722")</summary>
    public string? Color { get; set; }

    // === Flags ===

    /// <summary>Is this a global (cloud-synced) sensor type?</summary>
    public bool IsGlobal { get; set; } = true;

    /// <summary>Is this sensor type active?</summary>
    public bool IsActive { get; set; } = true;

    // === Timestamps ===

    /// <summary>Creation timestamp</summary>
    public DateTime CreatedAt { get; set; }

    /// <summary>Last update timestamp</summary>
    public DateTime UpdatedAt { get; set; }

    // === Navigation Properties ===

    /// <summary>Capabilities (measurement types this sensor can provide)</summary>
    public ICollection<SensorTypeCapability> Capabilities { get; set; } = new List<SensorTypeCapability>();

    /// <summary>Sensor instances of this type</summary>
    public ICollection<Sensor> Sensors { get; set; } = new List<Sensor>();
}
