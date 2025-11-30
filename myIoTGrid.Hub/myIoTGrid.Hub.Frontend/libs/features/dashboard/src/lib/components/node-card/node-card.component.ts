import { Component, input, computed, inject } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule } from '@angular/router';
import { MatCardModule } from '@angular/material/card';
import { MatIconModule } from '@angular/material/icon';
import { MatChipsModule } from '@angular/material/chips';
import { MatTooltipModule } from '@angular/material/tooltip';
import { Node, Reading, Protocol } from '@myiotgrid/shared/models';
import { SensorTypeApiService } from '@myiotgrid/shared/data-access';
import { RelativeTimePipe, SensorUnitPipe } from '@myiotgrid/shared/utils';

@Component({
  selector: 'myiotgrid-node-card',
  standalone: true,
  imports: [
    CommonModule,
    RouterModule,
    MatCardModule,
    MatIconModule,
    MatChipsModule,
    MatTooltipModule,
    RelativeTimePipe,
    SensorUnitPipe
  ],
  templateUrl: './node-card.component.html',
  styleUrl: './node-card.component.scss'
})
export class NodeCardComponent {
  private readonly sensorTypeService = inject(SensorTypeApiService);

  node = input.required<Node>();
  latestReadings = input<Reading[]>([]);

  readonly locationName = computed(() =>
    this.node().location?.name || 'Unbekannt'
  );

  readonly protocolIcon = computed(() => {
    switch (this.node().protocol) {
      case Protocol.WLAN:
        return 'wifi';
      case Protocol.LoRaWAN:
        return 'cell_tower';
      default:
        return 'device_hub';
    }
  });

  readonly protocolLabel = computed(() => {
    switch (this.node().protocol) {
      case Protocol.WLAN:
        return 'WLAN';
      case Protocol.LoRaWAN:
        return 'LoRaWAN';
      default:
        return 'Unbekannt';
    }
  });

  readonly primaryReading = computed(() => {
    const readings = this.latestReadings();
    if (readings.length === 0) return undefined;
    // Return first reading as primary
    return readings[0];
  });

  readonly secondaryReadings = computed(() => {
    const readings = this.latestReadings();
    if (readings.length <= 1) return [];
    return readings.slice(1, 4); // Max 3 secondary readings
  });

  readonly sensorCount = computed(() =>
    this.node().sensors?.length || 0
  );

  getIconForSensorType(sensorTypeId: string): string {
    const icon = this.sensorTypeService.getIcon(sensorTypeId);
    if (icon && icon !== 'sensors') return icon;

    // Fallback based on common sensor types
    const displayName = this.sensorTypeService.getDisplayName(sensorTypeId)?.toLowerCase() || '';
    if (displayName.includes('temp')) return 'thermostat';
    if (displayName.includes('humid') || displayName.includes('feucht')) return 'water_drop';
    if (displayName.includes('co2')) return 'air';
    if (displayName.includes('druck') || displayName.includes('press')) return 'speed';
    if (displayName.includes('licht') || displayName.includes('light')) return 'light_mode';
    return 'sensors';
  }

  getColorForSensorType(sensorTypeId: string): string {
    return this.sensorTypeService.getColor(sensorTypeId);
  }
}
