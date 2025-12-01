namespace myIoTGrid.Hub.Shared.DTOs;

/// <summary>
/// DTO for Hub status information
/// </summary>
/// <param name="IsOnline">Current online status</param>
/// <param name="LastSeen">Last heartbeat timestamp</param>
/// <param name="NodeCount">Number of connected nodes</param>
/// <param name="OnlineNodeCount">Number of online nodes</param>
public record HubStatusDto(
    bool IsOnline,
    DateTime? LastSeen,
    int NodeCount,
    int OnlineNodeCount
);
