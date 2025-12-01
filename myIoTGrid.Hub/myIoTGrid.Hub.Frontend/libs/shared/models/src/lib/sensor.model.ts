import { SensorType } from './sensor-type.model';

/**
 * Sensor model - corresponds to Backend SensorDto
 * Represents a concrete sensor instance with calibration settings and pin configuration overrides.
 * Inherits defaults from SensorType and can override them.
 */
export interface Sensor {
  [key: string]: unknown;
  id: string;
  tenantId: string;
  sensorTypeId: string;
  sensorTypeCode: string;
  sensorTypeName: string;
  name: string;
  description?: string;
  serialNumber?: string;

  // Interval Override (null = inherit from SensorType)
  intervalSecondsOverride?: number;

  // Pin Configuration Override (null = inherit from SensorType)
  i2cAddressOverride?: string;
  sdaPinOverride?: number;
  sclPinOverride?: number;
  oneWirePinOverride?: number;
  analogPinOverride?: number;
  digitalPinOverride?: number;
  triggerPinOverride?: number;
  echoPinOverride?: number;

  // Calibration
  offsetCorrection: number;
  gainCorrection: number;
  lastCalibratedAt?: string;
  calibrationNotes?: string;
  calibrationDueAt?: string;

  // Capabilities
  activeCapabilityIds: string[];

  // Status
  isActive: boolean;
  createdAt: string;
  updatedAt: string;

  // Optional: populated from SensorType lookup
  sensorType?: SensorType;
}

export interface CreateSensorDto {
  sensorTypeId: string;
  name: string;
  description?: string;
  serialNumber?: string;

  // Interval Override
  intervalSecondsOverride?: number;

  // Pin Configuration Override
  i2cAddressOverride?: string;
  sdaPinOverride?: number;
  sclPinOverride?: number;
  oneWirePinOverride?: number;
  analogPinOverride?: number;
  digitalPinOverride?: number;
  triggerPinOverride?: number;
  echoPinOverride?: number;

  // Initial Calibration
  offsetCorrection?: number;
  gainCorrection?: number;

  // Capabilities
  activeCapabilityIds?: string[];
}

export interface UpdateSensorDto {
  name?: string;
  description?: string;
  serialNumber?: string;

  // Interval Override
  intervalSecondsOverride?: number;

  // Pin Configuration Override
  i2cAddressOverride?: string;
  sdaPinOverride?: number;
  sclPinOverride?: number;
  oneWirePinOverride?: number;
  analogPinOverride?: number;
  digitalPinOverride?: number;
  triggerPinOverride?: number;
  echoPinOverride?: number;

  // Calibration
  offsetCorrection?: number;
  gainCorrection?: number;
  calibrationNotes?: string;

  // Capabilities
  activeCapabilityIds?: string[];

  // Status
  isActive?: boolean;
}

export interface CalibrateSensorDto {
  offsetCorrection: number;
  gainCorrection?: number;
  calibrationNotes?: string;
  calibrationDueAt?: string;
}
