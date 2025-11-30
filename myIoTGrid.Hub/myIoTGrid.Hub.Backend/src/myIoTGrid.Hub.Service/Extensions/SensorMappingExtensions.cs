using System.Text.Json;
using myIoTGrid.Hub.Domain.Entities;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Extensions;

/// <summary>
/// Mapping Extensions for Sensor Entity.
/// Concrete sensor instance with calibration settings.
/// </summary>
public static class SensorMappingExtensions
{
    /// <summary>
    /// Converts a Sensor entity to a SensorDto
    /// </summary>
    public static SensorDto ToDto(this Sensor sensor)
    {
        return new SensorDto(
            Id: sensor.Id,
            TenantId: sensor.TenantId,
            SensorTypeId: sensor.SensorTypeId,
            SensorTypeCode: sensor.SensorType?.Code ?? string.Empty,
            SensorTypeName: sensor.SensorType?.Name ?? string.Empty,
            Name: sensor.Name,
            Description: sensor.Description,
            SerialNumber: sensor.SerialNumber,
            IntervalSecondsOverride: sensor.IntervalSecondsOverride,
            OffsetCorrection: sensor.OffsetCorrection,
            GainCorrection: sensor.GainCorrection,
            LastCalibratedAt: sensor.LastCalibratedAt,
            CalibrationNotes: sensor.CalibrationNotes,
            CalibrationDueAt: sensor.CalibrationDueAt,
            ActiveCapabilityIds: sensor.GetActiveCapabilityIds(),
            IsActive: sensor.IsActive,
            CreatedAt: sensor.CreatedAt,
            UpdatedAt: sensor.UpdatedAt
        );
    }

    /// <summary>
    /// Gets active capability IDs from JSON
    /// </summary>
    public static IEnumerable<Guid> GetActiveCapabilityIds(this Sensor sensor)
    {
        if (string.IsNullOrEmpty(sensor.ActiveCapabilityIdsJson))
            return Enumerable.Empty<Guid>();

        try
        {
            return JsonSerializer.Deserialize<List<Guid>>(sensor.ActiveCapabilityIdsJson)
                   ?? Enumerable.Empty<Guid>();
        }
        catch
        {
            return Enumerable.Empty<Guid>();
        }
    }

    /// <summary>
    /// Sets active capability IDs as JSON
    /// </summary>
    public static void SetActiveCapabilityIds(this Sensor sensor, IEnumerable<Guid>? capabilityIds)
    {
        if (capabilityIds == null || !capabilityIds.Any())
        {
            sensor.ActiveCapabilityIdsJson = null;
        }
        else
        {
            sensor.ActiveCapabilityIdsJson = JsonSerializer.Serialize(capabilityIds.ToList());
        }
    }

    /// <summary>
    /// Converts a CreateSensorDto to a Sensor entity
    /// </summary>
    public static Sensor ToEntity(this CreateSensorDto dto, Guid tenantId)
    {
        var now = DateTime.UtcNow;
        var sensor = new Sensor
        {
            Id = Guid.NewGuid(),
            TenantId = tenantId,
            SensorTypeId = dto.SensorTypeId,
            Name = dto.Name,
            Description = dto.Description,
            SerialNumber = dto.SerialNumber,
            IntervalSecondsOverride = dto.IntervalSecondsOverride,
            OffsetCorrection = 0,
            GainCorrection = 1.0,
            IsActive = true,
            CreatedAt = now,
            UpdatedAt = now
        };

        sensor.SetActiveCapabilityIds(dto.ActiveCapabilityIds);
        return sensor;
    }

    /// <summary>
    /// Applies an UpdateSensorDto to a Sensor entity
    /// </summary>
    public static void ApplyUpdate(this Sensor sensor, UpdateSensorDto dto)
    {
        if (!string.IsNullOrEmpty(dto.Name))
            sensor.Name = dto.Name;

        if (dto.Description != null)
            sensor.Description = dto.Description;

        if (dto.SerialNumber != null)
            sensor.SerialNumber = dto.SerialNumber;

        if (dto.IntervalSecondsOverride.HasValue)
            sensor.IntervalSecondsOverride = dto.IntervalSecondsOverride;

        if (dto.ActiveCapabilityIds != null)
            sensor.SetActiveCapabilityIds(dto.ActiveCapabilityIds);

        if (dto.IsActive.HasValue)
            sensor.IsActive = dto.IsActive.Value;

        sensor.UpdatedAt = DateTime.UtcNow;
    }

    /// <summary>
    /// Applies calibration to a Sensor entity
    /// </summary>
    public static void ApplyCalibration(this Sensor sensor, CalibrateSensorDto dto)
    {
        sensor.OffsetCorrection = dto.OffsetCorrection;
        sensor.GainCorrection = dto.GainCorrection;
        sensor.CalibrationNotes = dto.CalibrationNotes;
        sensor.CalibrationDueAt = dto.CalibrationDueAt;
        sensor.LastCalibratedAt = DateTime.UtcNow;
        sensor.UpdatedAt = DateTime.UtcNow;
    }

    /// <summary>
    /// Converts a list of Sensor Entities to DTOs
    /// </summary>
    public static IEnumerable<SensorDto> ToDtos(this IEnumerable<Sensor> sensors)
    {
        return sensors.Select(s => s.ToDto());
    }
}
