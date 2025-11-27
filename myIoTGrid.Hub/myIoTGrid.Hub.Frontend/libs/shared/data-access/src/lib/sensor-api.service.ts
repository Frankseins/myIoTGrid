import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { BaseApiService } from './base-api.service';
import { Sensor, CreateSensorDto, UpdateSensorDto } from '@myiotgrid/shared/models';

@Injectable({ providedIn: 'root' })
export class SensorApiService extends BaseApiService {
  private readonly endpoint = '/sensors';

  /**
   * Get sensor by ID
   * GET /api/sensors/{id}
   */
  getById(id: string): Observable<Sensor> {
    return this.get<Sensor>(`${this.endpoint}/${id}`);
  }

  /**
   * Get all sensors for a specific hub
   * GET /api/hubs/{hubId}/sensors
   */
  getByHubId(hubId: string): Observable<Sensor[]> {
    return this.get<Sensor[]>(`/hubs/${hubId}/sensors`);
  }

  /**
   * Create new sensor
   * POST /api/sensors
   */
  create(dto: CreateSensorDto): Observable<Sensor> {
    return this.post<Sensor>(this.endpoint, dto);
  }

  /**
   * Register/auto-register sensor
   * POST /api/sensors/register
   */
  register(dto: CreateSensorDto): Observable<Sensor> {
    return this.post<Sensor>(`${this.endpoint}/register`, dto);
  }

  /**
   * Update sensor
   * PUT /api/sensors/{id}
   */
  update(id: string, dto: UpdateSensorDto): Observable<Sensor> {
    return this.put<Sensor>(`${this.endpoint}/${id}`, dto);
  }

  /**
   * Update sensor status
   * PUT /api/sensors/{id}/status
   */
  updateStatus(id: string, isOnline: boolean): Observable<void> {
    return this.put<void>(`${this.endpoint}/${id}/status`, { isOnline });
  }
}
