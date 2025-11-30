using myIoTGrid.Hub.Domain.Entities;
using myIoTGrid.Hub.Service.Interfaces;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Services;

/// <summary>
/// Service for calculating effective configuration values.
/// Implements inheritance: Assignment → Sensor → SensorType
/// </summary>
public class EffectiveConfigService : IEffectiveConfigService
{
    /// <inheritdoc />
    public EffectiveConfigDto GetEffectiveConfig(
        NodeSensorAssignment assignment,
        Sensor sensor,
        SensorType sensorType)
    {
        return new EffectiveConfigDto(
            IntervalSeconds: GetEffectiveInterval(assignment, sensor, sensorType),
            I2CAddress: assignment.I2CAddressOverride ?? sensorType.DefaultI2CAddress,
            SdaPin: assignment.SdaPinOverride ?? sensorType.DefaultSdaPin,
            SclPin: assignment.SclPinOverride ?? sensorType.DefaultSclPin,
            OneWirePin: assignment.OneWirePinOverride ?? sensorType.DefaultOneWirePin,
            AnalogPin: assignment.AnalogPinOverride ?? sensorType.DefaultAnalogPin,
            DigitalPin: assignment.DigitalPinOverride ?? sensorType.DefaultDigitalPin,
            TriggerPin: assignment.TriggerPinOverride ?? sensorType.DefaultTriggerPin,
            EchoPin: assignment.EchoPinOverride ?? sensorType.DefaultEchoPin,
            OffsetCorrection: GetEffectiveOffset(sensor, sensorType),
            GainCorrection: GetEffectiveGain(sensor, sensorType)
        );
    }

    /// <inheritdoc />
    public double ApplyCalibration(
        double rawValue,
        Sensor sensor,
        SensorType sensorType)
    {
        var offset = GetEffectiveOffset(sensor, sensorType);
        var gain = GetEffectiveGain(sensor, sensorType);

        // Calibrated = (Raw * Gain) + Offset
        return (rawValue * gain) + offset;
    }

    /// <inheritdoc />
    public double GetEffectiveOffset(Sensor sensor, SensorType sensorType)
    {
        // Use sensor offset if set (non-zero), otherwise use SensorType default
        return Math.Abs(sensor.OffsetCorrection) > 0.0001
            ? sensor.OffsetCorrection
            : sensorType.DefaultOffsetCorrection;
    }

    /// <inheritdoc />
    public double GetEffectiveGain(Sensor sensor, SensorType sensorType)
    {
        // Use sensor gain if different from 1.0, otherwise use SensorType default
        return Math.Abs(sensor.GainCorrection - 1.0) > 0.0001
            ? sensor.GainCorrection
            : sensorType.DefaultGainCorrection;
    }

    /// <inheritdoc />
    public int GetEffectiveInterval(
        NodeSensorAssignment? assignment,
        Sensor? sensor,
        SensorType sensorType)
    {
        // Priority: Assignment → Sensor → SensorType
        if (assignment?.IntervalSecondsOverride.HasValue == true)
            return assignment.IntervalSecondsOverride.Value;

        if (sensor?.IntervalSecondsOverride.HasValue == true)
            return sensor.IntervalSecondsOverride.Value;

        return sensorType.DefaultIntervalSeconds;
    }
}
