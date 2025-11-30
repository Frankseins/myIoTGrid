using Microsoft.EntityFrameworkCore;
using myIoTGrid.Hub.Domain.Entities;

namespace myIoTGrid.Hub.Infrastructure.Data;

/// <summary>
/// Entity Framework Core DbContext for the Hub database
/// </summary>
public class HubDbContext : DbContext
{
    public HubDbContext(DbContextOptions<HubDbContext> options) : base(options)
    {
    }

    /// <summary>Tenants</summary>
    public DbSet<Tenant> Tenants => Set<Tenant>();

    /// <summary>Hubs (Raspberry Pi Gateways)</summary>
    public DbSet<Domain.Entities.Hub> Hubs => Set<Domain.Entities.Hub>();

    /// <summary>Nodes (ESP32/LoRa32 Devices) - Matter Nodes</summary>
    public DbSet<Node> Nodes => Set<Node>();

    // === Dreistufiges Sensor-Modell ===

    /// <summary>Sensor Types (Hardware Library) - e.g., DHT22, BME280</summary>
    public DbSet<SensorType> SensorTypes => Set<SensorType>();

    /// <summary>Sensor Type Capabilities (Measurement types per SensorType)</summary>
    public DbSet<SensorTypeCapability> SensorTypeCapabilities => Set<SensorTypeCapability>();

    /// <summary>Sensors (Instances with calibration)</summary>
    public DbSet<Sensor> Sensors => Set<Sensor>();

    /// <summary>Node Sensor Assignments (Hardware Binding) - Matter Endpoints</summary>
    public DbSet<NodeSensorAssignment> NodeSensorAssignments => Set<NodeSensorAssignment>();

    /// <summary>Readings (Measurement Data with raw + calibrated values)</summary>
    public DbSet<Reading> Readings => Set<Reading>();

    // === Synced Data from Cloud ===

    /// <summary>Synced Nodes (from Cloud)</summary>
    public DbSet<SyncedNode> SyncedNodes => Set<SyncedNode>();

    /// <summary>Synced Readings (from Cloud)</summary>
    public DbSet<SyncedReading> SyncedReadings => Set<SyncedReading>();

    // === Alerts ===

    /// <summary>Alert Types</summary>
    public DbSet<AlertType> AlertTypes => Set<AlertType>();

    /// <summary>Alerts/Warnings</summary>
    public DbSet<Alert> Alerts => Set<Alert>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        // Load all Configurations from this Assembly
        modelBuilder.ApplyConfigurationsFromAssembly(typeof(HubDbContext).Assembly);
    }
}
