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
import { HubApiService, SensorDataApiService } from '@myiotgrid/shared/data-access';
import { Hub, Sensor, SensorData } from '@myiotgrid/shared/models';
import { LoadingSpinnerComponent, EmptyStateComponent } from '@myiotgrid/shared/ui';
import { RelativeTimePipe, SensorUnitPipe } from '@myiotgrid/shared/utils';

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
    EmptyStateComponent,
    RelativeTimePipe,
    SensorUnitPipe
  ],
  templateUrl: './sensor-list.component.html',
  styleUrl: './sensor-list.component.scss'
})
export class SensorListComponent implements OnInit {
  private readonly hubApiService = inject(HubApiService);
  private readonly sensorDataApiService = inject(SensorDataApiService);

  readonly isLoading = signal(true);
  readonly hubs = signal<Hub[]>([]);
  readonly sensors = signal<Sensor[]>([]);
  readonly latestData = signal<Map<string, SensorData>>(new Map());

  filterStatus: 'all' | 'online' | 'offline' = 'all';

  get filteredSensors(): Sensor[] {
    const all = this.sensors();
    switch (this.filterStatus) {
      case 'online':
        return all.filter(s => s.isOnline);
      case 'offline':
        return all.filter(s => !s.isOnline);
      default:
        return all;
    }
  }

  async ngOnInit(): Promise<void> {
    await this.loadSensors();
  }

  private async loadSensors(): Promise<void> {
    this.isLoading.set(true);
    try {
      // First load all hubs
      const hubs = await this.hubApiService.getAll().toPromise();
      this.hubs.set(hubs || []);

      // Then load sensors for each hub
      if (hubs && hubs.length > 0) {
        const sensorPromises = hubs.map(hub =>
          this.hubApiService.getSensors(hub.id).toPromise()
        );
        const sensorArrays = await Promise.all(sensorPromises);
        const allSensors = sensorArrays.flat().filter((s): s is Sensor => s !== undefined);
        this.sensors.set(allSensors);
      }

      // Load latest sensor data
      const data = await this.sensorDataApiService.getLatest().toPromise();
      if (data) {
        const dataMap = new Map<string, SensorData>();
        data.forEach(d => {
          const key = d.sensorId || d.hubId;
          const existing = dataMap.get(key);
          if (!existing || new Date(d.timestamp) > new Date(existing.timestamp)) {
            dataMap.set(key, d);
          }
        });
        this.latestData.set(dataMap);
      }
    } catch (error) {
      console.error('Error loading sensors:', error);
    } finally {
      this.isLoading.set(false);
    }
  }

  getLatestData(sensorId: string): SensorData | undefined {
    return this.latestData().get(sensorId);
  }

  getHubName(hubId: string): string {
    const hub = this.hubs().find(h => h.id === hubId);
    return hub?.name || 'Unbekannt';
  }

  getSensorIcon(typeCode: string): string {
    switch (typeCode?.toLowerCase()) {
      case 'temperature':
        return 'thermostat';
      case 'humidity':
        return 'water_drop';
      case 'co2':
        return 'air';
      case 'pressure':
        return 'speed';
      case 'light':
        return 'light_mode';
      case 'soil_moisture':
        return 'grass';
      default:
        return 'sensors';
    }
  }
}
