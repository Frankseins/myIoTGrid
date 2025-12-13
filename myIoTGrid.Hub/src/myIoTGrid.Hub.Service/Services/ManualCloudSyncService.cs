using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using myIoTGrid.Hub.Infrastructure.Data;
using myIoTGrid.Shared.Common.DTOs;
using myIoTGrid.Shared.Common.Entities;
using myIoTGrid.Shared.Common.Options;
using myIoTGrid.Shared.Contracts.Services;

namespace myIoTGrid.Hub.Service.Services;

/// <summary>
/// Service for manual Hub-to-Cloud synchronization.
/// Orchestrates the 3-step sync process: Node → Sensors → Readings
/// </summary>
public class ManualCloudSyncService : IManualCloudSyncService
{
    private readonly HubDbContext _context;
    private readonly ICloudApiClient _cloudApiClient;
    private readonly IReadingService _readingService;
    private readonly IHubService _hubService;
    private readonly CloudApiOptions _options;
    private readonly ILogger<ManualCloudSyncService> _logger;

    // Track active sync operations
    private static readonly Dictionary<Guid, CancellationTokenSource> _activeSyncs = new();
    private static readonly object _syncLock = new();

    public ManualCloudSyncService(
        HubDbContext context,
        ICloudApiClient cloudApiClient,
        IReadingService readingService,
        IHubService hubService,
        IOptions<CloudApiOptions> options,
        ILogger<ManualCloudSyncService> logger)
    {
        _context = context;
        _cloudApiClient = cloudApiClient;
        _readingService = readingService;
        _hubService = hubService;
        _options = options.Value;
        _logger = logger;
    }

