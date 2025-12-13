import { Routes } from '@angular/router';
import { SettingsComponent } from './components/settings/settings.component';
import { CloudSyncComponent } from './components/cloud-sync/cloud-sync.component';

export const SETTINGS_ROUTES: Routes = [
  {
    path: '',
    component: SettingsComponent
  },
  {
    path: 'cloud-sync',
    component: CloudSyncComponent
  }
];
