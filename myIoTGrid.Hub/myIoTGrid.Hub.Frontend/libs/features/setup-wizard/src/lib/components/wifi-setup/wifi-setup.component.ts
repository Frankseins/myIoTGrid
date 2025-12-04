import { Component, inject, signal, OnInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormBuilder, FormGroup, Validators, ReactiveFormsModule } from '@angular/forms';
import { MatCardModule } from '@angular/material/card';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatDividerModule } from '@angular/material/divider';
import { MatSelectModule } from '@angular/material/select';
import { SetupWizardService, WifiNetwork, WifiCredentials } from '../../services/setup-wizard.service';

type ConfigState = 'idle' | 'scanning' | 'configuring' | 'success' | 'error';

@Component({
  selector: 'myiotgrid-wifi-setup',
  standalone: true,
  imports: [
    CommonModule,
    ReactiveFormsModule,
    MatCardModule,
    MatButtonModule,
    MatIconModule,
    MatFormFieldModule,
    MatInputModule,
    MatProgressSpinnerModule,
    MatDividerModule,
    MatSelectModule
  ],
  templateUrl: './wifi-setup.component.html',
  styleUrl: './wifi-setup.component.scss'
})
export class WifiSetupComponent implements OnInit {
  private readonly wizardService = inject(SetupWizardService);
  private readonly fb = inject(FormBuilder);

  readonly configState = signal<ConfigState>('idle');
  readonly networks = signal<WifiNetwork[]>([]);
  readonly showPassword = signal(false);
  readonly errorMessage = signal<string | null>(null);

  form!: FormGroup;

  readonly bleDevice = this.wizardService.bleDevice;

  ngOnInit(): void {
    this.initForm();
    this.scanNetworks();

    // Pre-fill if we have saved credentials
    const savedCredentials = this.wizardService.wifiCredentials();
    if (savedCredentials) {
      this.form.patchValue({
        ssid: savedCredentials.ssid,
        password: savedCredentials.password
      });
    }
  }

  private initForm(): void {
    this.form = this.fb.group({
      ssid: ['', [Validators.required, Validators.minLength(1), Validators.maxLength(32)]],
      password: ['', [Validators.required, Validators.minLength(8)]]
    });
  }

  scanNetworks(): void {
    this.configState.set('scanning');
    this.errorMessage.set(null);

    // Simulate network scan - in real implementation, this would
    // communicate with the ESP32 via BLE to get available networks
    setTimeout(() => {
      const simulatedNetworks: WifiNetwork[] = [
        { ssid: 'Zuhause', rssi: -45, secured: true },
        { ssid: 'FRITZ!Box 7590', rssi: -52, secured: true },
        { ssid: 'Gast-WLAN', rssi: -65, secured: true },
        { ssid: 'Nachbar_5G', rssi: -78, secured: true },
        { ssid: 'OpenWiFi', rssi: -82, secured: false }
      ];

      this.networks.set(simulatedNetworks);
      this.configState.set('idle');
    }, 1500);
  }

  selectNetwork(network: WifiNetwork): void {
    this.form.patchValue({ ssid: network.ssid });
    if (!network.secured) {
      this.form.patchValue({ password: '' });
      this.form.get('password')?.clearValidators();
      this.form.get('password')?.updateValueAndValidity();
    } else {
      this.form.get('password')?.setValidators([Validators.required, Validators.minLength(8)]);
      this.form.get('password')?.updateValueAndValidity();
    }
  }

  togglePasswordVisibility(): void {
    this.showPassword.update(v => !v);
  }

  getSignalStrengthIcon(rssi: number): string {
    if (rssi > -50) return 'signal_wifi_4_bar';
    if (rssi > -60) return 'network_wifi_3_bar';
    if (rssi > -70) return 'network_wifi_2_bar';
    return 'network_wifi_1_bar';
  }

  async onConfigure(): Promise<void> {
    if (this.form.invalid) {
      this.form.markAllAsTouched();
      return;
    }

    const credentials: WifiCredentials = {
      ssid: this.form.value.ssid,
      password: this.form.value.password
    };

    this.configState.set('configuring');
    this.errorMessage.set(null);

    // Simulate sending WiFi credentials to ESP32 via BLE
    // In real implementation, this would use Web Bluetooth GATT
    await new Promise(resolve => setTimeout(resolve, 2000));

    // Simulate connection test
    const success = Math.random() > 0.1; // 90% success rate

    if (success) {
      this.wizardService.setWifiCredentials(credentials);
      this.configState.set('success');

      // Auto-advance after brief delay
      setTimeout(() => {
        this.wizardService.nextStep();
      }, 1000);
    } else {
      this.configState.set('error');
      this.errorMessage.set('Verbindung fehlgeschlagen. Bitte überprüfen Sie das Passwort.');
    }
  }

  onBack(): void {
    this.wizardService.previousStep();
  }

  onCancel(): void {
    this.wizardService.exitWizard();
  }
}
