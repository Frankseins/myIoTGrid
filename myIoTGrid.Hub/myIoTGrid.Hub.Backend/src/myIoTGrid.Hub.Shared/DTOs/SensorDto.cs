namespace myIoTGrid.Hub.Shared.DTOs;

/// <summary>
/// DTO for Sensor instance information.
/// A concrete sensor with calibration settings.
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
    int? IntervalSecondsOverride,
    double OffsetCorrection,
    double GainCorrection,
    DateTime? LastCalibratedAt,
    string? CalibrationNotes,
    DateTime? CalibrationDueAt,
    IEnumerable<Guid> ActiveCapabilityIds,
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
    int? IntervalSecondsOverride = null,
    IEnumerable<Guid>? ActiveCapabilityIds = null
);

/// <summary>
/// DTO for updating a Sensor instance
/// </summary>
public record UpdateSensorDto(
    string? Name = null,
    string? Description = null,
    string? SerialNumber = null,
    int? IntervalSecondsOverride = null,
    IEnumerable<Guid>? ActiveCapabilityIds = null,
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
