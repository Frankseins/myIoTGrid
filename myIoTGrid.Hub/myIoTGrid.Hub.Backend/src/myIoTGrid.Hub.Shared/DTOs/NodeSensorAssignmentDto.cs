namespace myIoTGrid.Hub.Shared.DTOs;

/// <summary>
/// DTO for NodeSensorAssignment information.
/// Hardware binding of a Sensor to a Node.
/// </summary>
public record NodeSensorAssignmentDto(
    Guid Id,
    Guid NodeId,
    string NodeName,
    Guid SensorId,
    string SensorName,
    Guid SensorTypeId,
    string SensorTypeCode,
    string SensorTypeName,
    int EndpointId,
    string? Alias,
    string? I2CAddressOverride,
    int? SdaPinOverride,
    int? SclPinOverride,
    int? OneWirePinOverride,
    int? AnalogPinOverride,
    int? DigitalPinOverride,
    int? TriggerPinOverride,
    int? EchoPinOverride,
    int? IntervalSecondsOverride,
    bool IsActive,
    DateTime? LastSeenAt,
    DateTime AssignedAt,
    EffectiveConfigDto EffectiveConfig
);

/// <summary>
/// DTO for creating a NodeSensorAssignment
/// </summary>
public record CreateNodeSensorAssignmentDto(
    Guid SensorId,
    int EndpointId,
    string? Alias = null,
    string? I2CAddressOverride = null,
    int? SdaPinOverride = null,
    int? SclPinOverride = null,
    int? OneWirePinOverride = null,
    int? AnalogPinOverride = null,
    int? DigitalPinOverride = null,
    int? TriggerPinOverride = null,
    int? EchoPinOverride = null,
    int? IntervalSecondsOverride = null
);

/// <summary>
/// DTO for updating a NodeSensorAssignment
/// </summary>
public record UpdateNodeSensorAssignmentDto(
    string? Alias = null,
    string? I2CAddressOverride = null,
    int? SdaPinOverride = null,
    int? SclPinOverride = null,
    int? OneWirePinOverride = null,
    int? AnalogPinOverride = null,
    int? DigitalPinOverride = null,
    int? TriggerPinOverride = null,
    int? EchoPinOverride = null,
    int? IntervalSecondsOverride = null,
    bool? IsActive = null
);

/// <summary>
/// DTO for effective configuration after inheritance resolution.
/// EffectiveValue = Assignment ?? Sensor ?? SensorType
/// </summary>
public record EffectiveConfigDto(
    int IntervalSeconds,
    string? I2CAddress,
    int? SdaPin,
    int? SclPin,
    int? OneWirePin,
    int? AnalogPin,
    int? DigitalPin,
    int? TriggerPin,
    int? EchoPin,
    double OffsetCorrection,
    double GainCorrection
);
