using System.Text.Json;
using myIoTGrid.Hub.Domain.Entities;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Extensions;

/// <summary>
/// Mapping Extensions for Sensor Entity.
/// Concrete sensor instance with calibration settings and pin configuration overrides.
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

            // Interval Override
            IntervalSecondsOverride: sensor.IntervalSecondsOverride,

            // Pin Configuration Override
            I2CAddressOverride: sensor.I2CAddressOverride,
            SdaPinOverride: sensor.SdaPinOverride,
            SclPinOverride: sensor.SclPinOverride,
            OneWirePinOverride: sensor.OneWirePinOverride,
            AnalogPinOverride: sensor.AnalogPinOverride,
            DigitalPinOverride: sensor.DigitalPinOverride,
            TriggerPinOverride: sensor.TriggerPinOverride,
            EchoPinOverride: sensor.EchoPinOverride,

            // Calibration
            OffsetCorrection: sensor.OffsetCorrection,
            GainCorrection: sensor.GainCorrection,
            LastCalibratedAt: sensor.LastCalibratedAt,
            CalibrationNotes: sensor.CalibrationNotes,
            CalibrationDueAt: sensor.CalibrationDueAt,

            // Capabilities
            ActiveCapabilityIds: sensor.GetActiveCapabilityIds(),

            // Status
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

            // Interval Override
            IntervalSecondsOverride = dto.IntervalSecondsOverride,

            // Pin Configuration Override
            I2CAddressOverride = dto.I2CAddressOverride,
            SdaPinOverride = dto.SdaPinOverride,
            SclPinOverride = dto.SclPinOverride,
            OneWirePinOverride = dto.OneWirePinOverride,
            AnalogPinOverride = dto.AnalogPinOverride,
            DigitalPinOverride = dto.DigitalPinOverride,
            TriggerPinOverride = dto.TriggerPinOverride,
            EchoPinOverride = dto.EchoPinOverride,

            // Calibration (use provided values or defaults)
            OffsetCorrection = dto.OffsetCorrection ?? 0,
            GainCorrection = dto.GainCorrection ?? 1.0,

            // Status
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

        // Interval Override
        if (dto.IntervalSecondsOverride.HasValue)
            sensor.IntervalSecondsOverride = dto.IntervalSecondsOverride;

        // Pin Configuration Override
        if (dto.I2CAddressOverride != null)
            sensor.I2CAddressOverride = dto.I2CAddressOverride;

        if (dto.SdaPinOverride.HasValue)
            sensor.SdaPinOverride = dto.SdaPinOverride;

        if (dto.SclPinOverride.HasValue)
            sensor.SclPinOverride = dto.SclPinOverride;

        if (dto.OneWirePinOverride.HasValue)
            sensor.OneWirePinOverride = dto.OneWirePinOverride;

        if (dto.AnalogPinOverride.HasValue)
            sensor.AnalogPinOverride = dto.AnalogPinOverride;

        if (dto.DigitalPinOverride.HasValue)
            sensor.DigitalPinOverride = dto.DigitalPinOverride;

        if (dto.TriggerPinOverride.HasValue)
            sensor.TriggerPinOverride = dto.TriggerPinOverride;

        if (dto.EchoPinOverride.HasValue)
            sensor.EchoPinOverride = dto.EchoPinOverride;

        // Calibration
        if (dto.OffsetCorrection.HasValue)
            sensor.OffsetCorrection = dto.OffsetCorrection.Value;

        if (dto.GainCorrection.HasValue)
            sensor.GainCorrection = dto.GainCorrection.Value;

        if (dto.CalibrationNotes != null)
            sensor.CalibrationNotes = dto.CalibrationNotes;

        // Capabilities
        if (dto.ActiveCapabilityIds != null)
            sensor.SetActiveCapabilityIds(dto.ActiveCapabilityIds);

        // Status
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
