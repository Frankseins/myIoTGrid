import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { BaseApiService } from './base-api.service';
import { Node, CreateNodeDto, UpdateNodeDto, Sensor } from '@myiotgrid/shared/models';

@Injectable({ providedIn: 'root' })
export class NodeApiService extends BaseApiService {
  private readonly endpoint = '/nodes';

  /**
   * Get all nodes
   * GET /api/nodes
   */
  getAll(): Observable<Node[]> {
    return this.get<Node[]>(this.endpoint);
  }

  /**
   * Get node by ID
   * GET /api/nodes/{id}
   */
  getById(id: string): Observable<Node> {
    return this.get<Node>(`${this.endpoint}/${id}`);
  }

  /**
   * Get nodes for a specific hub
   * GET /api/nodes?hubId={hubId}
   */
  getByHubId(hubId: string): Observable<Node[]> {
    return this.get<Node[]>(this.endpoint, { hubId });
  }

  /**
   * Register/auto-register a node
   * POST /api/nodes/register
   */
  register(dto: CreateNodeDto): Observable<Node> {
    return this.post<Node>(`${this.endpoint}/register`, dto);
  }

  /**
   * Create new node
   * POST /api/nodes
   */
  create(dto: CreateNodeDto): Observable<Node> {
    return this.post<Node>(this.endpoint, dto);
  }

  /**
   * Update node
   * PUT /api/nodes/{id}
   */
  update(id: string, dto: UpdateNodeDto): Observable<Node> {
    return this.put<Node>(`${this.endpoint}/${id}`, dto);
  }

  /**
   * Delete node
   * DELETE /api/nodes/{id}
   */
  remove(id: string): Observable<void> {
    return this.delete<void>(`${this.endpoint}/${id}`);
  }

  /**
   * Get sensors for a specific node
   * GET /api/nodes/{nodeId}/sensors
   */
  getSensors(nodeId: string): Observable<Sensor[]> {
    return this.get<Sensor[]>(`${this.endpoint}/${nodeId}/sensors`);
  }

  /**
   * Update node status
   * PUT /api/nodes/{id}/status
   */
  updateStatus(id: string, isOnline: boolean): Observable<void> {
    return this.put<void>(`${this.endpoint}/${id}/status`, { isOnline });
  }
}
