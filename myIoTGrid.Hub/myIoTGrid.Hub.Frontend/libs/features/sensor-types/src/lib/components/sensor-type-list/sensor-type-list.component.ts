// ================================
// SensorType List Component
// Verwaltung aller Sensor-Typen mit CRUD
// Verwendet Route-basiertes Detail-Formular
// ================================

import { Component, OnInit, ChangeDetectionStrategy, ChangeDetectorRef } from '@angular/core';
import { CommonModule } from '@angular/common';
import { Router } from '@angular/router';

// Angular Material
import { MatIconModule } from '@angular/material/icon';
import { MatSnackBar, MatSnackBarModule } from '@angular/material/snack-bar';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSelectModule } from '@angular/material/select';
import { MatInputModule } from '@angular/material/input';
import { FormsModule } from '@angular/forms';

// Services & DTOs
import { SensorTypeApiService } from '@myiotgrid/shared/data-access';
import {
    SensorType,
    CommunicationProtocol,
    COMMUNICATION_PROTOCOL_LABELS
} from '@myiotgrid/shared/models';

// GenericTable
import {
    GenericTableComponent,
    GenericTableColumn,
    MaterialLazyEvent,
    ColumnTemplateDirective,
    EmptyStateComponent,
    LoadingSpinnerComponent
} from '@myiotgrid/shared/ui';

interface SensorTypeQueryDto {
    page: number;
    size: number;
    sort: string;
    search?: string;
    manufacturer?: string;
    category?: string;
    protocol?: number;
}

