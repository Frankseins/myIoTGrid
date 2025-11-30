using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;
using myIoTGrid.Hub.Domain.Entities;

namespace myIoTGrid.Hub.Infrastructure.Data.Configurations;

/// <summary>
/// EF Core Configuration for Sensor Entity.
/// Concrete sensor instance with calibration settings.
/// </summary>
public class SensorConfiguration : IEntityTypeConfiguration<Sensor>
{
    public void Configure(EntityTypeBuilder<Sensor> builder)
    {
        builder.ToTable("Sensors");

        // Primary Key
        builder.HasKey(s => s.Id);

        // Properties
        builder.Property(s => s.TenantId)
            .IsRequired();

        builder.Property(s => s.SensorTypeId)
            .IsRequired();

        builder.Property(s => s.Name)
            .IsRequired()
            .HasMaxLength(200);

        builder.Property(s => s.Description)
            .HasMaxLength(1000);

        builder.Property(s => s.SerialNumber)
            .HasMaxLength(100);

        // Calibration
        builder.Property(s => s.OffsetCorrection)
            .HasDefaultValue(0.0);

        builder.Property(s => s.GainCorrection)
            .HasDefaultValue(1.0);

        builder.Property(s => s.CalibrationNotes)
            .HasMaxLength(1000);

        // Active Capabilities (stored as JSON)
        builder.Property(s => s.ActiveCapabilityIdsJson)
            .HasMaxLength(2000);

        // Flags
        builder.Property(s => s.IsActive)
            .IsRequired()
            .HasDefaultValue(true);

        // Timestamps
        builder.Property(s => s.CreatedAt)
            .IsRequired();

        builder.Property(s => s.UpdatedAt)
            .IsRequired();

        // Relationships
        builder.HasOne(s => s.Tenant)
            .WithMany()
            .HasForeignKey(s => s.TenantId)
            .OnDelete(DeleteBehavior.Cascade);

        builder.HasOne(s => s.SensorType)
            .WithMany(st => st.Sensors)
            .HasForeignKey(s => s.SensorTypeId)
            .OnDelete(DeleteBehavior.Restrict);

        builder.HasMany(s => s.NodeAssignments)
            .WithOne(a => a.Sensor)
            .HasForeignKey(a => a.SensorId)
            .OnDelete(DeleteBehavior.Restrict);

        // Indexes
        builder.HasIndex(s => s.TenantId);
        builder.HasIndex(s => s.SensorTypeId);
        builder.HasIndex(s => s.SerialNumber);
        builder.HasIndex(s => s.IsActive);
        builder.HasIndex(s => new { s.TenantId, s.SensorTypeId });
    }
}
