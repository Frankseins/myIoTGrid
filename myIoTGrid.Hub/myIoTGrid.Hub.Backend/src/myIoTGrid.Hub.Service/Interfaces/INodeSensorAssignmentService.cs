using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Interfaces;

/// <summary>
/// Service Interface for NodeSensorAssignment management.
/// Hardware binding of Sensors to Nodes with pin configuration.
/// </summary>
public interface INodeSensorAssignmentService
{
    /// <summary>Returns all Assignments for a Node</summary>
    Task<IEnumerable<NodeSensorAssignmentDto>> GetByNodeAsync(Guid nodeId, CancellationToken ct = default);

    /// <summary>Returns all Assignments for a Sensor</summary>
    Task<IEnumerable<NodeSensorAssignmentDto>> GetBySensorAsync(Guid sensorId, CancellationToken ct = default);

    /// <summary>Returns an Assignment by Id</summary>
    Task<NodeSensorAssignmentDto?> GetByIdAsync(Guid id, CancellationToken ct = default);

    /// <summary>Returns an Assignment by NodeId and EndpointId</summary>
    Task<NodeSensorAssignmentDto?> GetByEndpointAsync(Guid nodeId, int endpointId, CancellationToken ct = default);

    /// <summary>Creates a new Assignment</summary>
    Task<NodeSensorAssignmentDto> CreateAsync(Guid nodeId, CreateNodeSensorAssignmentDto dto, CancellationToken ct = default);

    /// <summary>Updates an Assignment</summary>
    Task<NodeSensorAssignmentDto> UpdateAsync(Guid id, UpdateNodeSensorAssignmentDto dto, CancellationToken ct = default);

    /// <summary>Deletes an Assignment</summary>
    Task DeleteAsync(Guid id, CancellationToken ct = default);

    /// <summary>Updates LastSeenAt for an Assignment</summary>
    Task UpdateLastSeenAsync(Guid id, CancellationToken ct = default);
}
