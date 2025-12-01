import { Routes } from '@angular/router';
import { NodeListComponent } from './components/node-list/node-list.component';
import { NodeDetailComponent } from './components/node-detail/node-detail.component';
import { NodeFormComponent } from './components/node-form/node-form.component';

export const NODES_ROUTES: Routes = [
  {
    path: '',
    component: NodeListComponent
  },
  {
    path: 'new',
    component: NodeFormComponent
  },
  {
    path: ':id',
    component: NodeDetailComponent
  },
  {
    path: ':id/edit',
    component: NodeFormComponent
  }
];
