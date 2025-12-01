import { Component, OnInit, inject, signal } from '@angular/core';
import { CommonModule } from '@angular/common';
import { Router, RouterModule } from '@angular/router';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatChipsModule } from '@angular/material/chips';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSelectModule } from '@angular/material/select';
import { FormsModule } from '@angular/forms';
import { HubApiService, NodeApiService, ReadingApiService, SensorTypeApiService } from '@myiotgrid/shared/data-access';
import { Hub, Node, Reading } from '@myiotgrid/shared/models';
import { LoadingSpinnerComponent, EmptyStateComponent } from '@myiotgrid/shared/ui';
import { RelativeTimePipe, SensorUnitPipe } from '@myiotgrid/shared/utils';

@Component({
  selector: 'myiotgrid-node-list',
  standalone: true,
  imports: [
    CommonModule,
    RouterModule,
    MatCardModule,
    MatIconModule,
    MatButtonModule,
    MatChipsModule,
    MatFormFieldModule,
    MatSelectModule,
    FormsModule,
    LoadingSpinnerComponent,
    EmptyStateComponent,
    RelativeTimePipe,
    SensorUnitPipe
  ],
  templateUrl: './node-list.component.html',
  styleUrl: './node-list.component.scss'
})
export class NodeListComponent implements OnInit {
  private readonly router = inject(Router);
  private readonly hubApiService = inject(HubApiService);
  private readonly nodeApiService = inject(NodeApiService);
  private readonly readingApiService = inject(ReadingApiService);
  private readonly sensorTypeApiService = inject(SensorTypeApiService);

  readonly isLoading = signal(true);
  readonly initialLoadDone = signal(false);
  readonly hubs = signal<Hub[]>([]);
  readonly nodes = signal<Node[]>([]);
  readonly latestReadings = signal<Map<string, Reading[]>>(new Map());

  filterStatus: 'all' | 'online' | 'offline' = 'all';

  get filteredNodes(): Node[] {
    const all = this.nodes();
    switch (this.filterStatus) {
      case 'online':
        return all.filter(n => n.isOnline);
      case 'offline':
        return all.filter(n => !n.isOnline);
      default:
        return all;
    }
  }

  async ngOnInit(): Promise<void> {
    // Load SensorTypes for icons/colors
    this.sensorTypeApiService.getAll().subscribe();
    await this.loadNodes();
  }

  private async loadNodes(): Promise<void> {
    this.isLoading.set(true);
    try {
      // Load all hubs
      const hubs = await this.hubApiService.getAll().toPromise();
      this.hubs.set(hubs || []);

      // Load all nodes
      const nodes = await this.nodeApiService.getAll().toPromise();
      this.nodes.set(nodes || []);

      // Load latest readings
      const readings = await this.readingApiService.getLatest().toPromise();
      if (readings) {
        const readingMap = new Map<string, Reading[]>();
        readings.forEach(r => {
          const existing = readingMap.get(r.nodeId) || [];
          existing.push(r);
          readingMap.set(r.nodeId, existing);
        });
        this.latestReadings.set(readingMap);
      }
    } catch (error) {
      console.error('Error loading nodes:', error);
    } finally {
      this.isLoading.set(false);
      this.initialLoadDone.set(true);
    }
  }

  getLatestReadings(nodeId: string): Reading[] {
    return this.latestReadings().get(nodeId) || [];
  }

  getHubName(hubId: string): string {
    const hub = this.hubs().find(h => h.id === hubId);
    return hub?.name || 'Unbekannt';
  }

  getNodeIcon(node: Node): string {
    const protocol = node.protocol;
    switch (protocol) {
      case 1: // WLAN
        return 'wifi';
      case 2: // LoRaWAN
        return 'cell_tower';
      default:
        return 'router';
    }
  }

  getSensorIcon(typeId: string): string {
    return this.sensorTypeApiService.getIcon(typeId);
  }

  getSensorColor(typeId: string): string {
    return this.sensorTypeApiService.getColor(typeId);
  }

  onCreate(): void {
    this.router.navigate(['/nodes', 'new']);
  }
}
