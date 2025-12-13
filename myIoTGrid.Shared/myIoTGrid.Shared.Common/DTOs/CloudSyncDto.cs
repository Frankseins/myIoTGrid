using myIoTGrid.Shared.Common.Enums;

namespace myIoTGrid.Shared.Common.DTOs;

// ============================================================================
// Hub-Cloud Synchronisation DTOs
// Manual One-Way Sync: Hub → Cloud
// ============================================================================

// === Node Sync ===

/// <summary>
/// DTO for syncing a Node configuration to the Cloud.
/// Sent from Hub to Cloud via POST /api/hub-sync/nodes
/// </summary>
public record SyncNodeDto(
    /// <summary>Hub ID (from Hub installation)</summary>
    Guid HubId,
    /// <summary>Local Node ID in Hub database</summary>
    Guid LocalNodeId,
    /// <summary>Hardware identifier (MAC address)</summary>
    string NodeId,
    /// <summary>Display name</summary>
    string Name,
    /// <summary>Location information</summary>
    string? Location,
    /// <summary>Communication protocol</summary>
    ProtocolDto Protocol,
    /// <summary>Firmware version</summary>
    string? FirmwareVersion,
    /// <summary>Additional metadata as key-value pairs</summary>
    Dictionary<string, string>? Metadata
);

/// <summary>
/// Response from Cloud after upserting a Node.
/// </summary>
public record SyncNodeResponseDto(
    /// <summary>Cloud-assigned ID for this node</summary>
    Guid CloudId,
    /// <summary>True if node was created, false if updated</summary>
    bool WasCreated
);

// === Sensors Sync ===

/// <summary>
/// DTO for syncing all sensors (NodeSensorAssignments) for a Node to the Cloud.
/// </summary>
public record SyncSensorsDto(
    /// <summary>Cloud ID of the Node (from SyncNodeResponseDto)</summary>
    Guid NodeCloudId,
    /// <summary>All sensor assignments to sync</summary>
    SyncSensorDto[] Sensors
);

/// <summary>
/// Single sensor assignment configuration for Cloud sync.
/// </summary>
public record SyncSensorDto(
    /// <summary>Local sensor assignment ID in Hub database</summary>
    Guid LocalSensorId,
    /// <summary>Cloud ID of the parent Node</summary>
    Guid NodeCloudId,
    /// <summary>Sensor code (e.g., "bme280-wohnzimmer")</summary>
    string SensorCode,
    /// <summary>Display name</summary>
    string? Name,
    /// <summary>Measurement type (e.g., "temperature")</summary>
    string MeasurementType,
    /// <summary>Unit of measurement</summary>
    string Unit,
    /// <summary>Sampling interval in seconds</summary>
    int? SamplingIntervalSeconds,
    /// <summary>Alert threshold value</summary>
    double? Threshold,
    /// <summary>Whether sensor is enabled</summary>
    bool IsEnabled
);

/// <summary>
/// Response from Cloud after upserting sensors.
/// </summary>
public record SyncSensorsResponseDto(
    /// <summary>Results for each sensor</summary>
    SyncSensorResultDto[] Sensors
);

/// <summary>
/// Result for a single sensor upsert operation.
/// </summary>
public record SyncSensorResultDto(
    /// <summary>Local sensor assignment ID</summary>
    Guid LocalSensorId,
    /// <summary>Cloud-assigned ID</summary>
    Guid CloudId,
    /// <summary>True if created, false if updated</summary>
    bool WasCreated
);

// === Readings Sync ===

/// <summary>
/// DTO for uploading a batch of readings to the Cloud.
/// </summary>
public record SyncReadingsBatchDto(
    /// <summary>Cloud ID of the Node</summary>
    Guid NodeCloudId,
    /// <summary>Batch of readings to upload</summary>
    SyncReadingDto[] Readings
);

/// <summary>
/// Single reading for Cloud upload.
/// </summary>
public record SyncReadingDto(
    /// <summary>Local reading ID in Hub database</summary>
    long LocalReadingId,
    /// <summary>Cloud ID of the sensor (from SyncSensorResultDto)</summary>
    Guid SensorCloudId,
    /// <summary>Measurement type</summary>
    string MeasurementType,
    /// <summary>Raw value from sensor</summary>
    double RawValue,
    /// <summary>Calibrated value</summary>
    double Value,
    /// <summary>Unit of measurement</summary>
    string Unit,
    /// <summary>UTC timestamp of measurement</summary>
    DateTime Timestamp,
    /// <summary>Reading quality indicator</summary>
    ReadingQualityDto Quality
);

/// <summary>
/// Response from Cloud after uploading readings.
/// </summary>
public record SyncReadingsResponseDto(
    /// <summary>Number of readings accepted</summary>
    int AcceptedCount,
    /// <summary>Number of readings rejected (duplicates, errors)</summary>
    int RejectedCount
);

