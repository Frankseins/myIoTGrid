using myIoTGrid.Hub.Shared.Enums;

namespace myIoTGrid.Hub.Shared.DTOs;

/// <summary>
/// DTO for SensorType (hardware library) information.
/// Defines what a sensor CAN do.
/// </summary>
public record SensorTypeDto(
    Guid Id,
    string Code,
    string Name,
    string? Manufacturer,
    string? DatasheetUrl,
    string? Description,
    CommunicationProtocolDto Protocol,
    string? DefaultI2CAddress,
    int? DefaultSdaPin,
    int? DefaultSclPin,
    int? DefaultOneWirePin,
    int? DefaultAnalogPin,
    int? DefaultDigitalPin,
    int? DefaultTriggerPin,
    int? DefaultEchoPin,
    int DefaultIntervalSeconds,
    int MinIntervalSeconds,
    int WarmupTimeMs,
    double DefaultOffsetCorrection,
    double DefaultGainCorrection,
    string Category,
    string? Icon,
    string? Color,
    bool IsGlobal,
    bool IsActive,
    IEnumerable<SensorTypeCapabilityDto> Capabilities,
    DateTime CreatedAt,
    DateTime UpdatedAt
);

/// <summary>
/// DTO for SensorTypeCapability (measurement type).
/// Defines a single measurement capability of a sensor.
/// </summary>
public record SensorTypeCapabilityDto(
    Guid Id,
    string MeasurementType,
    string DisplayName,
    string Unit,
    double? MinValue,
    double? MaxValue,
    double Resolution,
    double Accuracy,
    uint? MatterClusterId,
    string? MatterClusterName,
    int SortOrder,
    bool IsActive
);

/// <summary>
/// DTO for creating a SensorType
/// </summary>
public record CreateSensorTypeDto(
    string Code,
    string Name,
    CommunicationProtocolDto Protocol,
    string? Manufacturer = null,
    string? DatasheetUrl = null,
    string? Description = null,
    string? DefaultI2CAddress = null,
    int? DefaultSdaPin = null,
    int? DefaultSclPin = null,
    int? DefaultOneWirePin = null,
    int? DefaultAnalogPin = null,
    int? DefaultDigitalPin = null,
    int? DefaultTriggerPin = null,
    int? DefaultEchoPin = null,
    int DefaultIntervalSeconds = 60,
    int MinIntervalSeconds = 1,
    int WarmupTimeMs = 0,
    double DefaultOffsetCorrection = 0,
    double DefaultGainCorrection = 1.0,
    string Category = "custom",
    string? Icon = null,
    string? Color = null,
    IEnumerable<CreateSensorTypeCapabilityDto>? Capabilities = null
);

/// <summary>
/// DTO for creating a SensorTypeCapability
/// </summary>
public record CreateSensorTypeCapabilityDto(
    string MeasurementType,
    string DisplayName,
    string Unit,
    double? MinValue = null,
    double? MaxValue = null,
    double Resolution = 0.01,
    double Accuracy = 0.5,
    uint? MatterClusterId = null,
    string? MatterClusterName = null,
    int SortOrder = 0
);

/// <summary>
/// DTO for updating a SensorType
/// </summary>
public record UpdateSensorTypeDto(
    string? Name = null,
    string? Manufacturer = null,
    string? DatasheetUrl = null,
    string? Description = null,
    string? DefaultI2CAddress = null,
    int? DefaultSdaPin = null,
    int? DefaultSclPin = null,
    int? DefaultOneWirePin = null,
    int? DefaultAnalogPin = null,
    int? DefaultDigitalPin = null,
    int? DefaultTriggerPin = null,
    int? DefaultEchoPin = null,
    int? DefaultIntervalSeconds = null,
    int? MinIntervalSeconds = null,
    int? WarmupTimeMs = null,
    double? DefaultOffsetCorrection = null,
    double? DefaultGainCorrection = null,
    string? Category = null,
    string? Icon = null,
    string? Color = null,
    bool? IsActive = null
);
