using myIoTGrid.Hub.Domain.Interfaces;

namespace myIoTGrid.Hub.Domain.Entities;

/// <summary>
/// Concrete sensor instance with calibration settings.
/// This is the "instance" level - a specific physical sensor with its configuration.
/// Can override SensorType defaults (interval, calibration).
/// </summary>
public class Sensor : ITenantEntity
{
    /// <summary>Primary key</summary>
    public Guid Id { get; set; }

    /// <summary>Tenant ID for multi-tenant support</summary>
    public Guid TenantId { get; set; }

    /// <summary>FK to the SensorType (hardware definition)</summary>
    public Guid SensorTypeId { get; set; }

    /// <summary>Display name (e.g., "Temperatursensor Wohnzimmer")</summary>
    public string Name { get; set; } = string.Empty;

    /// <summary>Optional description</summary>
    public string? Description { get; set; }

    /// <summary>Serial number of this physical sensor</summary>
    public string? SerialNumber { get; set; }

    // === Interval Override ===

    /// <summary>Override measurement interval (null = use SensorType default)</summary>
    public int? IntervalSecondsOverride { get; set; }

    // === Pin Configuration Override (null = use SensorType default) ===

    /// <summary>Override I2C address (e.g., "0x77" for second BME280 on bus)</summary>
    public string? I2CAddressOverride { get; set; }

    /// <summary>Override SDA pin for I2C</summary>
    public int? SdaPinOverride { get; set; }

    /// <summary>Override SCL pin for I2C</summary>
    public int? SclPinOverride { get; set; }

    /// <summary>Override OneWire data pin</summary>
    public int? OneWirePinOverride { get; set; }

    /// <summary>Override analog input pin</summary>
    public int? AnalogPinOverride { get; set; }

    /// <summary>Override digital GPIO pin</summary>
    public int? DigitalPinOverride { get; set; }

    /// <summary>Override trigger pin for ultrasonic sensors</summary>
    public int? TriggerPinOverride { get; set; }

    /// <summary>Override echo pin for ultrasonic sensors</summary>
    public int? EchoPinOverride { get; set; }

    // === Calibration ===

    /// <summary>Offset correction applied to raw values</summary>
    public double OffsetCorrection { get; set; }

    /// <summary>Gain/multiplier correction applied to raw values</summary>
    public double GainCorrection { get; set; } = 1.0;

    /// <summary>When this sensor was last calibrated</summary>
    public DateTime? LastCalibratedAt { get; set; }

    /// <summary>Notes about the calibration process</summary>
    public string? CalibrationNotes { get; set; }

    /// <summary>When the next calibration is due</summary>
    public DateTime? CalibrationDueAt { get; set; }

    // === Active Capabilities ===

    /// <summary>JSON array of active capability IDs (null = all capabilities active)</summary>
    public string? ActiveCapabilityIdsJson { get; set; }

    // === Status ===

    /// <summary>Is this sensor active?</summary>
    public bool IsActive { get; set; } = true;

    /// <summary>Creation timestamp</summary>
    public DateTime CreatedAt { get; set; }

    /// <summary>Last update timestamp</summary>
    public DateTime UpdatedAt { get; set; }

    // === Navigation Properties ===

    /// <summary>Tenant this sensor belongs to</summary>
    public Tenant Tenant { get; set; } = null!;

    /// <summary>Hardware type definition</summary>
    public SensorType SensorType { get; set; } = null!;

    /// <summary>Node assignments (where this sensor is installed)</summary>
    public ICollection<NodeSensorAssignment> NodeAssignments { get; set; } = new List<NodeSensorAssignment>();
}
