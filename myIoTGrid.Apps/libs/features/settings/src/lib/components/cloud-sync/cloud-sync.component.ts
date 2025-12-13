import { Component, OnInit, OnDestroy, inject, signal, computed } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatListModule } from '@angular/material/list';
import { MatDividerModule } from '@angular/material/divider';
import { MatProgressBarModule } from '@angular/material/progress-bar';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatChipsModule } from '@angular/material/chips';
import { MatTableModule } from '@angular/material/table';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatSnackBar, MatSnackBarModule } from '@angular/material/snack-bar';
import { SyncApiService, SyncSignalRService } from '@myiotgrid/shared/data-access';
import {
  SyncStatus,
  SyncSummary,
  SyncProgress,
  SyncResult,
  SyncHistoryEntry
} from '@myiotgrid/shared/models';
import { LoadingSpinnerComponent } from '@myiotgrid/shared/ui';
import { RelativeTimePipe } from '@myiotgrid/shared/utils';

interface ActiveSync {
  jobId: string;
  nodeId: string;
  nodeName: string;
  progress: SyncProgress;
  startedAt: Date;
}

@Component({
  selector: 'myiotgrid-cloud-sync',
  standalone: true,
  imports: [
    CommonModule,
    MatCardModule,
    MatIconModule,
    MatButtonModule,
    MatListModule,
    MatDividerModule,
    MatProgressBarModule,
    MatProgressSpinnerModule,
    MatExpansionModule,
    MatChipsModule,
    MatTableModule,
    MatTooltipModule,
    MatSnackBarModule,
    LoadingSpinnerComponent,
    RelativeTimePipe
  ],
  templateUrl: './cloud-sync.component.html',
  styleUrl: './cloud-sync.component.scss'
})
export class CloudSyncComponent implements OnInit, OnDestroy {
  private readonly syncApiService = inject(SyncApiService);
  private readonly syncSignalR = inject(SyncSignalRService);
  private readonly snackBar = inject(MatSnackBar);

  // State
  readonly isLoading = signal(true);
  readonly syncSummary = signal<SyncSummary | null>(null);
  readonly syncStatusList = signal<SyncStatus[]>([]);
  readonly activeSyncs = signal<Map<string, ActiveSync>>(new Map());
  readonly selectedNodeHistory = signal<SyncHistoryEntry[]>([]);
  readonly selectedNodeId = signal<string | null>(null);
  readonly isHistoryLoading = signal(false);

  // Computed
  readonly hasActiveSyncs = computed(() => this.activeSyncs().size > 0);
  readonly activeSyncsList = computed(() => Array.from(this.activeSyncs().values()));

  // Table columns
  readonly historyColumns = ['startedAt', 'nodeAction', 'sensors', 'readings', 'duration', 'status'];

  async ngOnInit(): Promise<void> {
    await this.loadData();
    await this.connectSignalR();
  }

  ngOnDestroy(): void {
    this.syncSignalR.leaveAllSyncGroup();
  }

  private async loadData(): Promise<void> {
    this.isLoading.set(true);
    try {
      const [summary, statusList] = await Promise.all([
        this.syncApiService.getSyncSummary().toPromise(),
        this.syncApiService.getAllSyncStatus().toPromise()
      ]);
      this.syncSummary.set(summary || null);
      this.syncStatusList.set(statusList || []);
    } catch (error) {
      console.error('Error loading sync data:', error);
      this.snackBar.open('Fehler beim Laden der Sync-Daten', 'OK', { duration: 3000 });
    } finally {
      this.isLoading.set(false);
    }
  }

  private async connectSignalR(): Promise<void> {
    try {
      await this.syncSignalR.startConnection();
      await this.syncSignalR.joinAllSyncGroup();

      // Subscribe to progress updates
      this.syncSignalR.onSyncProgress((jobId, nodeId, progress) => {
        this.handleSyncProgress(jobId, nodeId, progress);
      });

      // Subscribe to sync complete
      this.syncSignalR.onSyncComplete((jobId, nodeId, result) => {
        this.handleSyncComplete(jobId, nodeId, result);
      });
    } catch (error) {
      console.error('Error connecting to sync SignalR:', error);
    }
  }

