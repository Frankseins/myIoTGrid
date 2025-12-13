using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.SignalR;
using Microsoft.Extensions.Logging;
using myIoTGrid.Hub.Interface.Hubs;
using myIoTGrid.Shared.Common.DTOs;
using myIoTGrid.Shared.Contracts.Services;

namespace myIoTGrid.Hub.Interface.Controllers;

/// <summary>
/// Controller for Hub-to-Cloud synchronization.
/// Provides endpoints for manual sync operations.
/// </summary>
[ApiController]
[Route("api/[controller]")]
public class SyncController : ControllerBase
{
    private readonly IManualCloudSyncService _syncService;
    private readonly IHubContext<SyncProgressHub> _progressHub;
    private readonly ILogger<SyncController> _logger;

    public SyncController(
        IManualCloudSyncService syncService,
        IHubContext<SyncProgressHub> progressHub,
        ILogger<SyncController> logger)
    {
        _syncService = syncService;
        _progressHub = progressHub;
        _logger = logger;
    }

    /// <summary>
    /// Start manual sync for a specific node.
    /// Runs in background and reports progress via SignalR.
    /// </summary>
    [HttpPost("nodes/{nodeId:guid}")]
    [ProducesResponseType(typeof(StartSyncResponseDto), StatusCodes.Status202Accepted)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<IActionResult> SyncNode(Guid nodeId, CancellationToken ct)
    {
        var jobId = Guid.NewGuid();

        // Check if sync is already in progress
        if (await _syncService.IsSyncInProgressAsync(nodeId, ct))
        {
            return BadRequest(new { error = "A sync is already in progress for this node" });
        }

        _logger.LogInformation("Starting manual sync for node {NodeId} (Job: {JobId})", nodeId, jobId);

        // Start sync in background
        _ = Task.Run(async () =>
        {
            var progress = new Progress<SyncProgressDto>(async p =>
            {
                // Send progress via SignalR to node-specific group and all-sync group
                await _progressHub.Clients.Group($"sync:{nodeId}").SendAsync("SyncProgress", jobId, nodeId, p);
                await _progressHub.Clients.Group("sync:all").SendAsync("SyncProgress", jobId, nodeId, p);
            });

            var result = await _syncService.SyncNodeToCloudAsync(nodeId, progress, CancellationToken.None);

            // Send final result
            await _progressHub.Clients.Group($"sync:{nodeId}").SendAsync("SyncComplete", jobId, nodeId, result);
            await _progressHub.Clients.Group("sync:all").SendAsync("SyncComplete", jobId, nodeId, result);

        }, CancellationToken.None);

        return Accepted(new StartSyncResponseDto(
            JobId: jobId,
            Message: $"Sync started for node {nodeId}. Subscribe to SignalR for progress updates."
        ));
    }

    /// <summary>
    /// Cancel an in-progress sync for a node.
    /// </summary>
    [HttpPost("nodes/{nodeId:guid}/cancel")]
    [ProducesResponseType(StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<IActionResult> CancelSync(Guid nodeId, CancellationToken ct)
    {
        var cancelled = await _syncService.CancelSyncAsync(nodeId, ct);

        if (!cancelled)
        {
            return NotFound(new { error = "No sync in progress for this node" });
        }

        _logger.LogInformation("Sync cancelled for node {NodeId}", nodeId);

        return Ok(new { message = "Sync cancelled" });
    }

    /// <summary>
    /// Get sync status for a specific node.
    /// </summary>
    [HttpGet("nodes/{nodeId:guid}/status")]
    [ProducesResponseType(typeof(SyncStatusDto), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<IActionResult> GetSyncStatus(Guid nodeId, CancellationToken ct)
    {
        var status = await _syncService.GetSyncStatusAsync(nodeId, ct);

        if (status == null)
        {
            return NotFound(new { error = $"Node not found: {nodeId}" });
        }

        return Ok(status);
    }

    /// <summary>
    /// Get sync status for all nodes.
    /// </summary>
    [HttpGet("status")]
    [ProducesResponseType(typeof(IEnumerable<SyncStatusDto>), StatusCodes.Status200OK)]
    public async Task<IActionResult> GetAllSyncStatus(CancellationToken ct)
    {
        var statusList = await _syncService.GetAllSyncStatusAsync(ct);
        return Ok(statusList);
    }

    /// <summary>
    /// Get sync summary across all nodes.
    /// </summary>
    [HttpGet("summary")]
    [ProducesResponseType(typeof(SyncSummaryDto), StatusCodes.Status200OK)]
    public async Task<IActionResult> GetSyncSummary(CancellationToken ct)
    {
        var summary = await _syncService.GetSyncSummaryAsync(ct);
        return Ok(summary);
    }

    /// <summary>
    /// Get sync history for a specific node.
    /// </summary>
    [HttpGet("nodes/{nodeId:guid}/history")]
    [ProducesResponseType(typeof(IEnumerable<SyncHistoryEntryDto>), StatusCodes.Status200OK)]
    public async Task<IActionResult> GetSyncHistory(
        Guid nodeId,
        [FromQuery] int limit = 10,
        CancellationToken ct = default)
    {
        var history = await _syncService.GetSyncHistoryAsync(nodeId, limit, ct);
        return Ok(history);
    }

    /// <summary>
    /// Get count of unsynced readings for a specific node.
    /// </summary>
    [HttpGet("nodes/{nodeId:guid}/unsynced-count")]
    [ProducesResponseType(typeof(object), StatusCodes.Status200OK)]
    public async Task<IActionResult> GetUnsyncedReadingsCount(Guid nodeId, CancellationToken ct)
    {
        var count = await _syncService.GetUnsyncedReadingsCountAsync(nodeId, ct);
        return Ok(new { nodeId, unsyncedCount = count });
    }
}
