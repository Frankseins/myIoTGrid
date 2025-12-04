import { Component, inject, signal, OnInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormBuilder, FormGroup, Validators, ReactiveFormsModule } from '@angular/forms';
import { MatCardModule } from '@angular/material/card';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';
import { MatDividerModule } from '@angular/material/divider';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatSnackBar, MatSnackBarModule } from '@angular/material/snack-bar';
import { SetupWizardService } from '../../services/setup-wizard.service';
import { NodeApiService, SensorApiService } from '@myiotgrid/shared/data-access';
import { Sensor, CreateSensorDto, CommunicationProtocol, CreateSensorCapabilityDto } from '@myiotgrid/shared/models';

interface SensorTemplate {
  code: string;
  name: string;
  icon: string;
  protocol: CommunicationProtocol;
  category: string;
  capabilities: CreateSensorCapabilityDto[];
}

@Component({
  selector: 'myiotgrid-first-sensor',
  standalone: true,
  imports: [
    CommonModule,
    ReactiveFormsModule,
    MatCardModule,
    MatButtonModule,
    MatIconModule,
    MatFormFieldModule,
    MatInputModule,
    MatSelectModule,
    MatDividerModule,
    MatExpansionModule,
    MatSlideToggleModule,
    MatProgressSpinnerModule,
    MatSnackBarModule
  ],
  templateUrl: './first-sensor.component.html',
  styleUrl: './first-sensor.component.scss'
})
export class FirstSensorComponent implements OnInit {
  private readonly wizardService = inject(SetupWizardService);
  private readonly nodeApiService = inject(NodeApiService);
  private readonly sensorApiService = inject(SensorApiService);
  private readonly snackBar = inject(MatSnackBar);
  private readonly fb = inject(FormBuilder);

  readonly nodeInfo = this.wizardService.nodeInfo;
  readonly isCreating = signal(false);
  readonly showAdvanced = signal(false);
  readonly selectedTemplate = signal<SensorTemplate | null>(null);
  readonly existingSensors = signal<Sensor[]>([]);
  readonly useExisting = signal(false);
  readonly isLoadingSensors = signal(false);

  form!: FormGroup;

  readonly sensorTemplates: SensorTemplate[] = [
    {
      code: 'dht22',
      name: 'DHT22 (Temperatur & Feuchtigkeit)',
      icon: 'thermostat',
      protocol: CommunicationProtocol.Digital,
      category: 'climate',
      capabilities: [
        { measurementType: 'temperature', displayName: 'Temperatur', unit: '°C', minValue: -40, maxValue: 80, resolution: 0.1, accuracy: 0.5 },
        { measurementType: 'humidity', displayName: 'Luftfeuchtigkeit', unit: '%', minValue: 0, maxValue: 100, resolution: 0.1, accuracy: 2 }
      ]
    },
    {
      code: 'bme280',
      name: 'BME280 (Temperatur, Feuchtigkeit, Druck)',
      icon: 'air',
      protocol: CommunicationProtocol.I2C,
      category: 'climate',
      capabilities: [
        { measurementType: 'temperature', displayName: 'Temperatur', unit: '°C', minValue: -40, maxValue: 85, resolution: 0.01, accuracy: 0.5 },
        { measurementType: 'humidity', displayName: 'Luftfeuchtigkeit', unit: '%', minValue: 0, maxValue: 100, resolution: 0.008, accuracy: 3 },
        { measurementType: 'pressure', displayName: 'Luftdruck', unit: 'hPa', minValue: 300, maxValue: 1100, resolution: 0.18, accuracy: 1 }
      ]
    },
    {
      code: 'ds18b20',
      name: 'DS18B20 (Temperatur)',
      icon: 'device_thermostat',
      protocol: CommunicationProtocol.OneWire,
      category: 'climate',
      capabilities: [
        { measurementType: 'temperature', displayName: 'Temperatur', unit: '°C', minValue: -55, maxValue: 125, resolution: 0.0625, accuracy: 0.5 }
      ]
    },
    {
      code: 'bh1750',
      name: 'BH1750 (Licht)',
      icon: 'wb_sunny',
      protocol: CommunicationProtocol.I2C,
      category: 'light',
      capabilities: [
        { measurementType: 'illuminance', displayName: 'Helligkeit', unit: 'lux', minValue: 1, maxValue: 65535, resolution: 1, accuracy: 20 }
      ]
    },
    {
      code: 'soil_capacitive',
      name: 'Kapazitiver Bodenfeuchtesensor',
      icon: 'grass',
      protocol: CommunicationProtocol.Analog,
      category: 'soil',
      capabilities: [
        { measurementType: 'soil_moisture', displayName: 'Bodenfeuchtigkeit', unit: '%', minValue: 0, maxValue: 100, resolution: 1, accuracy: 5 }
      ]
    },
    {
      code: 'hcsr04',
      name: 'HC-SR04 (Ultraschall-Distanz)',
      icon: 'straighten',
      protocol: CommunicationProtocol.UltraSonic,
      category: 'distance',
      capabilities: [
        { measurementType: 'distance', displayName: 'Distanz', unit: 'cm', minValue: 2, maxValue: 400, resolution: 0.3, accuracy: 3 }
      ]
    }
  ];