    /// <inheritdoc />
    public async Task<SyncResultDto> SyncNodeToCloudAsync(
        Guid nodeId,
        IProgress<SyncProgressDto>? progress = null,
        CancellationToken ct = default)
    {
        var result = new SyncResultDto { NodeId = nodeId };
        var startTime = DateTime.UtcNow;
        var jobId = Guid.NewGuid();

        // Check if Cloud API is configured
        if (!_cloudApiClient.IsConfigured)
        {
            result.Success = false;
            result.Error = "Cloud API is not configured. Please set the API key in settings.";
            return result;
        }

        // Get node
        var node = await _context.Nodes
            .Include(n => n.Hub)
            .Include(n => n.SyncState)
            .FirstOrDefaultAsync(n => n.Id == nodeId, ct);

        if (node == null)
        {
            result.Success = false;
            result.Error = $"Node not found: {nodeId}";
            return result;
        }

        // Check if sync is already in progress
        if (await IsSyncInProgressAsync(nodeId, ct))
        {
            result.Success = false;
            result.Error = "A sync is already in progress for this node";
            return result;
        }

        // Create sync state if not exists
        var syncState = node.SyncState ?? await CreateSyncStateAsync(node, ct);

        // Create history entry
        var historyEntry = new SyncHistoryEntry
        {
            Id = Guid.NewGuid(),
            NodeId = nodeId,
            JobId = jobId,
            StartedAt = startTime
        };
        _context.SyncHistoryEntries.Add(historyEntry);

        // Mark sync as in progress
        syncState.IsSyncing = true;
        syncState.CurrentJobId = jobId;
        syncState.TotalSyncs++;
        await _context.SaveChangesAsync(ct);

        // Create linked cancellation token
        using var cts = CancellationTokenSource.CreateLinkedTokenSource(ct);
        lock (_syncLock)
        {
            _activeSyncs[nodeId] = cts;
        }

        try
        {
            // Step 1: Sync Node configuration
            progress?.Report(new SyncProgressDto(
                Stage: "Node",
                Message: "Syncing node configuration...",
                PercentComplete: 10));

            var nodeResult = await SyncNodeConfigAsync(nodeId, cts.Token);
            result.NodeAction = nodeResult.WasCreated ? "Created" : "Updated";

            // Update sync state with Cloud ID
            syncState.CloudNodeId = nodeResult.CloudId;

            // Step 2: Sync Sensors configuration
            progress?.Report(new SyncProgressDto(
                Stage: "Sensors",
                Message: "Syncing sensor configurations...",
                PercentComplete: 30));

            var sensorsResult = await SyncSensorsConfigAsync(nodeId, cts.Token);
            result.SensorsCreated = sensorsResult.Sensors.Count(s => s.WasCreated);
            result.SensorsUpdated = sensorsResult.Sensors.Length - result.SensorsCreated;

            // Step 3: Upload Readings
            progress?.Report(new SyncProgressDto(
                Stage: "Readings",
                Message: "Uploading readings...",
                PercentComplete: 50));

            var readingsCount = await SyncReadingsAsync(nodeId, progress, cts.Token);
            result.ReadingsSynced = readingsCount;

            // Success!
            result.Success = true;
            result.Duration = DateTime.UtcNow - startTime;

            // Update sync state
            syncState.LastSyncAt = DateTime.UtcNow;
            syncState.LastSyncSuccess = true;
            syncState.LastSyncError = null;
            syncState.LastSyncDuration = result.Duration;
            syncState.SuccessfulSyncs++;
            syncState.TotalReadingsSynced += readingsCount;

            // Update history entry
            historyEntry.Success = true;
            historyEntry.CompletedAt = DateTime.UtcNow;
            historyEntry.Duration = result.Duration;
            historyEntry.NodeAction = result.NodeAction;
            historyEntry.SensorsCreated = result.SensorsCreated;
            historyEntry.SensorsUpdated = result.SensorsUpdated;
            historyEntry.ReadingsSynced = result.ReadingsSynced;

            progress?.Report(new SyncProgressDto(
                Stage: "Complete",
                Message: $"Sync complete: {result.ReadingsSynced} readings uploaded",
                PercentComplete: 100));

            _logger.LogInformation(
                "Sync completed for node {NodeId}: {NodeAction}, {SensorsCreated} sensors created, " +
                "{SensorsUpdated} updated, {ReadingsSynced} readings uploaded in {Duration}s",
                nodeId, result.NodeAction, result.SensorsCreated,
                result.SensorsUpdated, result.ReadingsSynced, result.Duration.TotalSeconds);
        }
        catch (OperationCanceledException)
        {
            result.Success = false;
            result.Error = "Sync was cancelled";
            result.Duration = DateTime.UtcNow - startTime;

            syncState.LastSyncSuccess = false;
            syncState.LastSyncError = "Sync was cancelled";
            syncState.FailedSyncs++;

            historyEntry.Success = false;
            historyEntry.Error = "Sync was cancelled";
            historyEntry.CompletedAt = DateTime.UtcNow;
            historyEntry.Duration = result.Duration;

            _logger.LogWarning("Sync cancelled for node {NodeId}", nodeId);
        }
        catch (Exception ex)
        {
            result.Success = false;
            result.Error = ex.Message;
            result.Duration = DateTime.UtcNow - startTime;

            syncState.LastSyncSuccess = false;
            syncState.LastSyncError = ex.Message;
            syncState.FailedSyncs++;

            historyEntry.Success = false;
            historyEntry.Error = ex.Message;
            historyEntry.CompletedAt = DateTime.UtcNow;
            historyEntry.Duration = result.Duration;

            _logger.LogError(ex, "Sync failed for node {NodeId}: {Error}", nodeId, ex.Message);
        }
        finally
        {
            // Mark sync as complete
            syncState.IsSyncing = false;
            syncState.CurrentJobId = null;
            syncState.UpdatedAt = DateTime.UtcNow;
            await _context.SaveChangesAsync(CancellationToken.None);

            lock (_syncLock)
            {
                _activeSyncs.Remove(nodeId);
            }
        }

        return result;
    }

