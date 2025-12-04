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
import { MatTooltipModule } from '@angular/material/tooltip';
import { SetupWizardService, NodeInfo } from '../../services/setup-wizard.service';

interface IconOption {
  value: string;
  label: string;
}

interface LocationSuggestion {
  name: string;
  icon: string;
}

@Component({
  selector: 'myiotgrid-node-info',
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
    MatTooltipModule
  ],
  templateUrl: './node-info.component.html',
  styleUrl: './node-info.component.scss'
})
export class NodeInfoComponent implements OnInit {
  private readonly wizardService = inject(SetupWizardService);
  private readonly fb = inject(FormBuilder);

  form!: FormGroup;

  readonly bleDevice = this.wizardService.bleDevice;
  readonly selectedIcon = signal<string>('sensors');

  readonly icons: IconOption[] = [
    { value: 'sensors', label: 'Sensor' },
    { value: 'thermostat', label: 'Temperatur' },
    { value: 'water_drop', label: 'Feuchtigkeit' },
    { value: 'air', label: 'Luft' },
    { value: 'wb_sunny', label: 'Licht' },
    { value: 'grass', label: 'Pflanze' },
    { value: 'pool', label: 'Wasser' },
    { value: 'power', label: 'Energie' },
    { value: 'home', label: 'Haus' },
    { value: 'garage', label: 'Garage' },
    { value: 'yard', label: 'Garten' },
    { value: 'store', label: 'Lager' }
  ];

  readonly locationSuggestions: LocationSuggestion[] = [
    { name: 'Wohnzimmer', icon: 'living' },
    { name: 'Schlafzimmer', icon: 'bed' },
    { name: 'Küche', icon: 'kitchen' },
    { name: 'Bad', icon: 'bathroom' },
    { name: 'Büro', icon: 'work' },
    { name: 'Keller', icon: 'stairs' },
    { name: 'Dachboden', icon: 'roofing' },
    { name: 'Garten', icon: 'yard' },
    { name: 'Terrasse', icon: 'deck' },
    { name: 'Garage', icon: 'garage' },
    { name: 'Gewächshaus', icon: 'grass' }
  ];

  ngOnInit(): void {
    this.initForm();

    // Pre-fill if we have saved node info
    const savedInfo = this.wizardService.nodeInfo();
    if (savedInfo) {
      this.form.patchValue({
        name: savedInfo.name,
        location: savedInfo.location,
        icon: savedInfo.icon
      });
      this.selectedIcon.set(savedInfo.icon);
    } else {
      // Generate default name from BLE device
      const device = this.bleDevice();
      if (device) {
        const defaultName = device.name
          .replace(/^myIoTGrid\s*/i, '')
          .replace(/^ESP32-?/i, '')
          .replace(/^SIM-/i, '')
          || 'Neuer Node';
        this.form.patchValue({ name: defaultName });
      }
    }
  }

  private initForm(): void {
    this.form = this.fb.group({
      name: ['', [Validators.required, Validators.minLength(2), Validators.maxLength(50)]],
      location: [''],
      icon: ['sensors']
    });
  }

  selectIcon(icon: string): void {
    this.selectedIcon.set(icon);
    this.form.patchValue({ icon });
  }

  selectLocation(location: LocationSuggestion): void {
    this.form.patchValue({ location: location.name });
  }

  onSave(): void {
    if (this.form.invalid) {
      this.form.markAllAsTouched();
      return;
    }

    const nodeInfo: NodeInfo = {
      name: this.form.value.name,
      location: this.form.value.location || '',
      icon: this.form.value.icon
    };

    this.wizardService.setNodeInfo(nodeInfo);
    this.wizardService.nextStep();
  }

  onBack(): void {
    this.wizardService.previousStep();
  }

  onCancel(): void {
    this.wizardService.exitWizard();
  }
}
