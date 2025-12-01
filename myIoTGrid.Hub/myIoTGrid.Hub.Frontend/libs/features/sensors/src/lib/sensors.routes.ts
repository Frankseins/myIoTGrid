import { Routes } from '@angular/router';
import { SensorListComponent } from './components/sensor-list/sensor-list.component';
import { SensorFormComponent } from './components/sensor-form/sensor-form.component';

export const SENSORS_ROUTES: Routes = [
  {
    path: '',
    component: SensorListComponent
  },
  {
    path: ':id',
    component: SensorFormComponent
  }
];
