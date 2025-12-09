using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Interfaces;

/// <summary>
/// Service interface for Node Hardware Status management (Sprint 8).
/// </summary>
public interface INodeHardwareStatusService
{
    /// <summary>
    /// Reports hardware status from firmware.
    /// </summary>
    Task<NodeHardwareStatusDto?> ReportHardwareStatusAsync(ReportHardwareStatusDto dto, CancellationToken ct = default);

    /// <summary>
    /// Gets hardware status for a node by ID.
    /// </summary>
    Task<NodeHardwareStatusDto?> GetHardwareStatusAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>
    /// Gets hardware status for a node by serial number.
    /// </summary>
    Task<NodeHardwareStatusDto?> GetHardwareStatusBySerialAsync(string serialNumber, CancellationToken ct = default);
}