    /// <inheritdoc />
    public async Task<SyncNodeResponseDto> SyncNodeConfigAsync(Guid nodeId, CancellationToken ct = default)
    {
        var node = await _context.Nodes
            .Include(n => n.Hub)
            .Include(n => n.Location)
            .FirstOrDefaultAsync(n => n.Id == nodeId, ct);

        if (node == null)
            throw new InvalidOperationException($"Node not found: {nodeId}");

        var hub = await _hubService.GetCurrentHubAsync(ct);

        var dto = new SyncNodeDto(
            HubId: hub.Id,
            LocalNodeId: node.Id,
            NodeId: node.NodeId,
            Name: node.Name,
            Location: node.Location?.Name,
            Protocol: (ProtocolDto)node.Protocol,
            FirmwareVersion: node.FirmwareVersion,
            Metadata: new Dictionary<string, string>
            {
                ["MacAddress"] = node.MacAddress,
                ["IsSimulation"] = node.IsSimulation.ToString(),
                ["StorageMode"] = node.StorageMode.ToString()
            }
        );

        return await _cloudApiClient.UpsertNodeAsync(dto, ct);
    }

    /// <inheritdoc />
    public async Task<SyncSensorsResponseDto> SyncSensorsConfigAsync(Guid nodeId, CancellationToken ct = default)
    {
        var node = await _context.Nodes
            .Include(n => n.SyncState)
            .FirstOrDefaultAsync(n => n.Id == nodeId, ct);

        if (node == null)
            throw new InvalidOperationException($"Node not found: {nodeId}");

        if (node.SyncState?.CloudNodeId == null)
            throw new InvalidOperationException("Node must be synced first to get CloudNodeId");

        var nodeCloudId = node.SyncState.CloudNodeId.Value;

        // Get all sensor assignments for this node
        var assignments = await _context.NodeSensorAssignments
            .Include(a => a.Sensor)
                .ThenInclude(s => s.Capabilities)
            .Where(a => a.NodeId == nodeId && a.IsActive)
            .ToListAsync(ct);

        // Build sync DTOs - one per capability (measurement type)
        var sensorDtos = new List<SyncSensorDto>();

        foreach (var assignment in assignments)
        {
            foreach (var capability in assignment.Sensor.Capabilities)
            {
                sensorDtos.Add(new SyncSensorDto(
                    LocalSensorId: assignment.Id,
                    NodeCloudId: nodeCloudId,
                    SensorCode: assignment.Sensor.Code,
                    Name: assignment.Alias ?? assignment.Sensor.Name,
                    MeasurementType: capability.MeasurementType,
                    Unit: capability.Unit,
                    SamplingIntervalSeconds: assignment.IntervalSecondsOverride ?? assignment.Sensor.IntervalSeconds,
                    Threshold: null,
                    IsEnabled: assignment.IsActive
                ));
            }
        }

        var dto = new SyncSensorsDto(
            NodeCloudId: nodeCloudId,
            Sensors: sensorDtos.ToArray()
        );

        var response = await _cloudApiClient.UpsertSensorsAsync(dto, ct);

        // Update CloudSensorId on assignments
        foreach (var sensorResult in response.Sensors)
        {
            var assignment = assignments.FirstOrDefault(a => a.Id == sensorResult.LocalSensorId);
            if (assignment != null)
            {
                assignment.CloudSensorId = sensorResult.CloudId;
                assignment.LastSyncedAt = DateTime.UtcNow;
            }
        }

        await _context.SaveChangesAsync(ct);

        return response;
    }

