using myIoTGrid.Shared.Common.Interfaces;

namespace myIoTGrid.Shared.Common.Entities;

/// <summary>
/// Log entry for a sync operation.
/// Provides audit trail for Hub-Cloud synchronization.
/// </summary>
public class SyncHistoryEntry : IEntity
{
    /// <summary>Primary key</summary>
    public Guid Id { get; set; }

    /// <summary>FK to the Node that was synced</summary>
    public Guid NodeId { get; set; }

    /// <summary>Unique job ID for tracking this sync operation</summary>
    public Guid JobId { get; set; }

    // === Timing ===

    /// <summary>When the sync started</summary>
    public DateTime StartedAt { get; set; }

    /// <summary>When the sync completed (null if still running)</summary>
    public DateTime? CompletedAt { get; set; }

    /// <summary>Duration of the sync operation</summary>
    public TimeSpan? Duration { get; set; }

    // === Result ===

    /// <summary>Whether the sync was successful</summary>
    public bool Success { get; set; }

    /// <summary>Error message if sync failed</summary>
    public string? Error { get; set; }

    // === Statistics ===

    /// <summary>Action performed on Node: "Created" or "Updated"</summary>
    public string NodeAction { get; set; } = string.Empty;

    /// <summary>Number of sensors created in Cloud</summary>
    public int SensorsCreated { get; set; }

    /// <summary>Number of sensors updated in Cloud</summary>
    public int SensorsUpdated { get; set; }

    /// <summary>Number of readings uploaded to Cloud</summary>
    public int ReadingsSynced { get; set; }

    // === Navigation Properties ===

    /// <summary>Associated Node</summary>
    public Node Node { get; set; } = null!;
}
