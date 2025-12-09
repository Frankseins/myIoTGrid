using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Interfaces;

/// <summary>
/// Service Interface for Node Debug Log management (Sprint 8: Remote Debug System).
/// </summary>
public interface INodeDebugLogService
{
    /// <summary>
    /// Gets debug logs for a node with filtering and paging.
    /// </summary>
    Task<PaginatedResultDto<NodeDebugLogDto>> GetLogsAsync(DebugLogFilterDto filter, CancellationToken ct = default);

    /// <summary>
    /// Gets recent logs for a node (for live view).
    /// </summary>
    Task<IEnumerable<NodeDebugLogDto>> GetRecentLogsAsync(Guid nodeId, int count = 50, CancellationToken ct = default);

    /// <summary>
    /// Creates a batch of debug logs (from firmware upload).
    /// </summary>
    Task<int> CreateBatchAsync(string serialNumber, IEnumerable<CreateNodeDebugLogDto> logs, CancellationToken ct = default);

    /// <summary>
    /// Gets debug configuration for a node.
    /// </summary>
    Task<NodeDebugConfigurationDto?> GetDebugConfigurationAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>
    /// Gets debug configuration for a node by serial number.
    /// </summary>
    Task<NodeDebugConfigurationDto?> GetDebugConfigurationBySerialAsync(string serialNumber, CancellationToken ct = default);

    /// <summary>
    /// Sets debug level and remote logging for a node.
    /// </summary>
    Task<NodeDebugConfigurationDto?> SetDebugLevelAsync(Guid nodeId, SetNodeDebugLevelDto dto, CancellationToken ct = default);

    /// <summary>
    /// Sets debug level for a node by serial number.
    /// </summary>
    Task<NodeDebugConfigurationDto?> SetDebugLevelBySerialAsync(string serialNumber, SetNodeDebugLevelDto dto, CancellationToken ct = default);

    /// <summary>
    /// Gets error statistics for a node.
    /// </summary>
    Task<NodeErrorStatisticsDto?> GetErrorStatisticsAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>
    /// Gets error statistics for all nodes.
    /// </summary>
    Task<IEnumerable<NodeErrorStatisticsDto>> GetAllErrorStatisticsAsync(CancellationToken ct = default);

    /// <summary>
    /// Deletes debug logs older than specified date.
    /// </summary>
    Task<DebugLogCleanupResultDto> CleanupLogsAsync(DateTime before, CancellationToken ct = default);

    /// <summary>
    /// Deletes all debug logs for a node.
    /// </summary>
    Task<int> ClearLogsAsync(Guid nodeId, CancellationToken ct = default);
}
