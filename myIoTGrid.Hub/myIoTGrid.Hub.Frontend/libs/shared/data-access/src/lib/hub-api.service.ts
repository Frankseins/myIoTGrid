import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { BaseApiService } from './base-api.service';
import { Hub, Sensor, CreateHubDto, UpdateHubDto } from '@myiotgrid/shared/models';

@Injectable({ providedIn: 'root' })
export class HubApiService extends BaseApiService {
  private readonly endpoint = '/hubs';

  /**
   * Get all registered hubs
   * GET /api/hubs
   */
  getAll(): Observable<Hub[]> {
    return this.get<Hub[]>(this.endpoint);
  }

  /**
   * Get hub by ID
   * GET /api/hubs/{id}
   */
  getById(id: string): Observable<Hub> {
    return this.get<Hub>(`${this.endpoint}/${id}`);
  }

  /**
   * Get all sensors for a hub
   * GET /api/hubs/{id}/sensors
   */
  getSensors(hubId: string): Observable<Sensor[]> {
    return this.get<Sensor[]>(`${this.endpoint}/${hubId}/sensors`);
  }

  /**
   * Create new hub
   * POST /api/hubs
   */
  create(dto: CreateHubDto): Observable<Hub> {
    return this.post<Hub>(this.endpoint, dto);
  }

  /**
   * Update hub
   * PUT /api/hubs/{id}
   */
  update(id: string, dto: UpdateHubDto): Observable<Hub> {
    return this.put<Hub>(`${this.endpoint}/${id}`, dto);
  }
}