  private handleSyncProgress(jobId: string, nodeId: string, progress: SyncProgress): void {
    const current = this.activeSyncs();
    const existing = current.get(nodeId);

    if (existing) {
      existing.progress = progress;
    } else {
      const status = this.syncStatusList().find(s => s.nodeId === nodeId);
      current.set(nodeId, {
        jobId,
        nodeId,
        nodeName: status?.nodeName || nodeId,
        progress,
        startedAt: new Date()
      });
    }

    this.activeSyncs.set(new Map(current));
  }

  private handleSyncComplete(jobId: string, nodeId: string, result: SyncResult): void {
    // Remove from active syncs
    const current = this.activeSyncs();
    current.delete(nodeId);
    this.activeSyncs.set(new Map(current));

    // Show notification
    if (result.success) {
      this.snackBar.open(
        `Sync abgeschlossen: ${result.readingsSynced} Messwerte synchronisiert`,
        'OK',
        { duration: 5000 }
      );
    } else {
      this.snackBar.open(
        `Sync fehlgeschlagen: ${result.error}`,
        'OK',
        { duration: 5000, panelClass: 'error-snackbar' }
      );
    }

    // Reload data
    this.loadData();
  }

  async startSync(nodeId: string): Promise<void> {
    try {
      const response = await this.syncApiService.startSync(nodeId).toPromise();
      this.snackBar.open('Synchronisation gestartet...', 'OK', { duration: 2000 });

      // Optimistically add to active syncs
      const status = this.syncStatusList().find(s => s.nodeId === nodeId);
      if (response) {
        const current = this.activeSyncs();
        current.set(nodeId, {
          jobId: response.jobId,
          nodeId,
          nodeName: status?.nodeName || nodeId,
          progress: { stage: 'Starting', message: 'Sync wird gestartet...' },
          startedAt: new Date()
        });
        this.activeSyncs.set(new Map(current));
      }
    } catch (error: unknown) {
      const errorMessage = error instanceof Error ? error.message : 'Unbekannter Fehler';
      this.snackBar.open(`Fehler: ${errorMessage}`, 'OK', { duration: 3000 });
    }
  }

  async cancelSync(nodeId: string): Promise<void> {
    try {
      await this.syncApiService.cancelSync(nodeId).toPromise();
      this.snackBar.open('Synchronisation abgebrochen', 'OK', { duration: 2000 });

      // Remove from active syncs
      const current = this.activeSyncs();
      current.delete(nodeId);
      this.activeSyncs.set(new Map(current));

      // Reload data
      await this.loadData();
    } catch (error) {
      console.error('Error cancelling sync:', error);
      this.snackBar.open('Fehler beim Abbrechen', 'OK', { duration: 3000 });
    }
  }

  async loadHistory(nodeId: string): Promise<void> {
    if (this.selectedNodeId() === nodeId) {
      this.selectedNodeId.set(null);
      this.selectedNodeHistory.set([]);
      return;
    }

    this.selectedNodeId.set(nodeId);
    this.isHistoryLoading.set(true);
    try {
      const history = await this.syncApiService.getSyncHistory(nodeId, 10).toPromise();
      this.selectedNodeHistory.set(history || []);
    } catch (error) {
      console.error('Error loading sync history:', error);
      this.snackBar.open('Fehler beim Laden der Historie', 'OK', { duration: 3000 });
    } finally {
      this.isHistoryLoading.set(false);
    }
  }

  isSyncing(nodeId: string): boolean {
    return this.activeSyncs().has(nodeId);
  }

  getActiveSync(nodeId: string): ActiveSync | undefined {
    return this.activeSyncs().get(nodeId);
  }

  formatDuration(duration: string | undefined): string {
    if (!duration) return '–';
    // Duration comes as TimeSpan string like "00:00:05.1234567"
    const parts = duration.split(':');
    if (parts.length >= 3) {
      const seconds = parseFloat(parts[2]);
      const minutes = parseInt(parts[1], 10);
      const hours = parseInt(parts[0], 10);

      if (hours > 0) return `${hours}h ${minutes}m`;
      if (minutes > 0) return `${minutes}m ${Math.round(seconds)}s`;
      return `${seconds.toFixed(1)}s`;
    }
    return duration;
  }

  async refresh(): Promise<void> {
    await this.loadData();
  }
}
