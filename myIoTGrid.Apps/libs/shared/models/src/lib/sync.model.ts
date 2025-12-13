/**
 * Cloud Sync Models
 * Types for Hub-to-Cloud synchronization
 */

/**
 * Response when starting a sync job.
 */
export interface StartSyncResponse {
  jobId: string;
  message: string;
}

/**
 * Progress update during sync operation.
 * Sent via SignalR to connected clients.
 */
export interface SyncProgress {
  /** Current stage: "Node", "Sensors", "Readings", "Complete" */
  stage: string;
  /** Human-readable progress message */
  message: string;
  /** Number of readings synced so far (only in Readings stage) */
  readingsSynced?: number;
  /** Total readings to sync (only in Readings stage) */
  totalReadings?: number;
  /** Percentage complete (0-100) */
  percentComplete?: number;
}

/**
 * Final result of a sync operation.
 */
export interface SyncResult {
  /** Node ID that was synced */
  nodeId: string;
  /** Whether sync completed successfully */
  success: boolean;
  /** Error message if sync failed */
  error?: string;
  /** Total duration of sync operation */
  duration: string;
  /** Node action: "Created" or "Updated" */
  nodeAction: string;
  /** Number of sensors created in Cloud */
  sensorsCreated: number;
  /** Number of sensors updated in Cloud */
  sensorsUpdated: number;
  /** Number of readings uploaded to Cloud */
  readingsSynced: number;
}

/**
 * Current sync status for a Node.
 */
export interface SyncStatus {
  /** Node ID */
  nodeId: string;
  /** Node display name */
  nodeName: string;
  /** Timestamp of last successful sync */
  lastSyncAt?: string;
  /** Whether last sync was successful */
  lastSyncSuccess?: boolean;
  /** Error from last sync (if any) */
  lastSyncError?: string;
  /** Duration of last sync */
  lastSyncDuration?: string;
  /** Number of readings not yet synced to Cloud */
  unsyncedReadingsCount: number;
  /** Whether a sync is currently in progress */
  isSyncing: boolean;
  /** Cloud ID (null if never synced) */
  cloudId?: string;
}

/**
 * Summary status for all nodes.
 */
export interface SyncSummary {
  /** Total number of nodes */
  totalNodes: number;
  /** Number of nodes that have been synced at least once */
  syncedNodes: number;
  /** Number of nodes never synced */
  neverSyncedNodes: number;
  /** Total unsynced readings across all nodes */
  totalUnsyncedReadings: number;
  /** Timestamp of most recent sync */
  lastSyncAt?: string;
}

/**
 * Entry in sync history log.
 */
export interface SyncHistoryEntry {
  /** Unique ID of sync operation */
  id: string;
  /** Node ID */
  nodeId: string;
  /** Node display name */
  nodeName: string;
  /** Job ID for tracking */
  jobId: string;
  /** When sync started */
  startedAt: string;
  /** When sync completed */
  completedAt?: string;
  /** Duration of sync */
  duration?: string;
  /** Whether sync was successful */
  success: boolean;
  /** Error message if failed */
  error?: string;
  /** Node action performed */
  nodeAction: string;
  /** Number of sensors created */
  sensorsCreated: number;
  /** Number of sensors updated */
  sensorsUpdated: number;
  /** Number of readings uploaded */
  readingsSynced: number;
}

/**
 * Count of unsynced readings for a node.
 */
export interface UnsyncedCount {
  nodeId: string;
  unsyncedCount: number;
}
