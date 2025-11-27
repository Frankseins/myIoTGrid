import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { BaseApiService } from './base-api.service';
import { SensorData, CreateSensorDataDto, SensorDataFilter, PaginatedResult } from '@myiotgrid/shared/models';

@Injectable({ providedIn: 'root' })
export class SensorDataApiService extends BaseApiService {
  private readonly endpoint = '/sensordata';

  /**
   * Get filtered/paginated sensor data
   * GET /api/sensordata
   */
  getFiltered(filter: SensorDataFilter): Observable<PaginatedResult<SensorData>> {
    return this.get<PaginatedResult<SensorData>>(this.endpoint, filter as Record<string, unknown>);
  }

  /**
   * Get sensor data by ID
   * GET /api/sensordata/{id}
   */
  getById(id: string): Observable<SensorData> {
    return this.get<SensorData>(`${this.endpoint}/${id}`);
  }

  /**
   * Get latest measurements for all hubs
   * GET /api/sensordata/latest
   */
  getLatest(): Observable<SensorData[]> {
    return this.get<SensorData[]>(`${this.endpoint}/latest`);
  }

  /**
   * Get latest measurements for specific hub
   * GET /api/sensordata/latest/{hubId}
   */
  getLatestByHub(hubId: string): Observable<SensorData[]> {
    return this.get<SensorData[]>(`${this.endpoint}/latest/${hubId}`);
  }

  /**
   * Create new sensor measurement
   * POST /api/sensordata
   */
  create(dto: CreateSensorDataDto): Observable<SensorData> {
    return this.post<SensorData>(this.endpoint, dto);
  }
}
