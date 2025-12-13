using myIoTGrid.Shared.Common.Interfaces;

namespace myIoTGrid.Shared.Common.Entities;

/// <summary>
/// Tracks the Cloud synchronization state for a Node.
/// One-to-one relationship with Node.
/// </summary>
public class NodeSyncState : IEntity
{
    /// <summary>Primary key</summary>
    public Guid Id { get; set; }

    /// <summary>FK to the Node</summary>
    public Guid NodeId { get; set; }

    // === Cloud Reference ===

    /// <summary>Cloud-assigned ID for this node (null if never synced)</summary>
    public Guid? CloudNodeId { get; set; }

    // === Last Sync Info ===

    /// <summary>Timestamp of last successful sync</summary>
    public DateTime? LastSyncAt { get; set; }

    /// <summary>Whether the last sync was successful</summary>
    public bool LastSyncSuccess { get; set; }

    /// <summary>Error message from last sync (if any)</summary>
    public string? LastSyncError { get; set; }

    /// <summary>Duration of last sync operation</summary>
    public TimeSpan? LastSyncDuration { get; set; }

    // === Statistics ===

    /// <summary>Total number of sync operations attempted</summary>
    public int TotalSyncs { get; set; }

    /// <summary>Number of successful syncs</summary>
    public int SuccessfulSyncs { get; set; }

    /// <summary>Number of failed syncs</summary>
    public int FailedSyncs { get; set; }

    /// <summary>Total number of readings synced to Cloud</summary>
    public long TotalReadingsSynced { get; set; }

    // === Current Status ===

    /// <summary>Whether a sync is currently in progress</summary>
    public bool IsSyncing { get; set; }

    /// <summary>Current job ID (if sync in progress)</summary>
    public Guid? CurrentJobId { get; set; }

    /// <summary>When this record was last updated</summary>
    public DateTime UpdatedAt { get; set; }

    // === Navigation Properties ===

    /// <summary>Associated Node</summary>
    public Node Node { get; set; } = null!;
}
