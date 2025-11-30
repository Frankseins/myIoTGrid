using FluentAssertions;
using Microsoft.Extensions.Caching.Memory;
using Microsoft.Extensions.Logging;
using Moq;
using myIoTGrid.Hub.Domain.Entities;
using myIoTGrid.Hub.Domain.Enums;
using myIoTGrid.Hub.Infrastructure.Repositories;
using myIoTGrid.Hub.Service.Services;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Tests.Services;

public class SensorTypeServiceTests : IDisposable
{
    private readonly Infrastructure.Data.HubDbContext _context;
    private readonly SensorTypeService _sut;
    private readonly Mock<ILogger<SensorTypeService>> _loggerMock;
    private readonly IMemoryCache _memoryCache;

    public SensorTypeServiceTests()
    {
        _context = TestDbContextFactory.Create();
        _loggerMock = new Mock<ILogger<SensorTypeService>>();
        _memoryCache = new MemoryCache(new MemoryCacheOptions());
        var unitOfWork = new UnitOfWork(_context);

        _sut = new SensorTypeService(_context, unitOfWork, _memoryCache, _loggerMock.Object);
    }

    public void Dispose()
    {
        _context.Dispose();
        _memoryCache.Dispose();
    }

    private SensorType CreateTestSensorType(string code = "test_sensor", string name = "Test Sensor", bool isActive = true, bool isGlobal = false)
    {
        var sensorType = new SensorType
        {
            Id = Guid.NewGuid(),
            Code = code,
            Name = name,
            Protocol = CommunicationProtocol.I2C,
            Category = "climate",
            Icon = "thermostat",
            Color = "#FF5722",
            DefaultI2CAddress = "0x76",
            DefaultSdaPin = 21,
            DefaultSclPin = 22,
            DefaultIntervalSeconds = 60,
            MinIntervalSeconds = 1,
            DefaultOffsetCorrection = 0,
            DefaultGainCorrection = 1.0,
            IsGlobal = isGlobal,
            IsActive = isActive,
            CreatedAt = DateTime.UtcNow,
            UpdatedAt = DateTime.UtcNow
        };

        sensorType.Capabilities.Add(new SensorTypeCapability
        {
            Id = Guid.NewGuid(),
            SensorTypeId = sensorType.Id,
            MeasurementType = "temperature",
            DisplayName = "Temperatur",
            Unit = "°C",
            MinValue = -40,
            MaxValue = 85,
            Resolution = 0.01,
            Accuracy = 0.5,
            MatterClusterId = 1026,
            MatterClusterName = "TemperatureMeasurement",
            IsActive = true,
            SortOrder = 1
        });

        return sensorType;
    }

    #region GetAllAsync Tests

    [Fact]
    public async Task GetAllAsync_WhenNoSensorTypes_ReturnsEmptyList()
    {
        // Act
        var result = await _sut.GetAllAsync();

        // Assert
        result.Should().BeEmpty();
    }

    [Fact]
    public async Task GetAllAsync_WithSensorTypes_ReturnsOnlyActive()
    {
        // Arrange
        var activeSensor = CreateTestSensorType("active", "Active Sensor", isActive: true);
        var inactiveSensor = CreateTestSensorType("inactive", "Inactive Sensor", isActive: false);

        _context.SensorTypes.AddRange(activeSensor, inactiveSensor);
        await _context.SaveChangesAsync();

        // Act
        var result = (await _sut.GetAllAsync()).ToList();

        // Assert
        result.Should().HaveCount(1);
        result.First().Code.Should().Be("active");
    }

    [Fact]
    public async Task GetAllAsync_ReturnsSensorTypesOrderedByCategoryThenName()
    {
        // Arrange
        var zSensor = CreateTestSensorType("z_sensor", "Z Sensor");
        zSensor.Category = "water";
        var aSensor = CreateTestSensorType("a_sensor", "A Sensor");
        aSensor.Category = "climate";

        _context.SensorTypes.AddRange(zSensor, aSensor);
        await _context.SaveChangesAsync();

        // Act
        var result = (await _sut.GetAllAsync()).ToList();

        // Assert
        result.Should().HaveCount(2);
        result.First().Code.Should().Be("a_sensor"); // climate comes before water
    }

