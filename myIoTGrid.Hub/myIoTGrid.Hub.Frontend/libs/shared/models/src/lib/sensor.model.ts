import { SensorType } from './sensor-type.model';

/**
 * Sensor model - corresponds to Backend SensorDto
 * Matter-konform: Entspricht einem Matter Endpoint (physischer Sensor-Chip: DHT22, BME280, etc.)
 */
export interface Sensor {
  id: string;
  nodeId: string;
  sensorTypeId: string;
  endpointId: number;
  name?: string;
  isActive: boolean;
  sensorType?: SensorType;
  createdAt: string;
}

export interface CreateSensorDto {
  sensorTypeId: string;
  endpointId: number;
  name?: string;
}

export interface UpdateSensorDto {
  name?: string;
  isActive?: boolean;
}
