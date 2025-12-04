using FluentAssertions;
using Microsoft.AspNetCore.SignalR;
using Microsoft.Extensions.Logging;
using Moq;
using myIoTGrid.Hub.Interface.Hubs;
using myIoTGrid.Hub.Interface.Services;
using myIoTGrid.Hub.Shared.DTOs;
using myIoTGrid.Hub.Shared.Enums;

namespace myIoTGrid.Hub.Interface.Tests.Services;

/// <summary>
/// Tests for SignalRNotificationService.
/// </summary>
public class SignalRNotificationServiceTests
{
    private readonly Mock<IHubContext<SensorHub>> _hubContextMock;
    private readonly Mock<IHubClients> _clientsMock;
    private readonly Mock<IClientProxy> _clientProxyMock;
    private readonly Mock<ILogger<SignalRNotificationService>> _loggerMock;
    private readonly SignalRNotificationService _sut;

    public SignalRNotificationServiceTests()
    {
        _hubContextMock = new Mock<IHubContext<SensorHub>>();
        _clientsMock = new Mock<IHubClients>();
        _clientProxyMock = new Mock<IClientProxy>();
        _loggerMock = new Mock<ILogger<SignalRNotificationService>>();

        _hubContextMock.Setup(h => h.Clients).Returns(_clientsMock.Object);
        _clientsMock.Setup(c => c.Group(It.IsAny<string>())).Returns(_clientProxyMock.Object);

        _sut = new SignalRNotificationService(_hubContextMock.Object, _loggerMock.Object);
    }

    #region NotifyNewReadingAsync Tests

    [Fact]
    public async Task NotifyNewReadingAsync_SendsToTenantGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var nodeId = Guid.NewGuid();
        var reading = CreateReadingDto(tenantId, nodeId);