    /// <inheritdoc />
    public async Task<int> SyncReadingsAsync(
        Guid nodeId,
        IProgress<SyncProgressDto>? progress = null,
        CancellationToken ct = default)
    {
        var node = await _context.Nodes
            .Include(n => n.SyncState)
            .FirstOrDefaultAsync(n => n.Id == nodeId, ct);

        if (node == null)
            throw new InvalidOperationException($"Node not found: {nodeId}");

        if (node.SyncState?.CloudNodeId == null)
            throw new InvalidOperationException("Node must be synced first to get CloudNodeId");

        var nodeCloudId = node.SyncState.CloudNodeId.Value;
        var batchSize = _options.ReadingBatchSize;
        var totalSynced = 0;
        var offset = 0;

        // Get total count for progress reporting
        var totalCount = await _readingService.GetUnsyncedCountByNodeAsync(nodeId, ct);

        if (totalCount == 0)
        {
            _logger.LogInformation("No unsynced readings for node {NodeId}", nodeId);
            return 0;
        }

        // Build CloudSensorId lookup
        var assignmentLookup = await _context.NodeSensorAssignments
            .Where(a => a.NodeId == nodeId && a.CloudSensorId.HasValue)
            .ToDictionaryAsync(a => a.Id, a => a.CloudSensorId!.Value, ct);

        while (true)
        {
            ct.ThrowIfCancellationRequested();

            // Get next batch of unsynced readings
            var readings = (await _readingService.GetUnsyncedByNodeAsync(nodeId, batchSize, 0, ct)).ToList();

            if (readings.Count == 0)
                break;

            // Filter readings that have CloudSensorId (assignment must be synced)
            var validReadings = readings
                .Where(r => r.AssignmentId.HasValue && assignmentLookup.ContainsKey(r.AssignmentId.Value))
                .ToList();

            if (validReadings.Count == 0)
            {
                // Mark readings without valid assignments as synced (can't upload them)
                var invalidReadingIds = readings.Select(r => r.Id).ToList();
                await _readingService.MarkAsSyncedWithTimestampAsync(invalidReadingIds, ct);
                _logger.LogWarning(
                    "Skipped {Count} readings without valid cloud sensor assignments for node {NodeId}",
                    invalidReadingIds.Count, nodeId);
                continue;
            }

            // Map to sync DTOs
            var readingDtos = validReadings.Select(r => new SyncReadingDto(
                LocalReadingId: r.Id,
                SensorCloudId: assignmentLookup[r.AssignmentId!.Value],
                MeasurementType: r.MeasurementType,
                RawValue: r.RawValue,
                Value: r.Value,
                Unit: r.Unit,
                Timestamp: r.Timestamp,
                Quality: ReadingQualityDto.Good
            )).ToArray();

            // Upload batch
            var dto = new SyncReadingsBatchDto(
                NodeCloudId: nodeCloudId,
                Readings: readingDtos
            );

            await _cloudApiClient.UploadReadingsAsync(dto, ct);

            // Mark as synced
            var syncedIds = validReadings.Select(r => r.Id).ToList();
            await _readingService.MarkAsSyncedWithTimestampAsync(syncedIds, ct);

            totalSynced += validReadings.Count;
            offset += batchSize;

            // Report progress
            var percentComplete = totalCount > 0
                ? Math.Min(100, 50 + (int)(50.0 * totalSynced / totalCount))
                : 100;

            progress?.Report(new SyncProgressDto(
                Stage: "Readings",
                Message: $"Uploaded {totalSynced}/{totalCount} readings...",
                ReadingsSynced: totalSynced,
                TotalReadings: totalCount,
                PercentComplete: percentComplete));

            _logger.LogDebug("Synced batch: {Count} readings for node {NodeId}", validReadings.Count, nodeId);
        }

        return totalSynced;
    }

    /// <inheritdoc />
    public async Task<SyncStatusDto?> GetSyncStatusAsync(Guid nodeId, CancellationToken ct = default)
    {
        var node = await _context.Nodes
            .Include(n => n.SyncState)
            .FirstOrDefaultAsync(n => n.Id == nodeId, ct);

        if (node == null)
            return null;

        var unsyncedCount = await _readingService.GetUnsyncedCountByNodeAsync(nodeId, ct);
        var syncState = node.SyncState;

        return new SyncStatusDto(
            NodeId: nodeId,
            NodeName: node.Name,
            LastSyncAt: syncState?.LastSyncAt,
            LastSyncSuccess: syncState?.LastSyncSuccess,
            LastSyncError: syncState?.LastSyncError,
            LastSyncDuration: syncState?.LastSyncDuration,
            UnsyncedReadingsCount: unsyncedCount,
            IsSyncing: syncState?.IsSyncing ?? false,
            CloudId: syncState?.CloudNodeId
        );
    }

    /// <inheritdoc />
    public async Task<IEnumerable<SyncStatusDto>> GetAllSyncStatusAsync(CancellationToken ct = default)
    {
        var nodes = await _context.Nodes
            .Include(n => n.SyncState)
            .ToListAsync(ct);

        var statusList = new List<SyncStatusDto>();

        foreach (var node in nodes)
        {
            var unsyncedCount = await _readingService.GetUnsyncedCountByNodeAsync(node.Id, ct);
            var syncState = node.SyncState;

            statusList.Add(new SyncStatusDto(
                NodeId: node.Id,
                NodeName: node.Name,
                LastSyncAt: syncState?.LastSyncAt,
                LastSyncSuccess: syncState?.LastSyncSuccess,
                LastSyncError: syncState?.LastSyncError,
                LastSyncDuration: syncState?.LastSyncDuration,
                UnsyncedReadingsCount: unsyncedCount,
                IsSyncing: syncState?.IsSyncing ?? false,
                CloudId: syncState?.CloudNodeId
            ));
        }

        return statusList;
    }

