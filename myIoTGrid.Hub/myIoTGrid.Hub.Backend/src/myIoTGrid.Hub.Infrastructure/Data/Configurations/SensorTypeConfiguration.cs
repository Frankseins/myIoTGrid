using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;
using myIoTGrid.Hub.Domain.Entities;

namespace myIoTGrid.Hub.Infrastructure.Data.Configurations;

/// <summary>
/// EF Core Configuration for SensorType Entity.
/// Hardware sensor library with default configurations.
/// </summary>
public class SensorTypeConfiguration : IEntityTypeConfiguration<SensorType>
{
    public void Configure(EntityTypeBuilder<SensorType> builder)
    {
        builder.ToTable("SensorTypes");

        // Primary Key
        builder.HasKey(st => st.Id);

        // Properties - Identification
        builder.Property(st => st.Code)
            .IsRequired()
            .HasMaxLength(50);

        builder.Property(st => st.Name)
            .IsRequired()
            .HasMaxLength(100);

        builder.Property(st => st.Manufacturer)
            .HasMaxLength(100);

        builder.Property(st => st.DatasheetUrl)
            .HasMaxLength(500);

        builder.Property(st => st.Description)
            .HasMaxLength(1000);

        // Properties - Protocol
        builder.Property(st => st.Protocol)
            .IsRequired();

        // Properties - Default Pins
        builder.Property(st => st.DefaultI2CAddress)
            .HasMaxLength(10);

        // Properties - Timing
        builder.Property(st => st.DefaultIntervalSeconds)
            .IsRequired()
            .HasDefaultValue(60);

        builder.Property(st => st.MinIntervalSeconds)
            .IsRequired()
            .HasDefaultValue(1);

        builder.Property(st => st.WarmupTimeMs)
            .HasDefaultValue(0);

        // Properties - Calibration
        builder.Property(st => st.DefaultOffsetCorrection)
            .HasDefaultValue(0.0);

        builder.Property(st => st.DefaultGainCorrection)
            .HasDefaultValue(1.0);

        // Properties - Categorization
        builder.Property(st => st.Category)
            .IsRequired()
            .HasMaxLength(50);

        builder.Property(st => st.Icon)
            .HasMaxLength(50);

        builder.Property(st => st.Color)
            .HasMaxLength(20);

        // Properties - Flags
        builder.Property(st => st.IsGlobal)
            .IsRequired()
            .HasDefaultValue(true);

        builder.Property(st => st.IsActive)
            .IsRequired()
            .HasDefaultValue(true);

        // Properties - Timestamps
        builder.Property(st => st.CreatedAt)
            .IsRequired();

        builder.Property(st => st.UpdatedAt)
            .IsRequired();

        // Relationships
        builder.HasMany(st => st.Capabilities)
            .WithOne(c => c.SensorType)
            .HasForeignKey(c => c.SensorTypeId)
            .OnDelete(DeleteBehavior.Cascade);

        builder.HasMany(st => st.Sensors)
            .WithOne(s => s.SensorType)
            .HasForeignKey(s => s.SensorTypeId)
            .OnDelete(DeleteBehavior.Restrict);

        // Indexes
        builder.HasIndex(st => st.Code).IsUnique();
        builder.HasIndex(st => st.Category);
        builder.HasIndex(st => st.Protocol);
        builder.HasIndex(st => st.IsGlobal);
        builder.HasIndex(st => st.IsActive);
    }
}
