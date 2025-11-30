using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;
using myIoTGrid.Hub.Domain.Entities;

namespace myIoTGrid.Hub.Infrastructure.Data.Configurations;

/// <summary>
/// EF Core Configuration for SensorTypeCapability Entity.
/// Defines measurement capabilities of a SensorType.
/// </summary>
public class SensorTypeCapabilityConfiguration : IEntityTypeConfiguration<SensorTypeCapability>
{
    public void Configure(EntityTypeBuilder<SensorTypeCapability> builder)
    {
        builder.ToTable("SensorTypeCapabilities");

        // Primary Key
        builder.HasKey(c => c.Id);

        // Properties
        builder.Property(c => c.SensorTypeId)
            .IsRequired();

        builder.Property(c => c.MeasurementType)
            .IsRequired()
            .HasMaxLength(50);

        builder.Property(c => c.DisplayName)
            .IsRequired()
            .HasMaxLength(100);

        builder.Property(c => c.Unit)
            .IsRequired()
            .HasMaxLength(20);

        builder.Property(c => c.Resolution)
            .HasDefaultValue(0.01);

        builder.Property(c => c.Accuracy)
            .HasDefaultValue(0.5);

        builder.Property(c => c.MatterClusterName)
            .HasMaxLength(100);

        builder.Property(c => c.SortOrder)
            .HasDefaultValue(0);

        builder.Property(c => c.IsActive)
            .IsRequired()
            .HasDefaultValue(true);

        // Indexes
        builder.HasIndex(c => c.SensorTypeId);
        builder.HasIndex(c => c.MeasurementType);
        builder.HasIndex(c => new { c.SensorTypeId, c.MeasurementType }).IsUnique();
        builder.HasIndex(c => c.MatterClusterId);
    }
}
