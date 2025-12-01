/**
 * Communication protocol used by sensors.
 * Matches backend CommunicationProtocolDto enum.
 */
export enum CommunicationProtocol {
  I2C = 1,
  SPI = 2,
  OneWire = 3,
  Analog = 4,
  UART = 5,
  Digital = 6,
  UltraSonic = 7
}

/**
 * SensorTypeCapability - Measurement capability of a sensor
 * Matches backend SensorTypeCapabilityDto
 */
export interface SensorTypeCapability {
  id: string;
  measurementType: string;
  displayName: string;
  unit: string;
  minValue?: number;
  maxValue?: number;
  resolution: number;
  accuracy: number;
  matterClusterId?: number;
  matterClusterName?: string;
  sortOrder: number;
  isActive: boolean;
}

/**
 * SensorType model - corresponds to Backend SensorTypeDto
 * Defines what a sensor CAN do (hardware library).
 */
export interface SensorType {
  [key: string]: unknown;
  id: string;
  code: string;
  name: string;
  manufacturer?: string;
  datasheetUrl?: string;
  description?: string;
  protocol: CommunicationProtocol;
  defaultI2CAddress?: string;
  defaultSdaPin?: number;
  defaultSclPin?: number;
  defaultOneWirePin?: number;
  defaultAnalogPin?: number;
  defaultDigitalPin?: number;
  defaultTriggerPin?: number;
  defaultEchoPin?: number;
  defaultIntervalSeconds: number;
  minIntervalSeconds: number;
  warmupTimeMs: number;
  defaultOffsetCorrection: number;
  defaultGainCorrection: number;
  category: string;
  icon?: string;
  color?: string;
  isGlobal: boolean;
  isActive: boolean;
  capabilities: SensorTypeCapability[];
  createdAt: string;
  updatedAt: string;
}

/**
 * DTO for creating a SensorTypeCapability
 * Matches backend CreateSensorTypeCapabilityDto
 */
export interface CreateSensorTypeCapabilityDto {
  measurementType: string;
  displayName: string;
  unit: string;
  minValue?: number;
  maxValue?: number;
  resolution?: number;
  accuracy?: number;
  matterClusterId?: number;
  matterClusterName?: string;
  sortOrder?: number;
}

/**
 * DTO for creating a SensorType
 * Matches backend CreateSensorTypeDto
 */
export interface CreateSensorTypeDto {
  code: string;
  name: string;
  protocol: CommunicationProtocol;
  manufacturer?: string;
  datasheetUrl?: string;
  description?: string;
  defaultI2CAddress?: string;
  defaultSdaPin?: number;
  defaultSclPin?: number;
  defaultOneWirePin?: number;
  defaultAnalogPin?: number;
  defaultDigitalPin?: number;
  defaultTriggerPin?: number;
  defaultEchoPin?: number;
  defaultIntervalSeconds?: number;
  minIntervalSeconds?: number;
  warmupTimeMs?: number;
  defaultOffsetCorrection?: number;
  defaultGainCorrection?: number;
  category?: string;
  icon?: string;
  color?: string;
  capabilities?: CreateSensorTypeCapabilityDto[];
}

/**
 * DTO for updating a SensorType
 * Matches backend UpdateSensorTypeDto
 */
export interface UpdateSensorTypeDto {
  name?: string;
  manufacturer?: string;
  datasheetUrl?: string;
  description?: string;
  defaultI2CAddress?: string;
  defaultSdaPin?: number;
  defaultSclPin?: number;
  defaultOneWirePin?: number;
  defaultAnalogPin?: number;
  defaultDigitalPin?: number;
  defaultTriggerPin?: number;
  defaultEchoPin?: number;
  defaultIntervalSeconds?: number;
  minIntervalSeconds?: number;
  warmupTimeMs?: number;
  defaultOffsetCorrection?: number;
  defaultGainCorrection?: number;
  category?: string;
  icon?: string;
  color?: string;
  isActive?: boolean;
}

export type SensorCategory = 'climate' | 'water' | 'location' | 'custom' | string;

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

/**
 * Communication protocol labels for UI display
 */
export const COMMUNICATION_PROTOCOL_LABELS: Record<CommunicationProtocol, string> = {
  [CommunicationProtocol.I2C]: 'I²C',
  [CommunicationProtocol.SPI]: 'SPI',
  [CommunicationProtocol.OneWire]: '1-Wire',
  [CommunicationProtocol.Analog]: 'Analog',
  [CommunicationProtocol.UART]: 'UART',
  [CommunicationProtocol.Digital]: 'Digital',
  [CommunicationProtocol.UltraSonic]: 'Ultraschall'
};
