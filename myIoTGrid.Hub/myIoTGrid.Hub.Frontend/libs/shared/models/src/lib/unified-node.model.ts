import { Location } from './location.model';
import { Sensor } from './sensor.model';

/**
 * UnifiedNode model - corresponds to Backend UnifiedNodeDto
 * Unified view of all nodes (Local + Direct + Virtual + OtherHub)
 */
export interface UnifiedNode {
  id: string;
  nodeId: string;
  name: string;
  source: UnifiedNodeSource;
  sourceDetails?: string;
  sensors?: Sensor[];
  location?: Location;
  isOnline: boolean;
  lastSeen?: string;
  latestReadings?: UnifiedReading[];
}

export interface UnifiedReading {
  sensorTypeId: string;
  sensorTypeName: string;
  value: number;
  unit: string;
  timestamp: string;
  source: UnifiedNodeSource;
}

export type UnifiedNodeSource = 'Local' | 'Direct' | 'Virtual' | 'OtherHub';

export enum UnifiedNodeSourceEnum {
  Local = 0,
  Direct = 1,
  Virtual = 2,
  OtherHub = 3
}
