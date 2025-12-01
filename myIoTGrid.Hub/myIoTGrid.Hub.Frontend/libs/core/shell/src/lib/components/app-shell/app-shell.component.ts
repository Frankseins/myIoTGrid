import { Component, input } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule } from '@angular/router';
import { NavigationRailComponent } from '@myiotgrid/core/navigation';

@Component({
  selector: 'myiotgrid-app-shell',
  standalone: true,
  imports: [CommonModule, RouterModule, NavigationRailComponent],
  templateUrl: './app-shell.component.html',
  styleUrl: './app-shell.component.scss'
})
export class AppShellComponent {
  showNavigation = input<boolean>(true);
}
