import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { BaseApiService } from './base-api.service';
import { Reading, CreateReadingDto, ReadingFilter, PaginatedResult } from '@myiotgrid/shared/models';

@Injectable({ providedIn: 'root' })
export class ReadingApiService extends BaseApiService {
  private readonly endpoint = '/readings';

  /**
   * Get filtered/paginated readings
   * GET /api/readings
   */
  getFiltered(filter: ReadingFilter): Observable<PaginatedResult<Reading>> {
    return this.get<PaginatedResult<Reading>>(this.endpoint, filter as Record<string, unknown>);
  }

  /**
   * Get reading by ID
   * GET /api/readings/{id}
   */
  getById(id: number): Observable<Reading> {
    return this.get<Reading>(`${this.endpoint}/${id}`);
  }

  /**
   * Get latest readings for all nodes
   * GET /api/readings/latest
   */
  getLatest(): Observable<Reading[]> {
    return this.get<Reading[]>(`${this.endpoint}/latest`);
  }

  /**
   * Get latest readings for specific node
   * GET /api/readings/latest?nodeId={nodeId}
   */
  getLatestByNode(nodeId: string): Observable<Reading[]> {
    return this.get<Reading[]>(`${this.endpoint}/latest`, { nodeId });
  }

  /**
   * Get latest readings for specific hub
   * GET /api/readings/latest?hubId={hubId}
   */
  getLatestByHub(hubId: string): Observable<Reading[]> {
    return this.get<Reading[]>(`${this.endpoint}/latest`, { hubId });
  }

  /**
   * Get readings by node within time range
   * GET /api/readings?nodeId={nodeId}&from={from}&to={to}
   */
  getByNode(nodeId: string, hours: number = 24): Observable<PaginatedResult<Reading>> {
    const from = new Date(Date.now() - hours * 60 * 60 * 1000).toISOString();
    return this.getFiltered({ nodeId, from });
  }

  /**
   * Create new reading
   * POST /api/readings
   */
  create(dto: CreateReadingDto): Observable<Reading> {
    return this.post<Reading>(this.endpoint, dto);
  }
}
