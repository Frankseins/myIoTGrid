using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;
using myIoTGrid.Shared.Common.Entities;

namespace myIoTGrid.Hub.Infrastructure.Data.Configurations;

/// <summary>
/// EF Core Configuration for NodeSyncState Entity.
/// Tracks Cloud sync state for a Node.
/// </summary>
public class NodeSyncStateConfiguration : IEntityTypeConfiguration<NodeSyncState>
{
    public void Configure(EntityTypeBuilder<NodeSyncState> builder)
    {
        builder.ToTable("NodeSyncStates");

        // Primary Key
        builder.HasKey(s => s.Id);

        // Properties
        builder.Property(s => s.NodeId)
            .IsRequired();

        builder.Property(s => s.CloudNodeId);

        builder.Property(s => s.LastSyncAt);

        builder.Property(s => s.LastSyncSuccess)
            .IsRequired()
            .HasDefaultValue(false);

        builder.Property(s => s.LastSyncError)
            .HasMaxLength(2000);

        // Statistics
        builder.Property(s => s.TotalSyncs)
            .IsRequired()
            .HasDefaultValue(0);

        builder.Property(s => s.SuccessfulSyncs)
            .IsRequired()
            .HasDefaultValue(0);

        builder.Property(s => s.FailedSyncs)
            .IsRequired()
            .HasDefaultValue(0);

        builder.Property(s => s.TotalReadingsSynced)
            .IsRequired()
            .HasDefaultValue(0L);

        // Current Status
        builder.Property(s => s.IsSyncing)
            .IsRequired()
            .HasDefaultValue(false);

        builder.Property(s => s.CurrentJobId);

        builder.Property(s => s.UpdatedAt)
            .IsRequired();

        // Relationships
        builder.HasOne(s => s.Node)
            .WithOne(n => n.SyncState)
            .HasForeignKey<NodeSyncState>(s => s.NodeId)
            .OnDelete(DeleteBehavior.Cascade);

        // Indexes
        builder.HasIndex(s => s.NodeId).IsUnique();
        builder.HasIndex(s => s.CloudNodeId);
        builder.HasIndex(s => s.LastSyncAt);
        builder.HasIndex(s => s.IsSyncing);
    }
}
