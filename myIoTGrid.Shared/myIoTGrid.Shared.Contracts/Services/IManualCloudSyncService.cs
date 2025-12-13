using myIoTGrid.Shared.Common.DTOs;

namespace myIoTGrid.Shared.Contracts.Services;

/// <summary>
/// Service interface for manual Hub-to-Cloud synchronization.
/// Handles one-way sync of Nodes, Sensors, and Readings to Grid.Cloud.
/// </summary>
public interface IManualCloudSyncService
{
    /// <summary>
    /// Synchronizes a Node to the Cloud.
    /// This is the main entry point for manual sync.
    /// 1. Syncs Node configuration
    /// 2. Syncs all Sensor configurations
    /// 3. Uploads all unsynced Readings
    /// </summary>
    /// <param name="nodeId">Node ID to sync</param>
    /// <param name="progress">Optional progress reporter for UI updates</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Sync result with statistics</returns>
    Task<SyncResultDto> SyncNodeToCloudAsync(
        Guid nodeId,
        IProgress<SyncProgressDto>? progress = null,
        CancellationToken ct = default);

    /// <summary>
    /// Syncs only the Node configuration to Cloud.
    /// </summary>
    /// <param name="nodeId">Node ID to sync</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Result with CloudId and action taken</returns>
    Task<SyncNodeResponseDto> SyncNodeConfigAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>
    /// Syncs all Sensor configurations for a Node to Cloud.
    /// </summary>
    /// <param name="nodeId">Node ID</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Result with sensor sync statistics</returns>
    Task<SyncSensorsResponseDto> SyncSensorsConfigAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>
    /// Uploads all unsynced Readings for a Node to Cloud.
    /// </summary>
    /// <param name="nodeId">Node ID</param>
    /// <param name="progress">Optional progress reporter</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Number of readings synced</returns>
    Task<int> SyncReadingsAsync(
        Guid nodeId,
        IProgress<SyncProgressDto>? progress = null,
        CancellationToken ct = default);

    // === Status & History ===

    /// <summary>
    /// Gets the current sync status for a Node.
    /// </summary>
    Task<SyncStatusDto?> GetSyncStatusAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>
    /// Gets sync status for all Nodes.
    /// </summary>
    Task<IEnumerable<SyncStatusDto>> GetAllSyncStatusAsync(CancellationToken ct = default);

    /// <summary>
    /// Gets a summary of sync status across all Nodes.
    /// </summary>
    Task<SyncSummaryDto> GetSyncSummaryAsync(CancellationToken ct = default);

    /// <summary>
    /// Gets sync history for a Node.
    /// </summary>
    /// <param name="nodeId">Node ID</param>
    /// <param name="limit">Maximum entries to return</param>
    /// <param name="ct">Cancellation token</param>
    Task<IEnumerable<SyncHistoryEntryDto>> GetSyncHistoryAsync(
        Guid nodeId,
        int limit = 10,
        CancellationToken ct = default);

    /// <summary>
    /// Gets the count of unsynced readings for a Node.
    /// </summary>
    Task<int> GetUnsyncedReadingsCountAsync(Guid nodeId, CancellationToken ct = default);

    // === Sync State Management ===

    /// <summary>
    /// Checks if a sync is currently in progress for a Node.
    /// </summary>
    Task<bool> IsSyncInProgressAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>
    /// Cancels an in-progress sync operation.
    /// </summary>
    Task<bool> CancelSyncAsync(Guid nodeId, CancellationToken ct = default);
}
