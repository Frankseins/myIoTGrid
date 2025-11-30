/**
 * SensorType model - corresponds to Backend SensorTypeDto
 * Matter-konform: Entspricht einem Matter Cluster
 */
export interface SensorType {
  typeId: string;
  displayName: string;
  clusterId: number;
  matterClusterName?: string;
  unit: string;
  resolution: number;
  minValue?: number;
  maxValue?: number;
  description?: string;
  isCustom: boolean;
  category: SensorCategory;
  icon?: string;
  color?: string;
  isGlobal: boolean;
  createdAt: string;
}

export interface CreateSensorTypeDto {
  typeId: string;
  displayName: string;
  clusterId: number;
  unit: string;
  matterClusterName?: string;
  resolution?: number;
  minValue?: number;
  maxValue?: number;
  description?: string;
  isCustom?: boolean;
  category?: SensorCategory;
  icon?: string;
  color?: string;
}

export type SensorCategory = 'weather' | 'water' | 'air' | 'soil' | 'energy' | 'other';

/**
 * Standard Matter Cluster IDs
 */
export const MATTER_CLUSTERS = {
  TEMPERATURE: 0x0402,
  HUMIDITY: 0x0405,
  PRESSURE: 0x0403,
  ILLUMINANCE: 0x0400,
  OCCUPANCY: 0x0406,
  // Custom myIoTGrid (0xFC00+)
  WATER_LEVEL: 0xfc00,
  WATER_TEMPERATURE: 0xfc01,
  PH_VALUE: 0xfc02,
  FLOW_VELOCITY: 0xfc03,
  CO2: 0xfc04,
  PM25: 0xfc05,
  PM10: 0xfc06,
  SOIL_MOISTURE: 0xfc07,
  UV_INDEX: 0xfc08,
  WIND_SPEED: 0xfc09,
  RAINFALL: 0xfc0a,
  BATTERY: 0xfc0b,
  RSSI: 0xfc0c,
} as const;
