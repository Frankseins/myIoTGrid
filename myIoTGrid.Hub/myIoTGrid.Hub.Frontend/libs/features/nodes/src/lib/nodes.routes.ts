import { Routes } from '@angular/router';
import { NodeListComponent } from './components/node-list/node-list.component';
import { NodeDetailComponent } from './components/node-detail/node-detail.component';

export const NODES_ROUTES: Routes = [
  {
    path: '',
    component: NodeListComponent
  },
  {
    path: ':id',
    component: NodeDetailComponent
  }
];
