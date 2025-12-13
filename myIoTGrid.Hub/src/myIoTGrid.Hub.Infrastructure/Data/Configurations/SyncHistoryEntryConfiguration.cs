using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;
using myIoTGrid.Shared.Common.Entities;

namespace myIoTGrid.Hub.Infrastructure.Data.Configurations;

/// <summary>
/// EF Core Configuration for SyncHistoryEntry Entity.
/// Provides audit trail for Hub-Cloud synchronization.
/// </summary>
public class SyncHistoryEntryConfiguration : IEntityTypeConfiguration<SyncHistoryEntry>
{
    public void Configure(EntityTypeBuilder<SyncHistoryEntry> builder)
    {
        builder.ToTable("SyncHistoryEntries");

        // Primary Key
        builder.HasKey(e => e.Id);

        // Properties
        builder.Property(e => e.NodeId)
            .IsRequired();

        builder.Property(e => e.JobId)
            .IsRequired();

        builder.Property(e => e.StartedAt)
            .IsRequired();

        builder.Property(e => e.CompletedAt);

        builder.Property(e => e.Success)
            .IsRequired();

        builder.Property(e => e.Error)
            .HasMaxLength(2000);

        // Statistics
        builder.Property(e => e.NodeAction)
            .HasMaxLength(50);

        builder.Property(e => e.SensorsCreated)
            .IsRequired()
            .HasDefaultValue(0);

        builder.Property(e => e.SensorsUpdated)
            .IsRequired()
            .HasDefaultValue(0);

        builder.Property(e => e.ReadingsSynced)
            .IsRequired()
            .HasDefaultValue(0);

        // Relationships
        builder.HasOne(e => e.Node)
            .WithMany(n => n.SyncHistory)
            .HasForeignKey(e => e.NodeId)
            .OnDelete(DeleteBehavior.Cascade);

        // Indexes
        builder.HasIndex(e => e.NodeId);
        builder.HasIndex(e => e.JobId);
        builder.HasIndex(e => e.StartedAt);
        builder.HasIndex(e => new { e.NodeId, e.StartedAt });
    }
}
