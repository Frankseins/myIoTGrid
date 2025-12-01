/**
 * NodeSensorAssignment model - corresponds to Backend NodeSensorAssignmentDto
 * Hardware binding of a Sensor to a Node with pin configuration and effective config.
 */
export interface NodeSensorAssignment {
  id: string;
  nodeId: string;
  nodeName: string;
  sensorId: string;
  sensorName: string;
  sensorTypeId: string;
  sensorTypeCode: string;
  sensorTypeName: string;
  endpointId: number;
  alias?: string;
  i2cAddressOverride?: string;
  sdaPinOverride?: number;
  sclPinOverride?: number;
  oneWirePinOverride?: number;
  analogPinOverride?: number;
  digitalPinOverride?: number;
  triggerPinOverride?: number;
  echoPinOverride?: number;
  intervalSecondsOverride?: number;
  isActive: boolean;
  lastSeenAt?: string;
  assignedAt: string;
  effectiveConfig: EffectiveConfig;
}

/**
 * DTO for creating a NodeSensorAssignment
 */
export interface CreateNodeSensorAssignmentDto {
  sensorId: string;
  endpointId: number;
  alias?: string;
  i2cAddressOverride?: string;
  sdaPinOverride?: number;
  sclPinOverride?: number;
  oneWirePinOverride?: number;
  analogPinOverride?: number;
  digitalPinOverride?: number;
  triggerPinOverride?: number;
  echoPinOverride?: number;
  intervalSecondsOverride?: number;
}

/**
 * DTO for updating a NodeSensorAssignment
 */
export interface UpdateNodeSensorAssignmentDto {
  alias?: string;
  i2cAddressOverride?: string;
  sdaPinOverride?: number;
  sclPinOverride?: number;
  oneWirePinOverride?: number;
  analogPinOverride?: number;
  digitalPinOverride?: number;
  triggerPinOverride?: number;
  echoPinOverride?: number;
  intervalSecondsOverride?: number;
  isActive?: boolean;
}

/**
 * Effective configuration after inheritance resolution.
 * EffectiveValue = Assignment ?? Sensor ?? SensorType
 */
export interface EffectiveConfig {
  intervalSeconds: number;
  i2cAddress?: string;
  sdaPin?: number;
  sclPin?: number;
  oneWirePin?: number;
  analogPin?: number;
  digitalPin?: number;
  triggerPin?: number;
  echoPin?: number;
  offsetCorrection: number;
  gainCorrection: number;
}
