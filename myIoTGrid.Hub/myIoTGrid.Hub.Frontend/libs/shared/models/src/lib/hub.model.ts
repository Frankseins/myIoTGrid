import { Location } from './location.model';
import { Protocol } from './enums.model';

/**
 * Hub models - corresponds to Backend HubDto
 * Single-Hub-Architecture: Only one Hub per installation
 */
export interface Hub {
  [key: string]: unknown;
  id: string;
  tenantId: string;
  hubId: string;
  name: string;
  description?: string;
  protocol?: Protocol;
  defaultLocation?: Location;
  lastSeen?: string;
  isOnline: boolean;
  metadata?: string;
  createdAt: string;
  sensorCount?: number;
}

/**
 * Hub status information
 */
export interface HubStatus {
  isOnline: boolean;
  lastSeen?: string;
  nodeCount: number;
  onlineNodeCount: number;
}

export interface CreateHubDto {
  hubId: string;
  name: string;
  description?: string;
  protocol?: Protocol;
  defaultLocation?: Location;
  metadata?: string;
}

export interface UpdateHubDto {
  name?: string;
  description?: string;
  protocol?: Protocol;
  defaultLocation?: Location;
  metadata?: string;
}
