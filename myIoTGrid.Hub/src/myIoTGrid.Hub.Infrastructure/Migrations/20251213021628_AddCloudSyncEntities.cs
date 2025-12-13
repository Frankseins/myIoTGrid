using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace myIoTGrid.Hub.Infrastructure.Migrations
{
    /// <inheritdoc />
    public partial class AddCloudSyncEntities : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<DateTime>(
                name: "SyncedAt",
                table: "Readings",
                type: "TEXT",
                nullable: true);

            migrationBuilder.AddColumn<Guid>(
                name: "CloudSensorId",
                table: "NodeSensorAssignments",
                type: "TEXT",
                nullable: true);

            migrationBuilder.AddColumn<DateTime>(
                name: "LastSyncedAt",
                table: "NodeSensorAssignments",
                type: "TEXT",
                nullable: true);

            migrationBuilder.CreateTable(
                name: "NodeSyncStates",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "TEXT", nullable: false),
                    NodeId = table.Column<Guid>(type: "TEXT", nullable: false),
                    CloudNodeId = table.Column<Guid>(type: "TEXT", nullable: true),
                    LastSyncAt = table.Column<DateTime>(type: "TEXT", nullable: true),
                    LastSyncSuccess = table.Column<bool>(type: "INTEGER", nullable: false, defaultValue: false),
                    LastSyncError = table.Column<string>(type: "TEXT", maxLength: 2000, nullable: true),
                    LastSyncDuration = table.Column<TimeSpan>(type: "TEXT", nullable: true),
                    TotalSyncs = table.Column<int>(type: "INTEGER", nullable: false, defaultValue: 0),
                    SuccessfulSyncs = table.Column<int>(type: "INTEGER", nullable: false, defaultValue: 0),
                    FailedSyncs = table.Column<int>(type: "INTEGER", nullable: false, defaultValue: 0),
                    TotalReadingsSynced = table.Column<long>(type: "INTEGER", nullable: false, defaultValue: 0L),
                    IsSyncing = table.Column<bool>(type: "INTEGER", nullable: false, defaultValue: false),
                    CurrentJobId = table.Column<Guid>(type: "TEXT", nullable: true),
                    UpdatedAt = table.Column<DateTime>(type: "TEXT", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_NodeSyncStates", x => x.Id);
                    table.ForeignKey(
                        name: "FK_NodeSyncStates_Nodes_NodeId",
                        column: x => x.NodeId,
                        principalTable: "Nodes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "SyncHistoryEntries",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "TEXT", nullable: false),
                    NodeId = table.Column<Guid>(type: "TEXT", nullable: false),
                    JobId = table.Column<Guid>(type: "TEXT", nullable: false),
                    StartedAt = table.Column<DateTime>(type: "TEXT", nullable: false),
                    CompletedAt = table.Column<DateTime>(type: "TEXT", nullable: true),
                    Duration = table.Column<TimeSpan>(type: "TEXT", nullable: true),
                    Success = table.Column<bool>(type: "INTEGER", nullable: false),
                    Error = table.Column<string>(type: "TEXT", maxLength: 2000, nullable: true),
                    NodeAction = table.Column<string>(type: "TEXT", maxLength: 50, nullable: false),
                    SensorsCreated = table.Column<int>(type: "INTEGER", nullable: false, defaultValue: 0),
                    SensorsUpdated = table.Column<int>(type: "INTEGER", nullable: false, defaultValue: 0),
                    ReadingsSynced = table.Column<int>(type: "INTEGER", nullable: false, defaultValue: 0)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_SyncHistoryEntries", x => x.Id);
                    table.ForeignKey(
                        name: "FK_SyncHistoryEntries_Nodes_NodeId",
                        column: x => x.NodeId,
                        principalTable: "Nodes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_NodeSensorAssignments_CloudSensorId",
                table: "NodeSensorAssignments",
                column: "CloudSensorId");

            migrationBuilder.CreateIndex(
                name: "IX_NodeSyncStates_CloudNodeId",
                table: "NodeSyncStates",
                column: "CloudNodeId");

            migrationBuilder.CreateIndex(
                name: "IX_NodeSyncStates_IsSyncing",
                table: "NodeSyncStates",
                column: "IsSyncing");

            migrationBuilder.CreateIndex(
                name: "IX_NodeSyncStates_LastSyncAt",
                table: "NodeSyncStates",
                column: "LastSyncAt");

            migrationBuilder.CreateIndex(
                name: "IX_NodeSyncStates_NodeId",
                table: "NodeSyncStates",
                column: "NodeId",
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_SyncHistoryEntries_JobId",
                table: "SyncHistoryEntries",
                column: "JobId");

            migrationBuilder.CreateIndex(
                name: "IX_SyncHistoryEntries_NodeId",
                table: "SyncHistoryEntries",
                column: "NodeId");

            migrationBuilder.CreateIndex(
                name: "IX_SyncHistoryEntries_NodeId_StartedAt",
                table: "SyncHistoryEntries",
                columns: new[] { "NodeId", "StartedAt" });

            migrationBuilder.CreateIndex(
                name: "IX_SyncHistoryEntries_StartedAt",
                table: "SyncHistoryEntries",
                column: "StartedAt");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "NodeSyncStates");

            migrationBuilder.DropTable(
                name: "SyncHistoryEntries");

            migrationBuilder.DropIndex(
                name: "IX_NodeSensorAssignments_CloudSensorId",
                table: "NodeSensorAssignments");

            migrationBuilder.DropColumn(
                name: "SyncedAt",
                table: "Readings");

            migrationBuilder.DropColumn(
                name: "CloudSensorId",
                table: "NodeSensorAssignments");

            migrationBuilder.DropColumn(
                name: "LastSyncedAt",
                table: "NodeSensorAssignments");
        }
    }
}
