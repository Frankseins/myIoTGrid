import { Component, OnInit, inject, signal, input, computed } from '@angular/core';
import { CommonModule } from '@angular/common';
import { Router, RouterModule } from '@angular/router';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { MatChipsModule } from '@angular/material/chips';
import { SensorApiService, NodeApiService, SensorTypeApiService } from '@myiotgrid/shared/data-access';
import { Sensor, Node } from '@myiotgrid/shared/models';
import { LoadingSpinnerComponent, EmptyStateComponent } from '@myiotgrid/shared/ui';
import { RelativeTimePipe } from '@myiotgrid/shared/utils';

/**
 * SensorDetailComponent - Shows details of a physical sensor chip
 * Matter-konform: Corresponds to a Matter Endpoint
 */
@Component({
  selector: 'myiotgrid-sensor-detail',
  standalone: true,
  imports: [
    CommonModule,
    RouterModule,
    MatCardModule,
    MatIconModule,
    MatButtonModule,
    MatChipsModule,
    LoadingSpinnerComponent,
    EmptyStateComponent,
    RelativeTimePipe
  ],
  templateUrl: './sensor-detail.component.html',
  styleUrl: './sensor-detail.component.scss'
})
export class SensorDetailComponent implements OnInit {
  private readonly router = inject(Router);
  private readonly sensorApiService = inject(SensorApiService);
  private readonly nodeApiService = inject(NodeApiService);
  private readonly sensorTypeApiService = inject(SensorTypeApiService);

  id = input.required<string>();

  readonly isLoading = signal(true);
  readonly sensor = signal<Sensor | null>(null);
  readonly node = signal<Node | null>(null);

  readonly sensorIcon = computed(() => {
    const typeId = this.sensor()?.sensorTypeId;
    return typeId ? this.sensorTypeApiService.getIcon(typeId) : 'memory';
  });

  readonly sensorColor = computed(() => {
    const typeId = this.sensor()?.sensorTypeId;
    return typeId ? this.sensorTypeApiService.getColor(typeId) : '#666666';
  });

  async ngOnInit(): Promise<void> {
    // Load SensorTypes for icons/colors
    this.sensorTypeApiService.getAll().subscribe();
    await this.loadSensor();
  }

  private async loadSensor(): Promise<void> {
    this.isLoading.set(true);
    try {
      const sensor = await this.sensorApiService.getById(this.id()).toPromise();
      this.sensor.set(sensor || null);

      if (sensor) {
        const node = await this.nodeApiService.getById(sensor.nodeId).toPromise();
        this.node.set(node || null);
      }
    } catch (error) {
      console.error('Error loading sensor:', error);
    } finally {
      this.isLoading.set(false);
    }
  }

  getSensorDisplayName(typeId: string): string {
    return this.sensorTypeApiService.getDisplayName(typeId);
  }

  goBack(): void {
    this.router.navigate(['/sensors']);
  }

  goToNode(): void {
    const nodeId = this.sensor()?.nodeId;
    if (nodeId) {
      this.router.navigate(['/nodes', nodeId]);
    }
  }
}
