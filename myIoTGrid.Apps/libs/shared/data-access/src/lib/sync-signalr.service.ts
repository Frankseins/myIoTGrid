import { Injectable, inject, signal } from '@angular/core';
import * as signalR from '@microsoft/signalr';
import { API_CONFIG, defaultApiConfig } from './api.config';
import { SyncProgress, SyncResult } from '@myiotgrid/shared/models';

export type SyncConnectionState = 'disconnected' | 'connecting' | 'connected' | 'reconnecting';

/**
 * SignalR Service for Cloud Sync Progress updates.
 * Connects to the /hubs/sync-progress endpoint.
 */
@Injectable({ providedIn: 'root' })
export class SyncSignalRService {
  private readonly config = inject(API_CONFIG, { optional: true }) ?? defaultApiConfig;
  private hubConnection: signalR.HubConnection | null = null;

  readonly connectionState = signal<SyncConnectionState>('disconnected');
  readonly lastError = signal<string | null>(null);

  private get syncHubUrl(): string {
    // Use the same base URL but different hub path
    const base = this.config.signalRUrl.replace('/hubs/sensors', '');
    return `${base}/hubs/sync-progress`;
  }

  async startConnection(): Promise<void> {
    if (this.hubConnection?.state === signalR.HubConnectionState.Connected) {
      return;
    }

    this.connectionState.set('connecting');
    this.lastError.set(null);

    try {
      this.hubConnection = new signalR.HubConnectionBuilder()
        .withUrl(this.syncHubUrl)
        .withAutomaticReconnect([0, 2000, 5000, 10000, 30000])
        .configureLogging(signalR.LogLevel.Information)
        .build();

      this.setupConnectionHandlers();
      await this.hubConnection.start();
      this.connectionState.set('connected');
      console.log('Sync SignalR Connected');
    } catch (error) {
      const message = error instanceof Error ? error.message : 'Unknown error';
      this.lastError.set(message);
      this.connectionState.set('disconnected');
      console.error('Sync SignalR Connection Error:', error);
      throw error;
    }
  }

  async stopConnection(): Promise<void> {
    if (this.hubConnection) {
      await this.hubConnection.stop();
      this.hubConnection = null;
      this.connectionState.set('disconnected');
    }
  }

  private setupConnectionHandlers(): void {
    if (!this.hubConnection) return;

    this.hubConnection.onreconnecting(() => {
      this.connectionState.set('reconnecting');
      console.log('Sync SignalR Reconnecting...');
    });

    this.hubConnection.onreconnected(() => {
      this.connectionState.set('connected');
      console.log('Sync SignalR Reconnected');
    });

    this.hubConnection.onclose((error) => {
      this.connectionState.set('disconnected');
      if (error) {
        this.lastError.set(error.message);
        console.error('Sync SignalR Connection Closed with error:', error);
      }
    });
  }

  // ==========================================
  // Sync Progress Events
  // ==========================================

  /**
   * Subscribe to sync progress updates.
   * Event: SyncProgress
   * @param callback (jobId, nodeId, progress) => void
   */
  onSyncProgress(callback: (jobId: string, nodeId: string, progress: SyncProgress) => void): void {
    this.hubConnection?.on('SyncProgress', callback);
  }

  /**
   * Subscribe to sync complete events.
   * Event: SyncComplete
   * @param callback (jobId, nodeId, result) => void
   */
  onSyncComplete(callback: (jobId: string, nodeId: string, result: SyncResult) => void): void {
    this.hubConnection?.on('SyncComplete', callback);
  }

  // ==========================================
  // Group Management
  // ==========================================

  /**
   * Join a node-specific sync group to receive updates for that node.
   */
  async joinSyncGroup(nodeId: string): Promise<void> {
    await this.hubConnection?.invoke('JoinSyncGroup', nodeId);
  }

  /**
   * Leave a node-specific sync group.
   */
  async leaveSyncGroup(nodeId: string): Promise<void> {
    await this.hubConnection?.invoke('LeaveSyncGroup', nodeId);
  }

  /**
   * Join the all-sync group to receive updates for all nodes.
   */
  async joinAllSyncGroup(): Promise<void> {
    await this.hubConnection?.invoke('JoinAllSyncGroup');
  }

  /**
   * Leave the all-sync group.
   */
  async leaveAllSyncGroup(): Promise<void> {
    await this.hubConnection?.invoke('LeaveAllSyncGroup');
  }

  // ==========================================
  // Utility Methods
  // ==========================================

  /**
   * Remove event handler
   */
  off(eventName: string): void {
    this.hubConnection?.off(eventName);
  }
}