  readonly protocols = [
    { value: CommunicationProtocol.I2C, label: 'I²C' },
    { value: CommunicationProtocol.SPI, label: 'SPI' },
    { value: CommunicationProtocol.OneWire, label: '1-Wire' },
    { value: CommunicationProtocol.Analog, label: 'Analog' },
    { value: CommunicationProtocol.Digital, label: 'Digital' },
    { value: CommunicationProtocol.UltraSonic, label: 'Ultraschall' }
  ];

  ngOnInit(): void {
    this.initForm();
    this.loadExistingSensors();

    // Pre-select if we have saved sensor
    const savedSensor = this.wizardService.sensor();
    if (savedSensor) {
      const template = this.sensorTemplates.find(t => t.code === savedSensor.code);
      if (template) {
        this.selectTemplate(template);
      }
    }
  }

  private initForm(): void {
    this.form = this.fb.group({
      code: ['', [Validators.required]],
      name: ['', [Validators.required, Validators.minLength(2)]],
      protocol: [CommunicationProtocol.I2C, [Validators.required]],
      category: ['climate'],
      intervalSeconds: [60, [Validators.required, Validators.min(1), Validators.max(3600)]],
      // Advanced options
      i2cAddress: [''],
      digitalPin: [null],
      analogPin: [null],
      oneWirePin: [null],
      triggerPin: [null],
      echoPin: [null]
    });
  }

  private loadExistingSensors(): void {
    this.isLoadingSensors.set(true);
    this.sensorApiService.getAll().subscribe({
      next: (sensors) => {
        this.existingSensors.set(sensors);
        this.isLoadingSensors.set(false);
      },
      error: (error) => {
        console.error('Error loading sensors:', error);
        this.isLoadingSensors.set(false);
      }
    });
  }

  selectTemplate(template: SensorTemplate): void {
    this.selectedTemplate.set(template);
    this.useExisting.set(false);

    this.form.patchValue({
      code: template.code,
      name: template.name,
      protocol: template.protocol,
      category: template.category
    });

    // Set default pins based on protocol
    this.updateDefaultPins(template.protocol);
  }

  selectExistingSensor(sensor: Sensor): void {
    this.selectedTemplate.set(null);
    this.useExisting.set(true);

    this.form.patchValue({
      code: sensor.code,
      name: sensor.name,
      protocol: sensor.protocol,
      category: sensor.category,
      intervalSeconds: sensor.intervalSeconds,
      i2cAddress: sensor.i2cAddress || '',
      digitalPin: sensor.digitalPin,
      analogPin: sensor.analogPin,
      oneWirePin: sensor.oneWirePin,
      triggerPin: sensor.triggerPin,
      echoPin: sensor.echoPin
    });
  }

  private updateDefaultPins(protocol: CommunicationProtocol): void {
    switch (protocol) {
      case CommunicationProtocol.I2C:
        this.form.patchValue({ i2cAddress: '0x76' });
        break;
      case CommunicationProtocol.Digital:
        this.form.patchValue({ digitalPin: 4 });
        break;
      case CommunicationProtocol.Analog:
        this.form.patchValue({ analogPin: 34 });
        break;
      case CommunicationProtocol.OneWire:
        this.form.patchValue({ oneWirePin: 4 });
        break;
      case CommunicationProtocol.UltraSonic:
        this.form.patchValue({ triggerPin: 5, echoPin: 18 });
        break;
    }
  }

  toggleAdvanced(): void {
    this.showAdvanced.update(v => !v);
  }

  async onComplete(): Promise<void> {
    if (this.form.invalid) {
      this.form.markAllAsTouched();
      return;
    }

    this.isCreating.set(true);

    try {
      // Build CreateNodeDto with sensor
      const nodeDto = this.wizardService.buildCreateNodeDto();
      if (!nodeDto) {
        throw new Error('Invalid wizard state');
      }

      // Build sensor DTO
      const template = this.selectedTemplate();
      const sensorDto: CreateSensorDto = {
        code: this.form.value.code,
        name: this.form.value.name,
        protocol: this.form.value.protocol,
        category: this.form.value.category,
        intervalSeconds: this.form.value.intervalSeconds,
        i2cAddress: this.form.value.i2cAddress || undefined,
        digitalPin: this.form.value.digitalPin || undefined,
        analogPin: this.form.value.analogPin || undefined,
        oneWirePin: this.form.value.oneWirePin || undefined,
        triggerPin: this.form.value.triggerPin || undefined,
        echoPin: this.form.value.echoPin || undefined,
        capabilities: template?.capabilities || []
      };

      // Save sensor to wizard state
      this.wizardService.setSensor(sensorDto);

      // Add sensor to node DTO
      nodeDto.sensors = [sensorDto];

      // Create the node via API
      const createdNode = await this.nodeApiService.create(nodeDto).toPromise();

      if (createdNode) {
        this.wizardService.complete(createdNode);
      } else {
        throw new Error('Failed to create node');
      }
    } catch (error) {
      console.error('Error creating node:', error);
      this.snackBar.open('Fehler beim Erstellen des Nodes', 'Schließen', { duration: 5000 });
      this.isCreating.set(false);
    }
  }

  onSkip(): void {
    this.wizardService.skipSensorStep();
  }

  onBack(): void {
    this.wizardService.previousStep();
  }

  onCancel(): void {
    this.wizardService.exitWizard();
  }

  getProtocolLabel(protocol: CommunicationProtocol): string {
    const found = this.protocols.find(p => p.value === protocol);
    return found?.label || 'Unbekannt';
  }
}
