using myIoTGrid.Hub.Domain.Entities;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Extensions;

/// <summary>
/// Mapping Extensions for Reading Entity (Measurement).
/// Matter-konform: Entspricht einem Attribute Report.
/// </summary>
public static class ReadingMappingExtensions
{
    /// <summary>
    /// Converts a Reading entity to a ReadingDto
    /// </summary>
    public static ReadingDto ToDto(this Reading reading)
    {
        return new ReadingDto(
            Id: reading.Id,
            TenantId: reading.TenantId,
            NodeId: reading.NodeId,
            NodeName: reading.Node?.Name ?? string.Empty,
            AssignmentId: reading.AssignmentId,
            MeasurementType: reading.MeasurementType,
            RawValue: reading.RawValue,
            Value: reading.Value,
            Unit: reading.Unit,
            Timestamp: reading.Timestamp,
            Location: reading.Node?.Location?.ToDto(),
            IsSyncedToCloud: reading.IsSyncedToCloud
        );
    }

    /// <summary>
    /// Converts a CreateReadingDto to a Reading entity with calibration applied
    /// </summary>
    public static Reading ToEntity(
        this CreateReadingDto dto,
        Guid tenantId,
        Guid nodeId,
        Guid assignmentId,
        string unit,
        double calibratedValue)
    {
        return new Reading
        {
            TenantId = tenantId,
            NodeId = nodeId,
            AssignmentId = assignmentId,
            MeasurementType = dto.MeasurementType.ToLowerInvariant(),
            RawValue = dto.RawValue,
            Value = calibratedValue,
            Unit = unit,
            Timestamp = dto.Timestamp ?? DateTime.UtcNow,
            IsSyncedToCloud = false
        };
    }

    /// <summary>
    /// Converts a list of Reading Entities to DTOs
    /// </summary>
    public static IEnumerable<ReadingDto> ToDtos(this IEnumerable<Reading> readings)
    {
        return readings.Select(r => r.ToDto());
    }
}