        // Act
        await _sut.NotifyNewReadingAsync(reading);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetTenantGroupName(tenantId)), Times.Once);
        _clientProxyMock.Verify(p => p.SendCoreAsync(
            "NewReading",
            It.Is<object[]>(args => args.Length == 1 && args[0] == reading),
            It.IsAny<CancellationToken>()), Times.AtLeastOnce);
    }

    [Fact]
    public async Task NotifyNewReadingAsync_SendsToNodeGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var nodeId = Guid.NewGuid();
        var reading = CreateReadingDto(tenantId, nodeId);

        // Act
        await _sut.NotifyNewReadingAsync(reading);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetNodeGroupName(nodeId)), Times.Once);
    }

    [Fact]
    public async Task NotifyNewReadingAsync_WithCancellationToken_PassesToken()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var nodeId = Guid.NewGuid();
        var reading = CreateReadingDto(tenantId, nodeId);
        var cts = new CancellationTokenSource();

        // Act
        await _sut.NotifyNewReadingAsync(reading, cts.Token);

        // Assert
        _clientProxyMock.Verify(p => p.SendCoreAsync(
            "NewReading",
            It.IsAny<object[]>(),
            cts.Token), Times.AtLeastOnce);
    }

    #endregion

    #region NotifyAlertReceivedAsync Tests

    [Fact]
    public async Task NotifyAlertReceivedAsync_SendsToTenantGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var alert = CreateAlertDto(tenantId);

        // Act
        await _sut.NotifyAlertReceivedAsync(tenantId, alert);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetTenantGroupName(tenantId)), Times.Once);
        _clientProxyMock.Verify(p => p.SendCoreAsync(
            "AlertReceived",
            It.Is<object[]>(args => args.Length == 1 && args[0] == alert),
            It.IsAny<CancellationToken>()), Times.AtLeastOnce);
    }

    [Fact]
    public async Task NotifyAlertReceivedAsync_SendsToAlertLevelGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var alert = CreateAlertDto(tenantId, AlertLevelDto.Critical);

        // Act
        await _sut.NotifyAlertReceivedAsync(tenantId, alert);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetAlertGroupName((int)AlertLevelDto.Critical)), Times.Once);
    }

    #endregion

    #region NotifyAlertAcknowledgedAsync Tests

    [Fact]
    public async Task NotifyAlertAcknowledgedAsync_SendsToTenantGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var alert = CreateAlertDto(tenantId);

        // Act
        await _sut.NotifyAlertAcknowledgedAsync(tenantId, alert);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetTenantGroupName(tenantId)), Times.Once);
        _clientProxyMock.Verify(p => p.SendCoreAsync(
            "AlertAcknowledged",
            It.Is<object[]>(args => args.Length == 1 && args[0] == alert),
            It.IsAny<CancellationToken>()), Times.Once);
    }

    #endregion

    #region NotifyHubStatusChangedAsync Tests

    [Fact]
    public async Task NotifyHubStatusChangedAsync_SendsToTenantGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var hub = CreateHubDto(tenantId);

        // Act
        await _sut.NotifyHubStatusChangedAsync(tenantId, hub);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetTenantGroupName(tenantId)), Times.Once);
        _clientProxyMock.Verify(p => p.SendCoreAsync(
            "HubStatusChanged",
            It.Is<object[]>(args => args.Length == 1 && args[0] == hub),
            It.IsAny<CancellationToken>()), Times.AtLeastOnce);
    }

    [Fact]
    public async Task NotifyHubStatusChangedAsync_SendsToHubGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var hub = CreateHubDto(tenantId);

        // Act
        await _sut.NotifyHubStatusChangedAsync(tenantId, hub);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetHubGroupName(hub.HubId)), Times.Once);
    }

    #endregion

    #region NotifyNodeStatusChangedAsync Tests

    [Fact]
    public async Task NotifyNodeStatusChangedAsync_SendsToTenantGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var node = CreateNodeDto(tenantId);

        // Act
        await _sut.NotifyNodeStatusChangedAsync(tenantId, node);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetTenantGroupName(tenantId)), Times.Once);
        _clientProxyMock.Verify(p => p.SendCoreAsync(
            "NodeStatusChanged",
            It.Is<object[]>(args => args.Length == 1 && args[0] == node),
            It.IsAny<CancellationToken>()), Times.AtLeastOnce);
    }

    [Fact]
    public async Task NotifyNodeStatusChangedAsync_SendsToNodeGroup()
    {
        // Arrange
        var tenantId = Guid.NewGuid();
        var node = CreateNodeDto(tenantId);

        // Act
        await _sut.NotifyNodeStatusChangedAsync(tenantId, node);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetNodeGroupName(node.Id)), Times.Once);
    }

    #endregion

    #region NotifyNodeRegisteredAsync Tests

    [Fact]
    public async Task NotifyNodeRegisteredAsync_SendsToHubGroup()
    {
        // Arrange
        var hubId = Guid.NewGuid();
        var node = CreateNodeDto(Guid.NewGuid());

        // Act
        await _sut.NotifyNodeRegisteredAsync(hubId, node);

        // Assert
        _clientsMock.Verify(c => c.Group(SensorHub.GetHubGroupName(hubId.ToString())), Times.Once);
        _clientProxyMock.Verify(p => p.SendCoreAsync(
            "NodeRegistered",
            It.Is<object[]>(args => args.Length == 1 && args[0] == node),
            It.IsAny<CancellationToken>()), Times.Once);
    }

    #endregion

    #region Helper Methods

    private static ReadingDto CreateReadingDto(Guid tenantId, Guid nodeId)
    {
        return new ReadingDto(
            Id: 1,
            TenantId: tenantId,
            NodeId: nodeId,
            NodeName: "Test Node",
            AssignmentId: Guid.NewGuid(),
            SensorId: Guid.NewGuid(),
            SensorCode: "BME280",
            SensorName: "BME280 Sensor",
            SensorIcon: "thermostat",
            SensorColor: "#FF5722",
            MeasurementType: "temperature",
            DisplayName: "Temperature",
            RawValue: 21.3,
            Value: 21.5,
            Unit: "°C",
            Timestamp: DateTime.UtcNow,
            Location: null,
            IsSyncedToCloud: false
        );
    }

    private static AlertDto CreateAlertDto(Guid tenantId, AlertLevelDto level = AlertLevelDto.Warning)
    {
        return new AlertDto(
            Id: Guid.NewGuid(),
            TenantId: tenantId,
            HubId: Guid.NewGuid(),
            HubName: "Test Hub",
            NodeId: Guid.NewGuid(),
            NodeName: "Test Node",
            AlertTypeId: Guid.NewGuid(),
            AlertTypeCode: "mold_risk",
            AlertTypeName: "Mold Risk",
            Level: level,
            Message: "Test alert message",
            Recommendation: "Test recommendation",
            Source: AlertSourceDto.Local,
            CreatedAt: DateTime.UtcNow,
            ExpiresAt: null,
            AcknowledgedAt: null,
            IsActive: true
        );
    }

    private static HubDto CreateHubDto(Guid tenantId)
    {
        return new HubDto(
            Id: Guid.NewGuid(),
            TenantId: tenantId,
            HubId: "hub-test-01",
            Name: "Test Hub",
            Description: "Test hub description",
            LastSeen: DateTime.UtcNow,
            IsOnline: true,
            CreatedAt: DateTime.UtcNow,
            SensorCount: 5
        );
    }

    private static NodeDto CreateNodeDto(Guid tenantId)
    {
        return new NodeDto(
            Id: Guid.NewGuid(),
            HubId: Guid.NewGuid(),
            NodeId: "node-test-01",
            Name: "Test Node",
            Protocol: ProtocolDto.WLAN,
            Location: null,
            AssignmentCount: 3,
            LastSeen: DateTime.UtcNow,
            IsOnline: true,
            FirmwareVersion: "1.0.0",
            BatteryLevel: 85,
            CreatedAt: DateTime.UtcNow,
            MacAddress: "AA:BB:CC:DD:EE:FF",
            Status: NodeProvisioningStatusDto.Configured,
            IsSimulation: false
        );
    }

    #endregion
}