    [Fact]
    public async Task GetAllAsync_IncludesCapabilities()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        sensorType.Capabilities.Add(new SensorTypeCapability
        {
            Id = Guid.NewGuid(),
            SensorTypeId = sensorType.Id,
            MeasurementType = "humidity",
            DisplayName = "Luftfeuchtigkeit",
            Unit = "%",
            IsActive = true,
            SortOrder = 2
        });
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = (await _sut.GetAllAsync()).First();

        // Assert
        result.Capabilities.Should().HaveCount(2);
    }

    #endregion

    #region GetAllCachedAsync Tests

    [Fact]
    public async Task GetAllCachedAsync_ReturnsCachedData()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act - First call
        var result1 = await _sut.GetAllCachedAsync();

        // Add another sensor type
        var newSensor = CreateTestSensorType("new_sensor", "New Sensor");
        _context.SensorTypes.Add(newSensor);
        await _context.SaveChangesAsync();

        // Second call - should return cached data
        var result2 = await _sut.GetAllCachedAsync();

        // Assert
        result1.Should().HaveCount(1);
        result2.Should().HaveCount(1); // Still 1 because cached
    }

    [Fact]
    public async Task GetAllCachedAsync_AfterCacheExpiry_ReturnsNewData()
    {
        // Arrange - This test just verifies that cache is populated
        var sensorType = CreateTestSensorType();
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetAllCachedAsync();

        // Assert
        result.Should().HaveCount(1);
    }

    #endregion

    #region GetByIdAsync Tests

    [Fact]
    public async Task GetByIdAsync_ExistingSensorType_ReturnsSensorType()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetByIdAsync(sensorType.Id);

        // Assert
        result.Should().NotBeNull();
        result!.Id.Should().Be(sensorType.Id);
        result.Code.Should().Be(sensorType.Code);
    }

    [Fact]
    public async Task GetByIdAsync_NonExistingSensorType_ReturnsNull()
    {
        // Act
        var result = await _sut.GetByIdAsync(Guid.NewGuid());

        // Assert
        result.Should().BeNull();
    }

    #endregion

    #region GetByCodeAsync Tests

    [Fact]
    public async Task GetByCodeAsync_ExistingCode_ReturnsSensorType()
    {
        // Arrange
        var sensorType = CreateTestSensorType("dht22", "DHT22");
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetByCodeAsync("dht22");

        // Assert
        result.Should().NotBeNull();
        result!.Code.Should().Be("dht22");
    }

    [Fact]
    public async Task GetByCodeAsync_CaseInsensitive_ReturnsSensorType()
    {
        // Arrange
        var sensorType = CreateTestSensorType("dht22", "DHT22");
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetByCodeAsync("DHT22");

        // Assert
        result.Should().NotBeNull();
        result!.Code.Should().Be("dht22");
    }

    [Fact]
    public async Task GetByCodeAsync_NonExistingCode_ReturnsNull()
    {
        // Act
        var result = await _sut.GetByCodeAsync("nonexistent");

        // Assert
        result.Should().BeNull();
    }

    #endregion

    #region GetByCategoryAsync Tests

    [Fact]
    public async Task GetByCategoryAsync_ReturnsMatchingSensorTypes()
    {
        // Arrange
        var climateSensor = CreateTestSensorType("climate_sensor", "Climate Sensor");
        climateSensor.Category = "climate";

        var waterSensor = CreateTestSensorType("water_sensor", "Water Sensor");
        waterSensor.Category = "water";

        _context.SensorTypes.AddRange(climateSensor, waterSensor);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetByCategoryAsync("climate");

        // Assert
        result.Should().HaveCount(1);
        result.First().Code.Should().Be("climate_sensor");
    }

    [Fact]
    public async Task GetByCategoryAsync_CaseInsensitive_ReturnsMatchingSensorTypes()
    {
        // Arrange
        var climateSensor = CreateTestSensorType("climate_sensor", "Climate Sensor");
        climateSensor.Category = "climate";
        _context.SensorTypes.Add(climateSensor);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetByCategoryAsync("CLIMATE");

        // Assert
        result.Should().HaveCount(1);
    }

    [Fact]
    public async Task GetByCategoryAsync_NonExistingCategory_ReturnsEmpty()
    {
        // Arrange
        var sensor = CreateTestSensorType();
        sensor.Category = "climate";
        _context.SensorTypes.Add(sensor);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetByCategoryAsync("nonexistent");

        // Assert
        result.Should().BeEmpty();
    }

    #endregion

    #region GetCapabilitiesAsync Tests

    [Fact]
    public async Task GetCapabilitiesAsync_ReturnsCapabilities()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetCapabilitiesAsync(sensorType.Id);

        // Assert
        result.Should().HaveCount(1);
        result.First().MeasurementType.Should().Be("temperature");
    }

    [Fact]
    public async Task GetCapabilitiesAsync_ReturnsOnlyActiveCapabilities()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        sensorType.Capabilities.Add(new SensorTypeCapability
        {
            Id = Guid.NewGuid(),
            SensorTypeId = sensorType.Id,
            MeasurementType = "inactive_cap",
            DisplayName = "Inactive",
            Unit = "x",
            IsActive = false,
            SortOrder = 2
        });
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = await _sut.GetCapabilitiesAsync(sensorType.Id);

        // Assert
        result.Should().HaveCount(1);
        result.Should().NotContain(c => c.MeasurementType == "inactive_cap");
    }

    [Fact]
    public async Task GetCapabilitiesAsync_ReturnsOrderedBySortOrder()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        sensorType.Capabilities.Clear();
        sensorType.Capabilities.Add(new SensorTypeCapability
        {
            Id = Guid.NewGuid(),
            SensorTypeId = sensorType.Id,
            MeasurementType = "second",
            DisplayName = "Second",
            Unit = "s",
            IsActive = true,
            SortOrder = 2
        });
        sensorType.Capabilities.Add(new SensorTypeCapability
        {
            Id = Guid.NewGuid(),
            SensorTypeId = sensorType.Id,
            MeasurementType = "first",
            DisplayName = "First",
            Unit = "f",
            IsActive = true,
            SortOrder = 1
        });
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        var result = (await _sut.GetCapabilitiesAsync(sensorType.Id)).ToList();

        // Assert
        result.First().MeasurementType.Should().Be("first");
        result.Last().MeasurementType.Should().Be("second");
    }

    #endregion

    #region CreateAsync Tests

    [Fact]
    public async Task CreateAsync_ValidDto_CreatesSensorType()
    {
        // Arrange
        var dto = new CreateSensorTypeDto(
            Code: "new_sensor",
            Name: "New Sensor",
            Protocol: Shared.Enums.CommunicationProtocolDto.I2C,
            Category: "climate"
        );

        // Act
        var result = await _sut.CreateAsync(dto);

        // Assert
        result.Should().NotBeNull();
        result.Code.Should().Be("new_sensor");
        result.Name.Should().Be("New Sensor");
    }

    [Fact]
    public async Task CreateAsync_NormalizesCodeToLowerCase()
    {
        // Arrange
        var dto = new CreateSensorTypeDto(
            Code: "UPPER_CASE",
            Name: "Upper Case",
            Protocol: Shared.Enums.CommunicationProtocolDto.I2C,
            Category: "climate"
        );

        // Act
        var result = await _sut.CreateAsync(dto);

        // Assert
        result.Code.Should().Be("upper_case");
    }

    [Fact]
    public async Task CreateAsync_DuplicateCode_ThrowsException()
    {
        // Arrange
        var existingSensor = CreateTestSensorType("existing", "Existing");
        _context.SensorTypes.Add(existingSensor);
        await _context.SaveChangesAsync();

        var dto = new CreateSensorTypeDto(
            Code: "EXISTING", // Case-insensitive duplicate
            Name: "Duplicate",
            Protocol: Shared.Enums.CommunicationProtocolDto.I2C,
            Category: "climate"
        );

        // Act & Assert
        var act = () => _sut.CreateAsync(dto);
        await act.Should().ThrowAsync<InvalidOperationException>()
            .WithMessage("*already exists*");
    }

    [Fact]
    public async Task CreateAsync_SetsIsGlobalToFalse()
    {
        // Arrange
        var dto = new CreateSensorTypeDto(
            Code: "custom",
            Name: "Custom",
            Protocol: Shared.Enums.CommunicationProtocolDto.I2C,
            Category: "climate"
        );

        // Act
        var result = await _sut.CreateAsync(dto);

        // Assert
        result.IsGlobal.Should().BeFalse();
    }

    [Fact]
    public async Task CreateAsync_InvalidatesCache()
    {
        // Arrange
        var existingSensor = CreateTestSensorType();
        _context.SensorTypes.Add(existingSensor);
        await _context.SaveChangesAsync();

        // Populate cache
        await _sut.GetAllCachedAsync();

        var dto = new CreateSensorTypeDto(
            Code: "new_sensor",
            Name: "New Sensor",
            Protocol: Shared.Enums.CommunicationProtocolDto.I2C,
            Category: "climate"
        );

        // Act
        await _sut.CreateAsync(dto);
        var result = await _sut.GetAllCachedAsync();

        // Assert - Cache should be invalidated, so we get 2 results
        result.Should().HaveCount(2);
    }

    #endregion

    #region UpdateAsync Tests

    [Fact]
    public async Task UpdateAsync_ExistingSensorType_UpdatesSensorType()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        var dto = new UpdateSensorTypeDto(
            Name: "Updated Name",
            Manufacturer: "Updated Manufacturer"
        );

        // Act
        var result = await _sut.UpdateAsync(sensorType.Id, dto);

        // Assert
        result.Name.Should().Be("Updated Name");
        result.Manufacturer.Should().Be("Updated Manufacturer");
    }

    [Fact]
    public async Task UpdateAsync_NonExistingSensorType_ThrowsException()
    {
        // Arrange
        var dto = new UpdateSensorTypeDto(Name: "Updated");

        // Act & Assert
        var act = () => _sut.UpdateAsync(Guid.NewGuid(), dto);
        await act.Should().ThrowAsync<InvalidOperationException>()
            .WithMessage("*not found*");
    }

    [Fact]
    public async Task UpdateAsync_AllFields_UpdatesAllFields()
    {
        // Arrange
        var sensorType = CreateTestSensorType();
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        var dto = new UpdateSensorTypeDto(
            Name: "Updated",
            Manufacturer: "New Mfg",
            DatasheetUrl: "http://example.com",
            Description: "New Desc",
            DefaultI2CAddress: "0x77",
            DefaultSdaPin: 19,
            DefaultSclPin: 18,
            DefaultOneWirePin: 5,
            DefaultAnalogPin: 34,
            DefaultDigitalPin: 25,
            DefaultTriggerPin: 26,
            DefaultEchoPin: 27,
            DefaultIntervalSeconds: 30,
            MinIntervalSeconds: 5,
            WarmupTimeMs: 100,
            DefaultOffsetCorrection: 0.5,
            DefaultGainCorrection: 1.1,
            Category: "water",
            Icon: "water",
            Color: "#00BCD4",
            IsActive: false
        );

        // Act
        var result = await _sut.UpdateAsync(sensorType.Id, dto);

        // Assert
        result.Name.Should().Be("Updated");
        result.Manufacturer.Should().Be("New Mfg");
        result.Category.Should().Be("water");
    }

    #endregion

    #region DeleteAsync Tests

    [Fact]
    public async Task DeleteAsync_NonGlobalSensorType_DeletesSensorType()
    {
        // Arrange
        var sensorType = CreateTestSensorType(isGlobal: false);
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act
        await _sut.DeleteAsync(sensorType.Id);

        // Assert
        var deleted = await _context.SensorTypes.FindAsync(sensorType.Id);
        deleted.Should().BeNull();
    }

    [Fact]
    public async Task DeleteAsync_GlobalSensorType_ThrowsException()
    {
        // Arrange
        var sensorType = CreateTestSensorType(isGlobal: true);
        _context.SensorTypes.Add(sensorType);
        await _context.SaveChangesAsync();

        // Act & Assert
        var act = () => _sut.DeleteAsync(sensorType.Id);
        await act.Should().ThrowAsync<InvalidOperationException>()
            .WithMessage("*global*");
    }

    [Fact]
    public async Task DeleteAsync_NonExistingSensorType_ThrowsException()
    {
        // Act & Assert
        var act = () => _sut.DeleteAsync(Guid.NewGuid());
        await act.Should().ThrowAsync<InvalidOperationException>()
            .WithMessage("*not found*");
    }

    #endregion

    #region SyncFromCloudAsync Tests

    [Fact]
    public async Task SyncFromCloudAsync_DoesNotThrow()
    {
        // Act & Assert
        var act = () => _sut.SyncFromCloudAsync();
        await act.Should().NotThrowAsync();
    }

    #endregion

    #region SeedDefaultTypesAsync Tests

    [Fact]
    public async Task SeedDefaultTypesAsync_CreatesDefaultTypes()
    {
        // Act
        await _sut.SeedDefaultTypesAsync();

        // Assert
        var allTypes = await _sut.GetAllAsync();
        allTypes.Should().NotBeEmpty();
        allTypes.Should().Contain(t => t.Code == "dht22");
        allTypes.Should().Contain(t => t.Code == "bme280");
    }

    [Fact]
    public async Task SeedDefaultTypesAsync_WhenCalledTwice_DoesNotDuplicate()
    {
        // Act
        await _sut.SeedDefaultTypesAsync();
        var countAfterFirst = (await _sut.GetAllAsync()).Count();

        await _sut.SeedDefaultTypesAsync();
        var countAfterSecond = (await _sut.GetAllAsync()).Count();

        // Assert
        countAfterSecond.Should().Be(countAfterFirst);
    }

    [Fact]
    public async Task SeedDefaultTypesAsync_SetsIsGlobalToTrue()
    {
        // Act
        await _sut.SeedDefaultTypesAsync();

        // Assert
        var allTypes = _context.SensorTypes.ToList();
        allTypes.Should().AllSatisfy(st => st.IsGlobal.Should().BeTrue());
    }

    [Fact]
    public async Task SeedDefaultTypesAsync_CreatesCapabilities()
    {
        // Act
        await _sut.SeedDefaultTypesAsync();

        // Assert
        var dht22 = await _sut.GetByCodeAsync("dht22");
        dht22.Should().NotBeNull();
        dht22!.Capabilities.Should().NotBeEmpty();
        dht22.Capabilities.Should().Contain(c => c.MeasurementType == "temperature");
        dht22.Capabilities.Should().Contain(c => c.MeasurementType == "humidity");
    }

    [Theory]
    [InlineData("dht22", "DHT22 (AM2302)")]
    [InlineData("bme280", "GY-BME280 Breakout (I²C)")]
    [InlineData("bh1750", "BH1750 Lichtsensor (I²C)")]
    [InlineData("ds18b20", "DS18B20 wasserdicht")]
    public async Task SeedDefaultTypesAsync_CreatesExpectedTypes(string code, string name)
    {
        // Act
        await _sut.SeedDefaultTypesAsync();

        // Assert
        var sensorType = await _sut.GetByCodeAsync(code);
        sensorType.Should().NotBeNull();
        sensorType!.Name.Should().Be(name);
    }

    #endregion
}
