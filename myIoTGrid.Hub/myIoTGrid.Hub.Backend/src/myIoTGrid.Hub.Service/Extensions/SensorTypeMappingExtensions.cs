using myIoTGrid.Hub.Domain.Entities;
using myIoTGrid.Hub.Domain.Enums;
using myIoTGrid.Hub.Shared.DTOs;
using myIoTGrid.Hub.Shared.Enums;

namespace myIoTGrid.Hub.Service.Extensions;

/// <summary>
/// Mapping Extensions for SensorType Entity.
/// Hardware sensor library with capabilities.
/// </summary>
public static class SensorTypeMappingExtensions
{
    /// <summary>
    /// Converts a SensorType entity to a SensorTypeDto
    /// </summary>
    public static SensorTypeDto ToDto(this SensorType sensorType)
    {
        return new SensorTypeDto(
            Id: sensorType.Id,
            Code: sensorType.Code,
            Name: sensorType.Name,
            Manufacturer: sensorType.Manufacturer,
            DatasheetUrl: sensorType.DatasheetUrl,
            Description: sensorType.Description,
            Protocol: (CommunicationProtocolDto)sensorType.Protocol,
            DefaultI2CAddress: sensorType.DefaultI2CAddress,
            DefaultSdaPin: sensorType.DefaultSdaPin,
            DefaultSclPin: sensorType.DefaultSclPin,
            DefaultOneWirePin: sensorType.DefaultOneWirePin,
            DefaultAnalogPin: sensorType.DefaultAnalogPin,
            DefaultDigitalPin: sensorType.DefaultDigitalPin,
            DefaultTriggerPin: sensorType.DefaultTriggerPin,
            DefaultEchoPin: sensorType.DefaultEchoPin,
            DefaultIntervalSeconds: sensorType.DefaultIntervalSeconds,
            MinIntervalSeconds: sensorType.MinIntervalSeconds,
            WarmupTimeMs: sensorType.WarmupTimeMs,
            DefaultOffsetCorrection: sensorType.DefaultOffsetCorrection,
            DefaultGainCorrection: sensorType.DefaultGainCorrection,
            Category: sensorType.Category,
            Icon: sensorType.Icon,
            Color: sensorType.Color,
            IsGlobal: sensorType.IsGlobal,
            IsActive: sensorType.IsActive,
            Capabilities: sensorType.Capabilities.Select(c => c.ToDto()),
            CreatedAt: sensorType.CreatedAt,
            UpdatedAt: sensorType.UpdatedAt
        );
    }

    /// <summary>
    /// Converts a SensorTypeCapability entity to a SensorTypeCapabilityDto
    /// </summary>
    public static SensorTypeCapabilityDto ToDto(this SensorTypeCapability capability)
    {
        return new SensorTypeCapabilityDto(
            Id: capability.Id,
            MeasurementType: capability.MeasurementType,
            DisplayName: capability.DisplayName,
            Unit: capability.Unit,
            MinValue: capability.MinValue,
            MaxValue: capability.MaxValue,
            Resolution: capability.Resolution,
            Accuracy: capability.Accuracy,
            MatterClusterId: capability.MatterClusterId,
            MatterClusterName: capability.MatterClusterName,
            SortOrder: capability.SortOrder,
            IsActive: capability.IsActive
        );
    }

    /// <summary>
    /// Converts a CreateSensorTypeDto to a SensorType entity
    /// </summary>
    public static SensorType ToEntity(this CreateSensorTypeDto dto)
    {
        var now = DateTime.UtcNow;
        var sensorType = new SensorType
        {
            Id = Guid.NewGuid(),
            Code = dto.Code.ToLowerInvariant(),
            Name = dto.Name,
            Manufacturer = dto.Manufacturer,
            DatasheetUrl = dto.DatasheetUrl,
            Description = dto.Description,
            Protocol = (CommunicationProtocol)dto.Protocol,
            DefaultI2CAddress = dto.DefaultI2CAddress,
            DefaultSdaPin = dto.DefaultSdaPin,
            DefaultSclPin = dto.DefaultSclPin,
            DefaultOneWirePin = dto.DefaultOneWirePin,
            DefaultAnalogPin = dto.DefaultAnalogPin,
            DefaultDigitalPin = dto.DefaultDigitalPin,
            DefaultTriggerPin = dto.DefaultTriggerPin,
            DefaultEchoPin = dto.DefaultEchoPin,
            DefaultIntervalSeconds = dto.DefaultIntervalSeconds,
            MinIntervalSeconds = dto.MinIntervalSeconds,
            WarmupTimeMs = dto.WarmupTimeMs,
            DefaultOffsetCorrection = dto.DefaultOffsetCorrection,
            DefaultGainCorrection = dto.DefaultGainCorrection,
            Category = dto.Category,
            Icon = dto.Icon,
            Color = dto.Color,
            IsGlobal = false,
            IsActive = true,
            CreatedAt = now,
            UpdatedAt = now
        };

        if (dto.Capabilities != null)
        {
            var sortOrder = 0;
            foreach (var cap in dto.Capabilities)
            {
                sensorType.Capabilities.Add(new SensorTypeCapability
                {
                    Id = Guid.NewGuid(),
                    MeasurementType = cap.MeasurementType.ToLowerInvariant(),
                    DisplayName = cap.DisplayName,
                    Unit = cap.Unit,
                    MinValue = cap.MinValue,
                    MaxValue = cap.MaxValue,
                    Resolution = cap.Resolution,
                    Accuracy = cap.Accuracy,
                    MatterClusterId = cap.MatterClusterId,
                    MatterClusterName = cap.MatterClusterName,
                    SortOrder = cap.SortOrder != 0 ? cap.SortOrder : sortOrder++,
                    IsActive = true
                });
            }
        }

        return sensorType;
    }

    /// <summary>
    /// Converts a list of SensorType Entities to DTOs
    /// </summary>
    public static IEnumerable<SensorTypeDto> ToDtos(this IEnumerable<SensorType> sensorTypes)
    {
        return sensorTypes.Select(st => st.ToDto());
    }
}
