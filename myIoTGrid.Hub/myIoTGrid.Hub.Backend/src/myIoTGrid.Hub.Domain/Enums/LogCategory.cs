namespace myIoTGrid.Hub.Domain.Enums;

/// <summary>
/// Log category for categorizing debug logs (Sprint 8: Remote Debug System).
/// Used for filtering logs by subsystem.
/// </summary>
public enum LogCategory
{
    /// <summary>System/boot/state machine logs</summary>
    System = 0,

    /// <summary>Hardware/I2C/UART/GPIO logs</summary>
    Hardware = 1,

    /// <summary>WiFi/BLE/connectivity logs</summary>
    Network = 2,

    /// <summary>Sensor reading/measurement logs</summary>
    Sensor = 3,

    /// <summary>GPS/GNSS specific logs</summary>
    GPS = 4,

    /// <summary>HTTP API/Hub communication logs</summary>
    API = 5,

    /// <summary>SD card/NVS/storage logs</summary>
    Storage = 6,

    /// <summary>Error conditions (always logged)</summary>
    Error = 7
}
