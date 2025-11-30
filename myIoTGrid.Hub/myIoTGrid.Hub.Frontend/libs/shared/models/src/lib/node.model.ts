import { Location } from './location.model';
import { Protocol } from './enums.model';
import { Sensor } from './sensor.model';

/**
 * Node model - corresponds to Backend NodeDto
 * Matter-konform: Entspricht einem Matter Node (ESP32/LoRa32 device)
 */
export interface Node {
  id: string;
  hubId: string;
  nodeId: string;
  name: string;
  protocol: Protocol;
  location?: Location;
  sensors: Sensor[];
  lastSeen?: string;
  isOnline: boolean;
  firmwareVersion?: string;
  batteryLevel?: number;
  createdAt: string;
}

export interface CreateNodeDto {
  nodeId: string;
  name?: string;
  hubIdentifier?: string;
  hubId?: string;
  protocol?: Protocol;
  location?: Location;
  sensors?: CreateSensorDto[];
}

export interface UpdateNodeDto {
  name?: string;
  location?: Location;
  firmwareVersion?: string;
}

export interface NodeStatusDto {
  nodeId: string;
  isOnline: boolean;
  lastSeen?: string;
  batteryLevel?: number;
}

// Re-export for convenience
import { CreateSensorDto } from './sensor.model';
