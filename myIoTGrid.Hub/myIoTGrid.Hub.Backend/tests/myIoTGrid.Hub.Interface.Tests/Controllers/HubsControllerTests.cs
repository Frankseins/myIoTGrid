using FluentAssertions;
using Microsoft.AspNetCore.Mvc;
using Moq;
using myIoTGrid.Hub.Interface.Controllers;
using myIoTGrid.Hub.Service.Interfaces;
using myIoTGrid.Hub.Shared.DTOs;
using myIoTGrid.Hub.Shared.Enums;

namespace myIoTGrid.Hub.Interface.Tests.Controllers;

/// <summary>
/// Tests for HubsController.
/// Hub = Raspberry Pi Gateway
/// </summary>
public class HubsControllerTests
{
    private readonly Mock<IHubService> _hubServiceMock;
    private readonly Mock<INodeService> _nodeServiceMock;
    private readonly HubsController _sut;

    private readonly Guid _tenantId = Guid.Parse("00000000-0000-0000-0000-000000000001");
    private readonly Guid _hubId = Guid.Parse("00000000-0000-0000-0000-000000000002");

    public HubsControllerTests()
    {
        _hubServiceMock = new Mock<IHubService>();
        _nodeServiceMock = new Mock<INodeService>();
        _sut = new HubsController(_hubServiceMock.Object, _nodeServiceMock.Object);
    }

    #region GetAll Tests

    [Fact]
    public async Task GetAll_ReturnsOkWithHubs()
    {
        // Arrange
        var hubs = new List<HubDto>
        {
            CreateHubDto("hub-01", "Hub 1"),
            CreateHubDto("hub-02", "Hub 2")
        };

        _hubServiceMock.Setup(s => s.GetAllAsync(It.IsAny<CancellationToken>()))
            .ReturnsAsync(hubs);

        // Act
        var result = await _sut.GetAll(CancellationToken.None);

        // Assert
        var okResult = result.Should().BeOfType<OkObjectResult>().Subject;
        var returnedHubs = okResult.Value.Should().BeAssignableTo<IEnumerable<HubDto>>().Subject;
        returnedHubs.Should().HaveCount(2);
    }

    [Fact]
    public async Task GetAll_WithNoHubs_ReturnsOkWithEmptyList()
    {
        // Arrange
        _hubServiceMock.Setup(s => s.GetAllAsync(It.IsAny<CancellationToken>()))
            .ReturnsAsync(new List<HubDto>());

        // Act
        var result = await _sut.GetAll(CancellationToken.None);

        // Assert
        var okResult = result.Should().BeOfType<OkObjectResult>().Subject;
        var returnedHubs = okResult.Value.Should().BeAssignableTo<IEnumerable<HubDto>>().Subject;
        returnedHubs.Should().BeEmpty();
    }

    #endregion

    #region GetById Tests

    [Fact]
    public async Task GetById_WithExistingHub_ReturnsOkWithHub()
    {
        // Arrange
        var hub = CreateHubDto("hub-01", "Test Hub");

        _hubServiceMock.Setup(s => s.GetByIdAsync(_hubId, It.IsAny<CancellationToken>()))
            .ReturnsAsync(hub);

        // Act
        var result = await _sut.GetById(_hubId, CancellationToken.None);

        // Assert
        var okResult = result.Should().BeOfType<OkObjectResult>().Subject;
        var returnedHub = okResult.Value.Should().BeOfType<HubDto>().Subject;
        returnedHub.HubId.Should().Be("hub-01");
    }

    [Fact]
    public async Task GetById_WithNonExistingHub_ReturnsNotFound()
    {
        // Arrange
        _hubServiceMock.Setup(s => s.GetByIdAsync(_hubId, It.IsAny<CancellationToken>()))
            .ReturnsAsync((HubDto?)null);

        // Act
        var result = await _sut.GetById(_hubId, CancellationToken.None);

        // Assert
        result.Should().BeOfType<NotFoundResult>();
    }

    #endregion

    #region GetNodes Tests

    [Fact]
    public async Task GetNodes_ReturnsOkWithNodes()
    {
        // Arrange
        var nodes = new List<NodeDto>
        {
            CreateNodeDto("node-01", "Node 1"),
            CreateNodeDto("node-02", "Node 2")
        };

        _nodeServiceMock.Setup(s => s.GetByHubAsync(_hubId, It.IsAny<CancellationToken>()))
            .ReturnsAsync(nodes);

        // Act
        var result = await _sut.GetNodes(_hubId, CancellationToken.None);

        // Assert
        var okResult = result.Should().BeOfType<OkObjectResult>().Subject;
        var returnedNodes = okResult.Value.Should().BeAssignableTo<IEnumerable<NodeDto>>().Subject;
        returnedNodes.Should().HaveCount(2);
    }