// === Progress & Status ===

/// <summary>
/// Progress update during sync operation.
/// Sent via SignalR to connected clients.
/// </summary>
public record SyncProgressDto(
    /// <summary>Current stage: "Node", "Sensors", "Readings", "Complete"</summary>
    string Stage,
    /// <summary>Human-readable progress message</summary>
    string Message,
    /// <summary>Number of readings synced so far (only in Readings stage)</summary>
    int? ReadingsSynced = null,
    /// <summary>Total readings to sync (only in Readings stage)</summary>
    int? TotalReadings = null,
    /// <summary>Percentage complete (0-100)</summary>
    int? PercentComplete = null
);

/// <summary>
/// Final result of a sync operation.
/// </summary>
public record SyncResultDto
{
    /// <summary>Node ID that was synced</summary>
    public Guid NodeId { get; init; }

    /// <summary>Whether sync completed successfully</summary>
    public bool Success { get; set; }

    /// <summary>Error message if sync failed</summary>
    public string? Error { get; set; }

    /// <summary>Total duration of sync operation</summary>
    public TimeSpan Duration { get; set; }

    /// <summary>Node action: "Created" or "Updated"</summary>
    public string NodeAction { get; set; } = string.Empty;

    /// <summary>Number of sensors created in Cloud</summary>
    public int SensorsCreated { get; set; }

    /// <summary>Number of sensors updated in Cloud</summary>
    public int SensorsUpdated { get; set; }

    /// <summary>Number of readings uploaded to Cloud</summary>
    public int ReadingsSynced { get; set; }
}

/// <summary>
/// Current sync status for a Node.
/// </summary>
public record SyncStatusDto(
    /// <summary>Node ID</summary>
    Guid NodeId,
    /// <summary>Node display name</summary>
    string NodeName,
    /// <summary>Timestamp of last successful sync</summary>
    DateTime? LastSyncAt,
    /// <summary>Whether last sync was successful</summary>
    bool? LastSyncSuccess,
    /// <summary>Error from last sync (if any)</summary>
    string? LastSyncError,
    /// <summary>Duration of last sync</summary>
    TimeSpan? LastSyncDuration,
    /// <summary>Number of readings not yet synced to Cloud</summary>
    int UnsyncedReadingsCount,
    /// <summary>Whether a sync is currently in progress</summary>
    bool IsSyncing,
    /// <summary>Cloud ID (null if never synced)</summary>
    Guid? CloudId
);

/// <summary>
/// Summary status for all nodes.
/// </summary>
public record SyncSummaryDto(
    /// <summary>Total number of nodes</summary>
    int TotalNodes,
    /// <summary>Number of nodes that have been synced at least once</summary>
    int SyncedNodes,
    /// <summary>Number of nodes never synced</summary>
    int NeverSyncedNodes,
    /// <summary>Total unsynced readings across all nodes</summary>
    int TotalUnsyncedReadings,
    /// <summary>Timestamp of most recent sync</summary>
    DateTime? LastSyncAt
);

/// <summary>
/// Entry in sync history log.
/// </summary>
public record SyncHistoryEntryDto(
    /// <summary>Unique ID of sync operation</summary>
    Guid Id,
    /// <summary>Node ID</summary>
    Guid NodeId,
    /// <summary>Node display name</summary>
    string NodeName,
    /// <summary>Job ID for tracking</summary>
    Guid JobId,
    /// <summary>When sync started</summary>
    DateTime StartedAt,
    /// <summary>When sync completed</summary>
    DateTime? CompletedAt,
    /// <summary>Duration of sync</summary>
    TimeSpan? Duration,
    /// <summary>Whether sync was successful</summary>
    bool Success,
    /// <summary>Error message if failed</summary>
    string? Error,
    /// <summary>Node action performed</summary>
    string NodeAction,
    /// <summary>Number of sensors created</summary>
    int SensorsCreated,
    /// <summary>Number of sensors updated</summary>
    int SensorsUpdated,
    /// <summary>Number of readings uploaded</summary>
    int ReadingsSynced
);

// === Request DTOs ===

/// <summary>
/// Request to start manual sync for a node.
/// </summary>
public record StartSyncRequestDto(
    /// <summary>Node ID to sync</summary>
    Guid NodeId
);

/// <summary>
/// Response when sync job is started.
/// </summary>
public record StartSyncResponseDto(
    /// <summary>Job ID for tracking progress via SignalR</summary>
    Guid JobId,
    /// <summary>Message</summary>
    string Message
);

// === Quality Enum ===

/// <summary>
/// Quality indicator for readings.
/// </summary>
public enum ReadingQualityDto
{
    Good = 0,
    Uncertain = 1,
    Bad = 2
}