    /// <inheritdoc />
    public async Task<SyncSummaryDto> GetSyncSummaryAsync(CancellationToken ct = default)
    {
        var nodes = await _context.Nodes
            .Include(n => n.SyncState)
            .ToListAsync(ct);

        var totalNodes = nodes.Count;
        var syncedNodes = nodes.Count(n => n.SyncState?.CloudNodeId != null);
        var neverSyncedNodes = totalNodes - syncedNodes;

        var totalUnsyncedReadings = 0;
        foreach (var node in nodes)
        {
            totalUnsyncedReadings += await _readingService.GetUnsyncedCountByNodeAsync(node.Id, ct);
        }

        var lastSyncAt = nodes
            .Where(n => n.SyncState?.LastSyncAt != null)
            .Select(n => n.SyncState!.LastSyncAt)
            .OrderByDescending(d => d)
            .FirstOrDefault();

        return new SyncSummaryDto(
            TotalNodes: totalNodes,
            SyncedNodes: syncedNodes,
            NeverSyncedNodes: neverSyncedNodes,
            TotalUnsyncedReadings: totalUnsyncedReadings,
            LastSyncAt: lastSyncAt
        );
    }

    /// <inheritdoc />
    public async Task<IEnumerable<SyncHistoryEntryDto>> GetSyncHistoryAsync(
        Guid nodeId,
        int limit = 10,
        CancellationToken ct = default)
    {
        var entries = await _context.SyncHistoryEntries
            .Include(e => e.Node)
            .Where(e => e.NodeId == nodeId)
            .OrderByDescending(e => e.StartedAt)
            .Take(limit)
            .ToListAsync(ct);

        return entries.Select(e => new SyncHistoryEntryDto(
            Id: e.Id,
            NodeId: e.NodeId,
            NodeName: e.Node.Name,
            JobId: e.JobId,
            StartedAt: e.StartedAt,
            CompletedAt: e.CompletedAt,
            Duration: e.Duration,
            Success: e.Success,
            Error: e.Error,
            NodeAction: e.NodeAction,
            SensorsCreated: e.SensorsCreated,
            SensorsUpdated: e.SensorsUpdated,
            ReadingsSynced: e.ReadingsSynced
        ));
    }

    /// <inheritdoc />
    public async Task<int> GetUnsyncedReadingsCountAsync(Guid nodeId, CancellationToken ct = default)
    {
        return await _readingService.GetUnsyncedCountByNodeAsync(nodeId, ct);
    }

    /// <inheritdoc />
    public Task<bool> IsSyncInProgressAsync(Guid nodeId, CancellationToken ct = default)
    {
        lock (_syncLock)
        {
            return Task.FromResult(_activeSyncs.ContainsKey(nodeId));
        }
    }

    /// <inheritdoc />
    public Task<bool> CancelSyncAsync(Guid nodeId, CancellationToken ct = default)
    {
        lock (_syncLock)
        {
            if (_activeSyncs.TryGetValue(nodeId, out var cts))
            {
                cts.Cancel();
                _logger.LogInformation("Sync cancelled for node {NodeId}", nodeId);
                return Task.FromResult(true);
            }
        }

        return Task.FromResult(false);
    }

    private async Task<NodeSyncState> CreateSyncStateAsync(Node node, CancellationToken ct)
    {
        var syncState = new NodeSyncState
        {
            Id = Guid.NewGuid(),
            NodeId = node.Id,
            UpdatedAt = DateTime.UtcNow
        };

        _context.NodeSyncStates.Add(syncState);
        await _context.SaveChangesAsync(ct);

        node.SyncState = syncState;
        return syncState;
    }
}
