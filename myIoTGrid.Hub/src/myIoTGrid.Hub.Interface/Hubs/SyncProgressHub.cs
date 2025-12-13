using Microsoft.AspNetCore.SignalR;
using Microsoft.Extensions.Logging;

namespace myIoTGrid.Hub.Interface.Hubs;

/// <summary>
/// SignalR Hub for real-time sync progress updates.
/// Clients can subscribe to sync progress for specific nodes.
/// </summary>
public class SyncProgressHub : Microsoft.AspNetCore.SignalR.Hub
{
    private readonly ILogger<SyncProgressHub> _logger;

    public SyncProgressHub(ILogger<SyncProgressHub> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Subscribe to sync progress updates for a specific node.
    /// </summary>
    public async Task JoinSyncGroup(string nodeId)
    {
        await Groups.AddToGroupAsync(Context.ConnectionId, $"sync:{nodeId}");
        _logger.LogDebug("Client {ConnectionId} joined sync group for node {NodeId}",
            Context.ConnectionId, nodeId);
    }

    /// <summary>
    /// Unsubscribe from sync progress updates for a specific node.
    /// </summary>
    public async Task LeaveSyncGroup(string nodeId)
    {
        await Groups.RemoveFromGroupAsync(Context.ConnectionId, $"sync:{nodeId}");
        _logger.LogDebug("Client {ConnectionId} left sync group for node {NodeId}",
            Context.ConnectionId, nodeId);
    }

    /// <summary>
    /// Subscribe to all sync progress updates.
    /// </summary>
    public async Task JoinAllSyncGroup()
    {
        await Groups.AddToGroupAsync(Context.ConnectionId, "sync:all");
        _logger.LogDebug("Client {ConnectionId} joined all sync updates group",
            Context.ConnectionId);
    }

    /// <summary>
    /// Unsubscribe from all sync progress updates.
    /// </summary>
    public async Task LeaveAllSyncGroup()
    {
        await Groups.RemoveFromGroupAsync(Context.ConnectionId, "sync:all");
        _logger.LogDebug("Client {ConnectionId} left all sync updates group",
            Context.ConnectionId);
    }

    public override async Task OnConnectedAsync()
    {
        _logger.LogDebug("Client connected to SyncProgressHub: {ConnectionId}", Context.ConnectionId);
        await base.OnConnectedAsync();
    }

    public override async Task OnDisconnectedAsync(Exception? exception)
    {
        _logger.LogDebug("Client disconnected from SyncProgressHub: {ConnectionId}", Context.ConnectionId);
        await base.OnDisconnectedAsync(exception);
    }
}
