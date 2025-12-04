import { Component, inject, signal, OnInit, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MatCardModule } from '@angular/material/card';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatListModule } from '@angular/material/list';
import { MatDividerModule } from '@angular/material/divider';
import { MatChipsModule } from '@angular/material/chips';
import { SetupWizardService, BleDevice } from '../../services/setup-wizard.service';

type ScanState = 'idle' | 'scanning' | 'connecting' | 'connected' | 'error';

@Component({
  selector: 'myiotgrid-ble-pairing',
  standalone: true,
  imports: [
    CommonModule,
    MatCardModule,
    MatButtonModule,
    MatIconModule,
    MatProgressSpinnerModule,
    MatListModule,
    MatDividerModule,
    MatChipsModule
  ],
  templateUrl: './ble-pairing.component.html',
  styleUrl: './ble-pairing.component.scss'
})
export class BlePairingComponent implements OnInit, OnDestroy {
  private readonly wizardService = inject(SetupWizardService);

  readonly scanState = signal<ScanState>('idle');
  readonly devices = signal<BleDevice[]>([]);
  readonly selectedDevice = signal<BleDevice | null>(null);
  readonly errorMessage = signal<string | null>(null);
  readonly isBluetoothSupported = signal(true);

  private scanInterval: ReturnType<typeof setInterval> | null = null;

  ngOnInit(): void {
    // Check if Web Bluetooth is supported
    if (!('bluetooth' in navigator)) {
      this.isBluetoothSupported.set(false);
      this.errorMessage.set('Web Bluetooth wird von diesem Browser nicht unterstützt. Bitte verwenden Sie Chrome, Edge oder Opera.');
    }
  }

  ngOnDestroy(): void {
    this.stopScanSimulation();
  }

  async startScan(): Promise<void> {
    this.scanState.set('scanning');
    this.devices.set([]);
    this.errorMessage.set(null);

    // Check for real Bluetooth API
    if ('bluetooth' in navigator) {
      try {
        // Try to use Web Bluetooth API
        const device = await (navigator as Navigator & { bluetooth: Bluetooth }).bluetooth.requestDevice({
          filters: [
            { namePrefix: 'myIoTGrid' },
            { namePrefix: 'ESP32' },
            { namePrefix: 'SIM-' }
          ],
          optionalServices: ['generic_access']
        });

        if (device) {
          const bleDevice: BleDevice = {
            id: device.id,
            name: device.name || 'Unbekanntes Gerät',
            rssi: -50, // Web Bluetooth doesn't expose RSSI directly
            macAddress: this.generateMacFromId(device.id)
          };

          this.devices.set([bleDevice]);
          this.scanState.set('idle');
        }
      } catch (error: unknown) {
        // User cancelled or no device found
        if (error instanceof DOMException && error.name === 'NotFoundError') {
          this.errorMessage.set('Kein Gerät gefunden. Stellen Sie sicher, dass der Node eingeschaltet ist.');
        } else if (error instanceof DOMException && error.name === 'NotAllowedError') {
          // User cancelled - that's OK, just go back to idle
          this.scanState.set('idle');
          return;
        } else {
          // Fallback to simulation
          this.startScanSimulation();
          return;
        }
        this.scanState.set('error');
      }
    } else {
      // Fallback: Simulate scanning for demo purposes
      this.startScanSimulation();
    }
  }

  private startScanSimulation(): void {
    // Simulate device discovery
    const simulatedDevices: BleDevice[] = [];
    let scanCount = 0;

    this.scanInterval = setInterval(() => {
      scanCount++;

      // Add simulated devices progressively
      if (scanCount === 2) {
        simulatedDevices.push({
          id: 'sim-001',
          name: 'myIoTGrid Sensor A',
          rssi: -45,
          macAddress: 'AA:BB:CC:DD:EE:01'
        });
        this.devices.set([...simulatedDevices]);
      }

      if (scanCount === 4) {
        simulatedDevices.push({
          id: 'sim-002',
          name: 'ESP32-Wohnzimmer',
          rssi: -62,
          macAddress: 'AA:BB:CC:DD:EE:02'
        });
        this.devices.set([...simulatedDevices]);
      }

      if (scanCount === 6) {
        simulatedDevices.push({
          id: 'sim-003',
          name: 'SIM-Sensor-Test',
          rssi: -78,
          macAddress: 'AA:BB:CC:DD:EE:03'
        });
        this.devices.set([...simulatedDevices]);
      }

      if (scanCount >= 8) {
        this.stopScanSimulation();
        this.scanState.set('idle');
      }
    }, 500);
  }

  private stopScanSimulation(): void {
    if (this.scanInterval) {
      clearInterval(this.scanInterval);
      this.scanInterval = null;
    }
  }

  stopScan(): void {
    this.stopScanSimulation();
    this.scanState.set('idle');
  }

  selectDevice(device: BleDevice): void {
    this.selectedDevice.set(device);
  }

  async connectToDevice(): Promise<void> {
    const device = this.selectedDevice();
    if (!device) return;

    this.scanState.set('connecting');
    this.errorMessage.set(null);

    // Simulate connection delay
    await new Promise(resolve => setTimeout(resolve, 1500));

    // Simulate successful connection
    this.wizardService.setBleDevice(device);
    this.scanState.set('connected');

    // Auto-advance after brief delay
    setTimeout(() => {
      this.wizardService.nextStep();
    }, 800);
  }

  getSignalStrengthIcon(rssi: number): string {
    if (rssi > -50) return 'signal_cellular_4_bar';
    if (rssi > -60) return 'signal_cellular_3_bar';
    if (rssi > -70) return 'signal_cellular_2_bar';
    return 'signal_cellular_1_bar';
  }

  getSignalStrengthClass(rssi: number): string {
    if (rssi > -50) return 'signal-excellent';
    if (rssi > -60) return 'signal-good';
    if (rssi > -70) return 'signal-fair';
    return 'signal-weak';
  }

  private generateMacFromId(id: string): string {
    // Generate a pseudo-MAC from device ID
    const hash = id.split('').reduce((a, b) => {
      a = ((a << 5) - a) + b.charCodeAt(0);
      return a & a;
    }, 0);

    const bytes = [
      (hash >> 24) & 0xff,
      (hash >> 16) & 0xff,
      (hash >> 8) & 0xff,
      hash & 0xff,
      (hash >> 12) & 0xff,
      (hash >> 4) & 0xff
    ];

    return bytes.map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(':');
  }

  onBack(): void {
    this.wizardService.previousStep();
  }

  onCancel(): void {
    this.wizardService.exitWizard();
  }
}

// Type declarations for Web Bluetooth API
interface Bluetooth {
  requestDevice(options: RequestDeviceOptions): Promise<BluetoothDevice>;
}

interface RequestDeviceOptions {
  filters?: BluetoothLEScanFilter[];
  optionalServices?: BluetoothServiceUUID[];
  acceptAllDevices?: boolean;
}

interface BluetoothLEScanFilter {
  name?: string;
  namePrefix?: string;
  services?: BluetoothServiceUUID[];
}

type BluetoothServiceUUID = string | number;

interface BluetoothDevice {
  id: string;
  name?: string;
}
