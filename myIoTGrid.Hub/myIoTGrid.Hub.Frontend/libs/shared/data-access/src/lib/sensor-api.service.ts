import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { BaseApiService } from './base-api.service';
import { Sensor, CreateSensorDto, UpdateSensorDto } from '@myiotgrid/shared/models';

/**
 * API Service for physical Sensor chips (DHT22, BME280, etc.)
 * Matter-konform: Corresponds to Matter Endpoints
 */
@Injectable({ providedIn: 'root' })
export class SensorApiService extends BaseApiService {
  private readonly endpoint = '/sensors';

  /**
   * Get all sensors
   * GET /api/sensors
   */
  getAll(): Observable<Sensor[]> {
    return this.get<Sensor[]>(this.endpoint);
  }

  /**
   * Get sensor by ID
   * GET /api/sensors/{id}
   */
  getById(id: string): Observable<Sensor> {
    return this.get<Sensor>(`${this.endpoint}/${id}`);
  }

  /**
   * Get sensors for a specific node
   * GET /api/sensors?nodeId={nodeId}
   */
  getByNodeId(nodeId: string): Observable<Sensor[]> {
    return this.get<Sensor[]>(this.endpoint, { nodeId });
  }

  /**
   * Create new sensor on a node
   * POST /api/sensors
   */
  create(nodeId: string, dto: CreateSensorDto): Observable<Sensor> {
    return this.post<Sensor>(this.endpoint, { ...dto, nodeId });
  }

  /**
   * Update sensor
   * PUT /api/sensors/{id}
   */
  update(id: string, dto: UpdateSensorDto): Observable<Sensor> {
    return this.put<Sensor>(`${this.endpoint}/${id}`, dto);
  }

  /**
   * Delete sensor
   * DELETE /api/sensors/{id}
   */
  remove(id: string): Observable<void> {
    return this.delete<void>(`${this.endpoint}/${id}`);
  }

  /**
   * Activate/deactivate sensor
   * PUT /api/sensors/{id}/active
   */
  setActive(id: string, isActive: boolean): Observable<Sensor> {
    return this.put<Sensor>(`${this.endpoint}/${id}`, { isActive });
  }
}
