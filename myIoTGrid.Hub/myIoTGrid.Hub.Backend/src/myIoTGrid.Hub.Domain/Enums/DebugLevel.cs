namespace myIoTGrid.Hub.Domain.Enums;

/// <summary>
/// Debug level for node logging (Sprint 8: Remote Debug System).
/// Controls verbosity of debug output and performance impact.
/// </summary>
public enum DebugLevel
{
    /// <summary>Minimal logging, errors only. Best performance.</summary>
    Production = 0,

    /// <summary>Standard logging for normal operation. Default level.</summary>
    Normal = 1,

    /// <summary>Verbose logging for troubleshooting. Higher battery/performance impact.</summary>
    Debug = 2
}
