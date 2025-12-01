namespace myIoTGrid.Hub.Shared.DTOs;

/// <summary>
/// DTO for Sensor instance information.
/// A concrete sensor with calibration settings and pin configuration overrides.
/// </summary>
public record SensorDto(
    Guid Id,
    Guid TenantId,
    Guid SensorTypeId,
    string SensorTypeCode,
    string SensorTypeName,
    string Name,
    string? Description,
    string? SerialNumber,

    // Interval Override
    int? IntervalSecondsOverride,

    // Pin Configuration Override
    string? I2CAddressOverride,
    int? SdaPinOverride,
    int? SclPinOverride,
    int? OneWirePinOverride,
    int? AnalogPinOverride,
    int? DigitalPinOverride,
    int? TriggerPinOverride,
    int? EchoPinOverride,

    // Calibration
    double OffsetCorrection,
    double GainCorrection,
    DateTime? LastCalibratedAt,
    string? CalibrationNotes,
    DateTime? CalibrationDueAt,

    // Capabilities
    IEnumerable<Guid> ActiveCapabilityIds,

    // Status
    bool IsActive,
    DateTime CreatedAt,
    DateTime UpdatedAt
);

/// <summary>
/// DTO for creating a Sensor instance
/// </summary>
public record CreateSensorDto(
    Guid SensorTypeId,
    string Name,
    string? Description = null,
    string? SerialNumber = null,

    // Interval Override
    int? IntervalSecondsOverride = null,

    // Pin Configuration Override
    string? I2CAddressOverride = null,
    int? SdaPinOverride = null,
    int? SclPinOverride = null,
    int? OneWirePinOverride = null,
    int? AnalogPinOverride = null,
    int? DigitalPinOverride = null,
    int? TriggerPinOverride = null,
    int? EchoPinOverride = null,

    // Calibration (initial values)
    double? OffsetCorrection = null,
    double? GainCorrection = null,

    // Capabilities
    IEnumerable<Guid>? ActiveCapabilityIds = null
);

/// <summary>
/// DTO for updating a Sensor instance
/// </summary>
public record UpdateSensorDto(
    string? Name = null,
    string? Description = null,
    string? SerialNumber = null,

    // Interval Override
    int? IntervalSecondsOverride = null,

    // Pin Configuration Override
    string? I2CAddressOverride = null,
    int? SdaPinOverride = null,
    int? SclPinOverride = null,
    int? OneWirePinOverride = null,
    int? AnalogPinOverride = null,
    int? DigitalPinOverride = null,
    int? TriggerPinOverride = null,
    int? EchoPinOverride = null,

    // Calibration
    double? OffsetCorrection = null,
    double? GainCorrection = null,
    string? CalibrationNotes = null,

    // Capabilities
    IEnumerable<Guid>? ActiveCapabilityIds = null,

    // Status
    bool? IsActive = null
);

/// <summary>
/// DTO for calibrating a Sensor
/// </summary>
public record CalibrateSensorDto(
    double OffsetCorrection,
    double GainCorrection = 1.0,
    string? CalibrationNotes = null,
    DateTime? CalibrationDueAt = null
);
