import { Injectable, signal, computed } from '@angular/core';
import { Observable, tap, shareReplay } from 'rxjs';
import { BaseApiService } from './base-api.service';
import { SensorType, CreateSensorTypeDto, SensorCategory } from '@myiotgrid/shared/models';

/**
 * API Service for SensorTypes with caching
 * Matter-konform: Corresponds to Matter Clusters
 */
@Injectable({ providedIn: 'root' })
export class SensorTypeApiService extends BaseApiService {
  private readonly endpoint = '/sensortypes';

  // Cache for SensorTypes (rarely changed)
  private cache$?: Observable<SensorType[]>;

  // Signals for reactive access
  readonly sensorTypes = signal<SensorType[]>([]);
  readonly sensorTypesMap = signal<Map<string, SensorType>>(new Map());
  readonly isLoaded = signal(false);

  // Computed values
  readonly categories = computed(() => {
    const types = this.sensorTypes();
    const uniqueCategories = new Set(types.map(t => t.category));
    return Array.from(uniqueCategories);
  });

  /**
   * Get all sensor types (with caching)
   * GET /api/sensortypes
   */
  getAll(): Observable<SensorType[]> {
    if (!this.cache$) {
      this.cache$ = this.get<SensorType[]>(this.endpoint).pipe(
        tap(types => {
          this.sensorTypes.set(types);
          this.sensorTypesMap.set(new Map(types.map(t => [t.typeId, t])));
          this.isLoaded.set(true);
        }),
        shareReplay(1)
      );
    }
    return this.cache$;
  }

  /**
   * Get sensor type by typeId
   * GET /api/sensortypes/{typeId}
   */
  getById(typeId: string): Observable<SensorType> {
    return this.get<SensorType>(`${this.endpoint}/${typeId}`);
  }

  /**
   * Get sensor types by category
   * GET /api/sensortypes/category/{category}
   */
  getByCategory(category: SensorCategory): Observable<SensorType[]> {
    return this.get<SensorType[]>(`${this.endpoint}/category/${category}`);
  }

  /**
   * Create new sensor type
   * POST /api/sensortypes
   */
  create(dto: CreateSensorTypeDto): Observable<SensorType> {
    return this.post<SensorType>(this.endpoint, dto).pipe(
      tap(() => this.clearCache())
    );
  }

  // ==========================================
  // Synchronous helper methods (after initial load)
  // ==========================================

  /**
   * Get unit for a sensor type (synchronous)
   */
  getUnit(typeId: string): string {
    return this.sensorTypesMap().get(typeId)?.unit ?? 'unknown';
  }

  /**
   * Get display name for a sensor type (synchronous)
   */
  getDisplayName(typeId: string): string {
    return this.sensorTypesMap().get(typeId)?.displayName ?? typeId;
  }

  /**
   * Get icon for a sensor type (synchronous)
   */
  getIcon(typeId: string): string {
    return this.sensorTypesMap().get(typeId)?.icon ?? 'sensors';
  }

  /**
   * Get color for a sensor type (synchronous)
   */
  getColor(typeId: string): string {
    return this.sensorTypesMap().get(typeId)?.color ?? '#666666';
  }

  /**
   * Get full sensor type by typeId (synchronous)
   */
  getType(typeId: string): SensorType | undefined {
    return this.sensorTypesMap().get(typeId);
  }

  /**
   * Get types by category (synchronous)
   */
  getTypesByCategory(category: SensorCategory): SensorType[] {
    return this.sensorTypes().filter(t => t.category === category);
  }

  /**
   * Clear cache (e.g., after admin changes)
   */
  clearCache(): void {
    this.cache$ = undefined;
    this.isLoaded.set(false);
  }
}
