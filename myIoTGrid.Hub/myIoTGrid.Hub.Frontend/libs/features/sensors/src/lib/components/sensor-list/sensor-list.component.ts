import { Component, OnInit, inject, signal } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule } from '@angular/router';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatChipsModule } from '@angular/material/chips';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSelectModule } from '@angular/material/select';
import { FormsModule } from '@angular/forms';
import { SensorApiService, NodeApiService, SensorTypeApiService } from '@myiotgrid/shared/data-access';
import { Sensor, Node } from '@myiotgrid/shared/models';
import { LoadingSpinnerComponent, EmptyStateComponent } from '@myiotgrid/shared/ui';

/**
 * SensorListComponent - Lists physical sensor chips (DHT22, BME280, etc.)
 * Matter-konform: Corresponds to Matter Endpoints
 */
@Component({
  selector: 'myiotgrid-sensor-list',
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
    EmptyStateComponent
  ],
  templateUrl: './sensor-list.component.html',
  styleUrl: './sensor-list.component.scss'
})
export class SensorListComponent implements OnInit {
  private readonly sensorApiService = inject(SensorApiService);
  private readonly nodeApiService = inject(NodeApiService);
  private readonly sensorTypeApiService = inject(SensorTypeApiService);

  readonly isLoading = signal(true);
  readonly sensors = signal<Sensor[]>([]);
  readonly nodes = signal<Node[]>([]);

  filterStatus: 'all' | 'active' | 'inactive' = 'all';
  filterNode: string = '';

  get filteredSensors(): Sensor[] {
    let result = this.sensors();

    switch (this.filterStatus) {
      case 'active':
        result = result.filter(s => s.isActive);
        break;
      case 'inactive':
        result = result.filter(s => !s.isActive);
        break;
    }

    if (this.filterNode) {
      result = result.filter(s => s.nodeId === this.filterNode);
    }

    return result;
  }

  async ngOnInit(): Promise<void> {
    // Load SensorTypes for icons/colors
    this.sensorTypeApiService.getAll().subscribe();
    await this.loadSensors();
  }

  private async loadSensors(): Promise<void> {
    this.isLoading.set(true);
    try {
      // Load all nodes for the filter dropdown
      const nodes = await this.nodeApiService.getAll().toPromise();
      this.nodes.set(nodes || []);

      // Load all sensors
      const sensors = await this.sensorApiService.getAll().toPromise();
      this.sensors.set(sensors || []);
    } catch (error) {
      console.error('Error loading sensors:', error);
    } finally {
      this.isLoading.set(false);
    }
  }

  getNodeName(nodeId: string): string {
    const node = this.nodes().find(n => n.id === nodeId);
    return node?.name || node?.nodeId || 'Unbekannt';
  }

  getSensorIcon(typeId: string): string {
    return this.sensorTypeApiService.getIcon(typeId);
  }

  getSensorColor(typeId: string): string {
    return this.sensorTypeApiService.getColor(typeId);
  }

  getSensorDisplayName(typeId: string): string {
    return this.sensorTypeApiService.getDisplayName(typeId);
  }
}
