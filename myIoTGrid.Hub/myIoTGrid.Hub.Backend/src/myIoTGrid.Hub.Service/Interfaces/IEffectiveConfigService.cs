using myIoTGrid.Hub.Domain.Entities;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Interfaces;

/// <summary>
/// Service Interface for calculating effective configuration values.
/// Implements inheritance: Assignment → Sensor → SensorType
/// </summary>
public interface IEffectiveConfigService
{
    /// <summary>
    /// Gets the effective configuration for an assignment.
    /// EffectiveValue = Assignment ?? Sensor ?? SensorType
    /// </summary>
    EffectiveConfigDto GetEffectiveConfig(
        NodeSensorAssignment assignment,
        Sensor sensor,
        SensorType sensorType);

    /// <summary>
    /// Applies calibration to a raw value.
    /// CalibratedValue = (RawValue * Gain) + Offset
    /// </summary>
    double ApplyCalibration(
        double rawValue,
        Sensor sensor,
        SensorType sensorType);

    /// <summary>
    /// Gets the effective offset correction.
    /// </summary>
    double GetEffectiveOffset(Sensor sensor, SensorType sensorType);

    /// <summary>
    /// Gets the effective gain correction.
    /// </summary>
    double GetEffectiveGain(Sensor sensor, SensorType sensorType);

    /// <summary>
    /// Gets the effective interval in seconds.
    /// </summary>
    int GetEffectiveInterval(
        NodeSensorAssignment? assignment,
        Sensor? sensor,
        SensorType sensorType);
}