@Component({
  selector: 'myiotgrid-sensor-type-list',
  standalone: true,
  templateUrl: './sensor-type-list.component.html',
  styleUrls: ['./sensor-type-list.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
  imports: [
    CommonModule,
    FormsModule,
    MatIconModule,
    MatSnackBarModule,
    MatFormFieldModule,
    MatSelectModule,
    MatInputModule,
    GenericTableComponent,
    ColumnTemplateDirective,
    EmptyStateComponent,
    LoadingSpinnerComponent,
  ],
})
export class SensorTypeListComponent implements OnInit {
  sensorTypes: SensorType[] = [];
  totalRecords = 0;
  loading = false;
  initialLoadDone = false;
  globalFilter = '';

  columns: GenericTableColumn[] = [
    { field: 'icon', header: '', width: '60px', sortable: false },
    { field: 'code', header: 'Code', width: '150px', sortable: true },
    { field: 'name', header: 'Name', sortable: true },
    {
      field: 'manufacturer',
      header: 'Hersteller',
      width: '150px',
      sortable: true,
    },
    { field: 'category', header: 'Kategorie', width: '120px', sortable: true },
    { field: 'protocol', header: 'Protokoll', width: '100px', sortable: true },
    {
      field: 'isGlobal',
      header: 'Global',
      width: '80px',
      sortable: true,
      type: 'boolean',
    },
    {
      field: 'createdAt',
      header: 'Erstellt',
      width: '160px',
      sortable: true,
      type: 'date',
    },
  ];

  // Category options für Label-Mapping und Filter
  categories = [
    { value: 'climate', label: 'Klima' },
    { value: 'water', label: 'Wasser' },
    { value: 'location', label: 'Standort' },
    { value: 'custom', label: 'Benutzerdefiniert' },
  ];

  // Protokoll-Optionen für Filter
  protocols = Object.entries(COMMUNICATION_PROTOCOL_LABELS).map(([key, label]) => ({
    value: parseInt(key),
    label
  }));

  // Aktuelle Filter-Werte
  currentFilters: Record<string, any> = {};

  private lastQueryParams: SensorTypeQueryDto | null = null;
  private lastSearchTerm = '';

  // Mapping von Frontend-Feldnamen (camelCase) zu Backend-Feldnamen (PascalCase)
  private readonly fieldMapping: Record<string, string> = {
    id: 'Id',
    code: 'Code',
    name: 'Name',
    manufacturer: 'Manufacturer',
    category: 'Category',
    protocol: 'Protocol',
    isGlobal: 'IsGlobal',
    createdAt: 'CreatedAt',
    updatedAt: 'UpdatedAt',
  };

  constructor(
    private router: Router,
    private sensorTypeApiService: SensorTypeApiService,
    private snackBar: MatSnackBar,
    private cd: ChangeDetectorRef
  ) {}

  ngOnInit(): void {
    this.loadSensorTypesLazy({
      first: 0,
      rows: 10,
      sortField: 'name',
      sortOrder: 1,
      globalFilter: this.globalFilter,
    });
  }

  /**
   * Lazy Loading von SensorTypes (wird von GenericTable aufgerufen)
   */
  loadSensorTypesLazy(event: MaterialLazyEvent): void {
    const currentSearchTerm = event.globalFilter || '';

    // Wenn Suchbegriff 1-2 Zeichen hat, ignoriere das Event
    if (currentSearchTerm.length > 0 && currentSearchTerm.length < 3) {
      this.lastSearchTerm = currentSearchTerm;
      return;
    }

    this.lastSearchTerm = currentSearchTerm;
    this.loading = true;

    // Seite berechnen (Backend erwartet 0-indexed page)
    const page = Math.floor((event.first ?? 0) / (event.rows ?? 10));
    const size = event.rows ?? 10;

    // Sort-String bauen mit Backend-Feldnamen (PascalCase)
    let sort = 'Name,asc';
    if (event.sortField) {
      const direction = event.sortOrder === 1 ? 'asc' : 'desc';
      const backendField =
        this.fieldMapping[event.sortField] || event.sortField;
      sort = `${backendField},${direction}`;
    }

    const search =
      currentSearchTerm.length >= 3 ? currentSearchTerm : undefined;

    // Filter aus Event extrahieren
    const eventFilters = event.filters || this.currentFilters || {};

    // Filters für QueryParams vorbereiten (lowercase Keys für Backend)
    const filters: Record<string, string> = {};
    if (eventFilters['manufacturer']) {
      filters['manufacturer'] = eventFilters['manufacturer'];
    }
    if (eventFilters['category']) {
      filters['category'] = eventFilters['category'];
    }
    if (eventFilters['protocol'] !== undefined && eventFilters['protocol'] !== '') {
      filters['protocol'] = String(eventFilters['protocol']);
    }

    const query: SensorTypeQueryDto = {
      page,
      size,
      sort,
      search,
    };

    this.lastQueryParams = query;

    this.sensorTypeApiService
      .getPaged({
        page: query.page,
        size: query.size,
        sort: query.sort,
        search: query.search,
        filters: Object.keys(filters).length > 0 ? filters : undefined,
      })
      .subscribe({
        next: (data) => {
          this.sensorTypes = data.items;
          this.totalRecords = data.totalRecords;
          this.loading = false;
          this.initialLoadDone = true;
          this.cd.markForCheck();
        },
        error: (error) => {
          console.error('Error loading sensor types:', error);
          this.snackBar.open(
            error.message || 'Fehler beim Laden der Sensortypen',
            'Schließen',
            { duration: 5000, panelClass: ['snackbar-error'] }
          );
          this.loading = false;
          this.initialLoadDone = true;
          this.cd.markForCheck();
        },
      });
  }

  /**
   * Navigiert zum Neu-Formular
   */
  onCreate(): void {
    this.router.navigate(['/sensor-types', 'new']);
  }

  /**
   * Navigiert zum Detail-Formular (wird vom GenericTable über Route gemacht)
   */
  onEdit(_sensorType: SensorType): void {
    // Navigation wird vom GenericTable mit detailMode='route' gehandhabt
  }

  /**
   * Löscht SensorType
   */
  deleteSensorType(sensorType: SensorType): void {
    if (sensorType.isGlobal) {
      this.snackBar.open(
        'Globale Sensortypen können nicht gelöscht werden',
        'Schließen',
        { duration: 3000 }
      );
      return;
    }

    this.sensorTypeApiService.deleteSensorType(sensorType.id).subscribe({
      next: () => {
        this.snackBar.open('Sensortyp erfolgreich gelöscht', 'Schließen', {
          duration: 3000,
          panelClass: ['snackbar-success'],
        });
        this.reloadTable();
      },
      error: (error) => {
        this.snackBar.open(
          error.message || 'Fehler beim Löschen',
          'Schließen',
          { duration: 5000, panelClass: ['snackbar-error'] }
        );
      },
    });
  }

  /**
   * Lädt Tabelle mit den zuletzt verwendeten Parametern neu
   */
  reloadTable(): void {
    if (!this.lastQueryParams) {
      this.loadSensorTypesLazy({
        first: 0,
        rows: 10,
        sortField: 'name',
        sortOrder: 1,
        globalFilter: this.globalFilter,
      });
      return;
    }

    const {
      page = 0,
      size = 10,
      sort = 'Name,asc',
      search = '',
    } = this.lastQueryParams;
    const [backendSortField, direction] = sort.split(',');

    // Reverse-Mapping: Backend PascalCase → Frontend camelCase
    const frontendSortField =
      Object.entries(this.fieldMapping).find(
        ([_, backend]) => backend === backendSortField
      )?.[0] || 'name';

    this.loadSensorTypesLazy({
      first: page * size,
      rows: size,
      sortField: frontendSortField,
      sortOrder: direction === 'desc' ? -1 : 1,
      globalFilter: search || this.globalFilter,
    });
  }

  /**
   * Hilfsmethode: Protokoll Label
   */
  getProtocolLabel(protocol: CommunicationProtocol): string {
    return COMMUNICATION_PROTOCOL_LABELS[protocol] || 'Unbekannt';
  }

  /**
   * Hilfsmethode: Kategorie Label
   */
  getCategoryLabel(category: string): string {
    const cat = this.categories.find((c) => c.value === category);
    return cat?.label || category;
  }

  /**
   * Filter wurden geändert
   */
  onFilterChange(filters: Record<string, any>): void {
    this.currentFilters = filters;
    // Tabelle mit Filtern neu laden
    this.loadSensorTypesLazy({
      first: 0,
      rows: 10,
      sortField: 'name',
      sortOrder: 1,
      globalFilter: this.globalFilter,
      filters: filters
    });
  }
}
