import { Routes } from '@angular/router';
import { SensorTypeListComponent } from './components/sensor-type-list/sensor-type-list.component';
import { SensorTypeFormComponent } from './components/sensor-type-form/sensor-type-form.component';

export const SENSOR_TYPES_ROUTES: Routes = [
  {
    path: '',
    component: SensorTypeListComponent
  },
  {
    path: ':id',
    component: SensorTypeFormComponent
  }
];
