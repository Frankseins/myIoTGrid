using myIoTGrid.Hub.Shared.Enums;

namespace myIoTGrid.Hub.Shared.DTOs;

/// <summary>
/// DTO for Node (ESP32/LoRa32 device) information.
/// Matter-konform: Entspricht einem Matter Node.
/// </summary>
public record NodeDto(
    Guid Id,
    Guid HubId,
    string NodeId,
    string Name,
    ProtocolDto Protocol,
    LocationDto? Location,
    int AssignmentCount,
    DateTime? LastSeen,
    bool IsOnline,
    string? FirmwareVersion,
    int? BatteryLevel,
    DateTime CreatedAt
);

/// <summary>
/// DTO for creating a Node
/// </summary>
public record CreateNodeDto(
    string NodeId,
    string? Name = null,
    string? HubIdentifier = null,
    Guid? HubId = null,
    ProtocolDto Protocol = ProtocolDto.WLAN,
    LocationDto? Location = null
);

/// <summary>
/// DTO for updating a Node
/// </summary>
public record UpdateNodeDto(
    string? Name = null,
    LocationDto? Location = null,
    string? FirmwareVersion = null
);

/// <summary>
/// DTO for Node status updates
/// </summary>
public record NodeStatusDto(
    Guid NodeId,
    bool IsOnline,
    DateTime? LastSeen,
    int? BatteryLevel
);