    [Fact]
    public async Task GetNodes_WithNoNodes_ReturnsOkWithEmptyList()
    {
        // Arrange
        _nodeServiceMock.Setup(s => s.GetByHubAsync(_hubId, It.IsAny<CancellationToken>()))
            .ReturnsAsync(new List<NodeDto>());

        // Act
        var result = await _sut.GetNodes(_hubId, CancellationToken.None);

        // Assert
        var okResult = result.Should().BeOfType<OkObjectResult>().Subject;
        var returnedNodes = okResult.Value.Should().BeAssignableTo<IEnumerable<NodeDto>>().Subject;
        returnedNodes.Should().BeEmpty();
    }

    #endregion

    #region Create Tests

    [Fact]
    public async Task Create_WithValidData_ReturnsCreatedAtAction()
    {
        // Arrange
        var dto = new CreateHubDto(
            HubId: "hub-01",
            Name: "Test Hub",
            Description: "A test hub"
        );
        var hub = CreateHubDto("hub-01", "Test Hub");

        _hubServiceMock.Setup(s => s.CreateAsync(dto, It.IsAny<CancellationToken>()))
            .ReturnsAsync(hub);

        // Act
        var result = await _sut.Create(dto, CancellationToken.None);

        // Assert
        var createdResult = result.Should().BeOfType<CreatedAtActionResult>().Subject;
        createdResult.ActionName.Should().Be(nameof(HubsController.GetById));
        createdResult.Value.Should().BeOfType<HubDto>();
    }

    [Fact]
    public async Task Create_WithMinimalData_ReturnsCreatedAtAction()
    {
        // Arrange
        var dto = new CreateHubDto(
            HubId: "hub-minimal",
            Name: "Minimal Hub"
        );
        var hub = CreateHubDto("hub-minimal", "Minimal Hub");

        _hubServiceMock.Setup(s => s.CreateAsync(dto, It.IsAny<CancellationToken>()))
            .ReturnsAsync(hub);

        // Act
        var result = await _sut.Create(dto, CancellationToken.None);

        // Assert
        result.Should().BeOfType<CreatedAtActionResult>();
    }

    #endregion

    #region Update Tests

    [Fact]
    public async Task Update_WithExistingHub_ReturnsOkWithHub()
    {
        // Arrange
        var dto = new UpdateHubDto(Name: "Updated Hub", Description: "Updated description");
        var hub = CreateHubDto("hub-01", "Updated Hub");

        _hubServiceMock.Setup(s => s.UpdateAsync(_hubId, dto, It.IsAny<CancellationToken>()))
            .ReturnsAsync(hub);

        // Act
        var result = await _sut.Update(_hubId, dto, CancellationToken.None);

        // Assert
        var okResult = result.Should().BeOfType<OkObjectResult>().Subject;
        var returnedHub = okResult.Value.Should().BeOfType<HubDto>().Subject;
        returnedHub.Name.Should().Be("Updated Hub");
    }

    [Fact]
    public async Task Update_WithNonExistingHub_ReturnsNotFound()
    {
        // Arrange
        var dto = new UpdateHubDto(Name: "Updated Hub", Description: null);

        _hubServiceMock.Setup(s => s.UpdateAsync(_hubId, dto, It.IsAny<CancellationToken>()))
            .ReturnsAsync((HubDto?)null);

        // Act
        var result = await _sut.Update(_hubId, dto, CancellationToken.None);

        // Assert
        result.Should().BeOfType<NotFoundResult>();
    }

    [Fact]
    public async Task Update_WithPartialData_ReturnsOk()
    {
        // Arrange
        var dto = new UpdateHubDto(Name: "Only Name Updated", Description: null);
        var hub = CreateHubDto("hub-01", "Only Name Updated");

        _hubServiceMock.Setup(s => s.UpdateAsync(_hubId, dto, It.IsAny<CancellationToken>()))
            .ReturnsAsync(hub);

        // Act
        var result = await _sut.Update(_hubId, dto, CancellationToken.None);

        // Assert
        result.Should().BeOfType<OkObjectResult>();
    }

    #endregion

    #region Helper Methods

    private HubDto CreateHubDto(string hubId, string name)
    {
        return new HubDto(
            Id: _hubId,
            TenantId: _tenantId,
            HubId: hubId,
            Name: name,
            Description: null,
            LastSeen: DateTime.UtcNow,
            IsOnline: true,
            CreatedAt: DateTime.UtcNow,
            SensorCount: 0
        );
    }

    private NodeDto CreateNodeDto(string nodeId, string name)
    {
        return new NodeDto(
            Id: Guid.NewGuid(),
            HubId: _hubId,
            NodeId: nodeId,
            Name: name,
            Protocol: ProtocolDto.WLAN,
            Location: null,
            AssignmentCount: 2,
            LastSeen: DateTime.UtcNow,
            IsOnline: true,
            FirmwareVersion: "1.0.0",
            BatteryLevel: 100,
            CreatedAt: DateTime.UtcNow
        );
    }

    #endregion
}
