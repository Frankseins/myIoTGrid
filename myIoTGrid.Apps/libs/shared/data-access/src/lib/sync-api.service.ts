import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { BaseApiService } from './base-api.service';
import {
  StartSyncResponse,
  SyncStatus,
  SyncSummary,
  SyncHistoryEntry,
  UnsyncedCount
} from '@myiotgrid/shared/models';

/**
 * API Service for Cloud Synchronization.
 * Handles Hub-to-Cloud sync operations.
 */
@Injectable({ providedIn: 'root' })
export class SyncApiService extends BaseApiService {
  private readonly endpoint = '/sync';

  /**
   * Start manual sync for a specific node.
   * POST /api/sync/nodes/{nodeId}
   * @returns 202 Accepted with JobId and Message
   */
  startSync(nodeId: string): Observable<StartSyncResponse> {
    return this.post<StartSyncResponse>(`${this.endpoint}/nodes/${nodeId}`, {});
  }

  /**
   * Cancel an in-progress sync for a node.
   * POST /api/sync/nodes/{nodeId}/cancel
   */
  cancelSync(nodeId: string): Observable<{ message: string }> {
    return this.post<{ message: string }>(`${this.endpoint}/nodes/${nodeId}/cancel`, {});
  }

  /**
   * Get sync status for a specific node.
   * GET /api/sync/nodes/{nodeId}/status
   */
  getSyncStatus(nodeId: string): Observable<SyncStatus> {
    return this.get<SyncStatus>(`${this.endpoint}/nodes/${nodeId}/status`);
  }

  /**
   * Get sync status for all nodes.
   * GET /api/sync/status
   */
  getAllSyncStatus(): Observable<SyncStatus[]> {
    return this.get<SyncStatus[]>(`${this.endpoint}/status`);
  }

  /**
   * Get sync summary across all nodes.
   * GET /api/sync/summary
   */
  getSyncSummary(): Observable<SyncSummary> {
    return this.get<SyncSummary>(`${this.endpoint}/summary`);
  }

  /**
   * Get sync history for a specific node.
   * GET /api/sync/nodes/{nodeId}/history
   */
  getSyncHistory(nodeId: string, limit: number = 10): Observable<SyncHistoryEntry[]> {
    return this.get<SyncHistoryEntry[]>(`${this.endpoint}/nodes/${nodeId}/history`, { limit });
  }

  /**
   * Get count of unsynced readings for a specific node.
   * GET /api/sync/nodes/{nodeId}/unsynced-count
   */
  getUnsyncedCount(nodeId: string): Observable<UnsyncedCount> {
    return this.get<UnsyncedCount>(`${this.endpoint}/nodes/${nodeId}/unsynced-count`);
  }
}
