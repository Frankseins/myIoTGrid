using System;
using Microsoft.EntityFrameworkCore.Migrations;
using Npgsql.EntityFrameworkCore.PostgreSQL.Metadata;

#nullable disable

namespace myIoTGrid.Cloud.Infrastructure.Migrations
{
    /// <inheritdoc />
    public partial class InitialCreate : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "AlertTypes",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    Code = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    Name = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: false),
                    Description = table.Column<string>(type: "character varying(1000)", maxLength: 1000, nullable: true),
                    DefaultLevel = table.Column<int>(type: "integer", nullable: false),
                    IconName = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: true),
                    IsGlobal = table.Column<bool>(type: "boolean", nullable: false, defaultValue: false),
                    CreatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_AlertTypes", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "SyncedNodes",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    CloudNodeId = table.Column<Guid>(type: "uuid", nullable: false),
                    NodeId = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    Name = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: false),
                    Source = table.Column<int>(type: "integer", nullable: false),
                    SourceDetails = table.Column<string>(type: "character varying(500)", maxLength: 500, nullable: true),
                    LocationName = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: true),
                    LocationLatitude = table.Column<double>(type: "double precision", nullable: true),
                    LocationLongitude = table.Column<double>(type: "double precision", nullable: true),
                    IsOnline = table.Column<bool>(type: "boolean", nullable: false, defaultValue: false),
                    LastSyncAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    CreatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_SyncedNodes", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "Tenants",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    Name = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: false),
                    CloudApiKey = table.Column<string>(type: "character varying(500)", maxLength: 500, nullable: true),
                    CreatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    LastSyncAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    IsActive = table.Column<bool>(type: "boolean", nullable: false, defaultValue: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Tenants", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "SyncedReadings",
                columns: table => new
                {
                    Id = table.Column<long>(type: "bigint", nullable: false)
                        .Annotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn),
                    SyncedNodeId = table.Column<Guid>(type: "uuid", nullable: false),
                    SensorCode = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    MeasurementType = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    Value = table.Column<double>(type: "double precision", nullable: false),
                    Unit = table.Column<string>(type: "character varying(50)", maxLength: 50, nullable: false),
                    Timestamp = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    SyncedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_SyncedReadings", x => x.Id);
                    table.ForeignKey(
                        name: "FK_SyncedReadings_SyncedNodes_SyncedNodeId",
                        column: x => x.SyncedNodeId,
                        principalTable: "SyncedNodes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "Hubs",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    TenantId = table.Column<Guid>(type: "uuid", nullable: false),
                    HubId = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    Name = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: false),
                    Description = table.Column<string>(type: "character varying(500)", maxLength: 500, nullable: true),
                    LastSeen = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    IsOnline = table.Column<bool>(type: "boolean", nullable: false, defaultValue: false),
                    CreatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    DefaultWifiSsid = table.Column<string>(type: "character varying(64)", maxLength: 64, nullable: true),
                    DefaultWifiPassword = table.Column<string>(type: "character varying(256)", maxLength: 256, nullable: true),
                    ApiUrl = table.Column<string>(type: "character varying(256)", maxLength: 256, nullable: true),
                    ApiPort = table.Column<int>(type: "integer", nullable: false, defaultValue: 5002)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Hubs", x => x.Id);
                    table.ForeignKey(
                        name: "FK_Hubs_Tenants_TenantId",
                        column: x => x.TenantId,
                        principalTable: "Tenants",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "Sensors",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    TenantId = table.Column<Guid>(type: "uuid", nullable: false),
                    Code = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    Name = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: false),
                    Description = table.Column<string>(type: "character varying(500)", maxLength: 500, nullable: true),
                    SerialNumber = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: true),
                    Manufacturer = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: true),
                    Model = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: true),
                    DatasheetUrl = table.Column<string>(type: "character varying(500)", maxLength: 500, nullable: true),
                    Protocol = table.Column<int>(type: "integer", nullable: false, defaultValue: 0),
                    I2CAddress = table.Column<string>(type: "character varying(10)", maxLength: 10, nullable: true),
                    SdaPin = table.Column<int>(type: "integer", nullable: true),
                    SclPin = table.Column<int>(type: "integer", nullable: true),
                    OneWirePin = table.Column<int>(type: "integer", nullable: true),
                    AnalogPin = table.Column<int>(type: "integer", nullable: true),
                    DigitalPin = table.Column<int>(type: "integer", nullable: true),
                    TriggerPin = table.Column<int>(type: "integer", nullable: true),
                    EchoPin = table.Column<int>(type: "integer", nullable: true),
                    BaudRate = table.Column<int>(type: "integer", nullable: true),
                    IntervalSeconds = table.Column<int>(type: "integer", nullable: false, defaultValue: 60),
                    MinIntervalSeconds = table.Column<int>(type: "integer", nullable: false, defaultValue: 1),
                    WarmupTimeMs = table.Column<int>(type: "integer", nullable: false, defaultValue: 0),
                    OffsetCorrection = table.Column<double>(type: "double precision", nullable: false, defaultValue: 0.0),
                    GainCorrection = table.Column<double>(type: "double precision", nullable: false, defaultValue: 1.0),
                    LastCalibratedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    CalibrationNotes = table.Column<string>(type: "character varying(1000)", maxLength: 1000, nullable: true),
                    CalibrationDueAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    Category = table.Column<string>(type: "character varying(50)", maxLength: 50, nullable: false),
                    Icon = table.Column<string>(type: "character varying(50)", maxLength: 50, nullable: true),
                    Color = table.Column<string>(type: "character varying(10)", maxLength: 10, nullable: true),
                    IsActive = table.Column<bool>(type: "boolean", nullable: false, defaultValue: true),
                    CreatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    UpdatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Sensors", x => x.Id);
                    table.ForeignKey(
                        name: "FK_Sensors_Tenants_TenantId",
                        column: x => x.TenantId,
                        principalTable: "Tenants",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "Nodes",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    HubId = table.Column<Guid>(type: "uuid", nullable: false),
                    NodeId = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    Name = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: false),
                    Protocol = table.Column<int>(type: "integer", nullable: false, defaultValue: 1),
                    LocationName = table.Column<string>(type: "character varying(200)", maxLength: 200, nullable: true),
                    LocationLatitude = table.Column<double>(type: "double precision", nullable: true),
                    LocationLongitude = table.Column<double>(type: "double precision", nullable: true),
                    FirmwareVersion = table.Column<string>(type: "character varying(50)", maxLength: 50, nullable: true),
                    BatteryLevel = table.Column<int>(type: "integer", nullable: true),
                    LastSeen = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    IsOnline = table.Column<bool>(type: "boolean", nullable: false, defaultValue: false),
                    CreatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    MacAddress = table.Column<string>(type: "character varying(17)", maxLength: 17, nullable: false),
                    ApiKeyHash = table.Column<string>(type: "character varying(256)", maxLength: 256, nullable: false),
                    Status = table.Column<int>(type: "integer", nullable: false, defaultValue: 0),
                    IsSimulation = table.Column<bool>(type: "boolean", nullable: false, defaultValue: false),
                    StorageMode = table.Column<int>(type: "integer", nullable: false, defaultValue: 0),
                    PendingSyncCount = table.Column<int>(type: "integer", nullable: false, defaultValue: 0),
                    LastSyncAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    LastSyncError = table.Column<string>(type: "character varying(500)", maxLength: 500, nullable: true),
                    DebugLevel = table.Column<int>(type: "integer", nullable: false, defaultValue: 1),
                    EnableRemoteLogging = table.Column<bool>(type: "boolean", nullable: false, defaultValue: false),
                    LastDebugChange = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    HardwareStatusJson = table.Column<string>(type: "jsonb", nullable: true),
                    HardwareStatusReportedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Nodes", x => x.Id);
                    table.ForeignKey(
                        name: "FK_Nodes_Hubs_HubId",
                        column: x => x.HubId,
                        principalTable: "Hubs",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "SensorCapabilities",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    SensorId = table.Column<Guid>(type: "uuid", nullable: false),
                    MeasurementType = table.Column<string>(type: "character varying(50)", maxLength: 50, nullable: false),
                    DisplayName = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    Unit = table.Column<string>(type: "character varying(20)", maxLength: 20, nullable: false),
                    MinValue = table.Column<double>(type: "double precision", nullable: true),
                    MaxValue = table.Column<double>(type: "double precision", nullable: true),
                    Resolution = table.Column<double>(type: "double precision", nullable: false, defaultValue: 0.01),
                    Accuracy = table.Column<double>(type: "double precision", nullable: false, defaultValue: 0.5),
                    MatterClusterId = table.Column<long>(type: "bigint", nullable: true),
                    MatterClusterName = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: true),
                    SortOrder = table.Column<int>(type: "integer", nullable: false, defaultValue: 0),
                    IsActive = table.Column<bool>(type: "boolean", nullable: false, defaultValue: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_SensorCapabilities", x => x.Id);
                    table.ForeignKey(
                        name: "FK_SensorCapabilities_Sensors_SensorId",
                        column: x => x.SensorId,
                        principalTable: "Sensors",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "Alerts",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    TenantId = table.Column<Guid>(type: "uuid", nullable: false),
                    HubId = table.Column<Guid>(type: "uuid", nullable: true),
                    NodeId = table.Column<Guid>(type: "uuid", nullable: true),
                    AlertTypeId = table.Column<Guid>(type: "uuid", nullable: false),
                    Level = table.Column<int>(type: "integer", nullable: false),
                    Message = table.Column<string>(type: "character varying(2000)", maxLength: 2000, nullable: false),
                    Recommendation = table.Column<string>(type: "character varying(2000)", maxLength: 2000, nullable: true),
                    Source = table.Column<int>(type: "integer", nullable: false),
                    CreatedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    ExpiresAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    AcknowledgedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    IsActive = table.Column<bool>(type: "boolean", nullable: false, defaultValue: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Alerts", x => x.Id);
                    table.ForeignKey(
                        name: "FK_Alerts_AlertTypes_AlertTypeId",
                        column: x => x.AlertTypeId,
                        principalTable: "AlertTypes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Restrict);
                    table.ForeignKey(
                        name: "FK_Alerts_Hubs_HubId",
                        column: x => x.HubId,
                        principalTable: "Hubs",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.SetNull);
                    table.ForeignKey(
                        name: "FK_Alerts_Nodes_NodeId",
                        column: x => x.NodeId,
                        principalTable: "Nodes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.SetNull);
                    table.ForeignKey(
                        name: "FK_Alerts_Tenants_TenantId",
                        column: x => x.TenantId,
                        principalTable: "Tenants",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "NodeDebugLogs",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    NodeId = table.Column<Guid>(type: "uuid", nullable: false),
                    NodeTimestamp = table.Column<long>(type: "bigint", nullable: false),
                    ReceivedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    Level = table.Column<int>(type: "integer", nullable: false),
                    Category = table.Column<int>(type: "integer", nullable: false),
                    Message = table.Column<string>(type: "character varying(4000)", maxLength: 4000, nullable: false),
                    StackTrace = table.Column<string>(type: "character varying(8000)", maxLength: 8000, nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_NodeDebugLogs", x => x.Id);
                    table.ForeignKey(
                        name: "FK_NodeDebugLogs_Nodes_NodeId",
                        column: x => x.NodeId,
                        principalTable: "Nodes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "NodeSensorAssignments",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uuid", nullable: false),
                    NodeId = table.Column<Guid>(type: "uuid", nullable: false),
                    SensorId = table.Column<Guid>(type: "uuid", nullable: false),
                    EndpointId = table.Column<int>(type: "integer", nullable: false),
                    Alias = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: true),
                    I2CAddressOverride = table.Column<string>(type: "character varying(10)", maxLength: 10, nullable: true),
                    SdaPinOverride = table.Column<int>(type: "integer", nullable: true),
                    SclPinOverride = table.Column<int>(type: "integer", nullable: true),
                    OneWirePinOverride = table.Column<int>(type: "integer", nullable: true),
                    AnalogPinOverride = table.Column<int>(type: "integer", nullable: true),
                    DigitalPinOverride = table.Column<int>(type: "integer", nullable: true),
                    TriggerPinOverride = table.Column<int>(type: "integer", nullable: true),
                    EchoPinOverride = table.Column<int>(type: "integer", nullable: true),
                    BaudRateOverride = table.Column<int>(type: "integer", nullable: true),
                    IntervalSecondsOverride = table.Column<int>(type: "integer", nullable: true),
                    IsActive = table.Column<bool>(type: "boolean", nullable: false, defaultValue: true),
                    LastSeenAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: true),
                    AssignedAt = table.Column<DateTime>(type: "timestamp with time zone", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_NodeSensorAssignments", x => x.Id);
                    table.ForeignKey(
                        name: "FK_NodeSensorAssignments_Nodes_NodeId",
                        column: x => x.NodeId,
                        principalTable: "Nodes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                    table.ForeignKey(
                        name: "FK_NodeSensorAssignments_Sensors_SensorId",
                        column: x => x.SensorId,
                        principalTable: "Sensors",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "Readings",
                columns: table => new
                {
                    Id = table.Column<long>(type: "bigint", nullable: false)
                        .Annotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn),
                    TenantId = table.Column<Guid>(type: "uuid", nullable: false),
                    NodeId = table.Column<Guid>(type: "uuid", nullable: false),
                    AssignmentId = table.Column<Guid>(type: "uuid", nullable: true),
                    MeasurementType = table.Column<string>(type: "character varying(100)", maxLength: 100, nullable: false),
                    RawValue = table.Column<double>(type: "double precision", nullable: false),
                    Value = table.Column<double>(type: "double precision", nullable: false),
                    Unit = table.Column<string>(type: "character varying(50)", maxLength: 50, nullable: false),
                    Timestamp = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    IsSyncedToCloud = table.Column<bool>(type: "boolean", nullable: false, defaultValue: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Readings", x => x.Id);
                    table.ForeignKey(
                        name: "FK_Readings_NodeSensorAssignments_AssignmentId",
                        column: x => x.AssignmentId,
                        principalTable: "NodeSensorAssignments",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.SetNull);
                    table.ForeignKey(
                        name: "FK_Readings_Nodes_NodeId",
                        column: x => x.NodeId,
                        principalTable: "Nodes",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_AlertTypeId",
                table: "Alerts",
                column: "AlertTypeId");

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_CreatedAt",
                table: "Alerts",
                column: "CreatedAt");

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_HubId",
                table: "Alerts",
                column: "HubId");

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_IsActive",
                table: "Alerts",
                column: "IsActive");

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_Level",
                table: "Alerts",
                column: "Level");

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_NodeId",
                table: "Alerts",
                column: "NodeId");

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_TenantId",
                table: "Alerts",
                column: "TenantId");

            migrationBuilder.CreateIndex(
                name: "IX_Alerts_TenantId_IsActive_CreatedAt",
                table: "Alerts",
                columns: new[] { "TenantId", "IsActive", "CreatedAt" });

            migrationBuilder.CreateIndex(
                name: "IX_AlertTypes_Code",
                table: "AlertTypes",
                column: "Code",
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_AlertTypes_IsGlobal",
                table: "AlertTypes",
                column: "IsGlobal");

            migrationBuilder.CreateIndex(
                name: "IX_Hubs_IsOnline",
                table: "Hubs",
                column: "IsOnline");

            migrationBuilder.CreateIndex(
                name: "IX_Hubs_TenantId",
                table: "Hubs",
                column: "TenantId");

            migrationBuilder.CreateIndex(
                name: "IX_Hubs_TenantId_HubId",
                table: "Hubs",
                columns: new[] { "TenantId", "HubId" },
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_NodeDebugLogs_Category",
                table: "NodeDebugLogs",
                column: "Category");

            migrationBuilder.CreateIndex(
                name: "IX_NodeDebugLogs_Level",
                table: "NodeDebugLogs",
                column: "Level");

            migrationBuilder.CreateIndex(
                name: "IX_NodeDebugLogs_NodeId",
                table: "NodeDebugLogs",
                column: "NodeId");

            migrationBuilder.CreateIndex(
                name: "IX_NodeDebugLogs_NodeId_ReceivedAt",
                table: "NodeDebugLogs",
                columns: new[] { "NodeId", "ReceivedAt" });

            migrationBuilder.CreateIndex(
                name: "IX_NodeDebugLogs_ReceivedAt",
                table: "NodeDebugLogs",
                column: "ReceivedAt");

            migrationBuilder.CreateIndex(
                name: "IX_Nodes_HubId",
                table: "Nodes",
                column: "HubId");

            migrationBuilder.CreateIndex(
                name: "IX_Nodes_HubId_NodeId",
                table: "Nodes",
                columns: new[] { "HubId", "NodeId" },
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_Nodes_IsOnline",
                table: "Nodes",
                column: "IsOnline");

            migrationBuilder.CreateIndex(
                name: "IX_Nodes_MacAddress",
                table: "Nodes",
                column: "MacAddress");

            migrationBuilder.CreateIndex(
                name: "IX_Nodes_Status",
                table: "Nodes",
                column: "Status");

            migrationBuilder.CreateIndex(
                name: "IX_NodeSensorAssignments_NodeId",
                table: "NodeSensorAssignments",
                column: "NodeId");

            migrationBuilder.CreateIndex(
                name: "IX_NodeSensorAssignments_NodeId_EndpointId",
                table: "NodeSensorAssignments",
                columns: new[] { "NodeId", "EndpointId" },
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_NodeSensorAssignments_SensorId",
                table: "NodeSensorAssignments",
                column: "SensorId");

            migrationBuilder.CreateIndex(
                name: "IX_Readings_AssignmentId",
                table: "Readings",
                column: "AssignmentId");

            migrationBuilder.CreateIndex(
                name: "IX_Readings_IsSyncedToCloud",
                table: "Readings",
                column: "IsSyncedToCloud");

            migrationBuilder.CreateIndex(
                name: "IX_Readings_MeasurementType",
                table: "Readings",
                column: "MeasurementType");

            migrationBuilder.CreateIndex(
                name: "IX_Readings_NodeId",
                table: "Readings",
                column: "NodeId");

            migrationBuilder.CreateIndex(
                name: "IX_Readings_NodeId_MeasurementType_Timestamp",
                table: "Readings",
                columns: new[] { "NodeId", "MeasurementType", "Timestamp" });

            migrationBuilder.CreateIndex(
                name: "IX_Readings_NodeId_Timestamp",
                table: "Readings",
                columns: new[] { "NodeId", "Timestamp" });

            migrationBuilder.CreateIndex(
                name: "IX_Readings_TenantId",
                table: "Readings",
                column: "TenantId");

            migrationBuilder.CreateIndex(
                name: "IX_Readings_TenantId_Timestamp",
                table: "Readings",
                columns: new[] { "TenantId", "Timestamp" });

            migrationBuilder.CreateIndex(
                name: "IX_Readings_Timestamp",
                table: "Readings",
                column: "Timestamp");

            migrationBuilder.CreateIndex(
                name: "IX_SensorCapabilities_SensorId",
                table: "SensorCapabilities",
                column: "SensorId");

            migrationBuilder.CreateIndex(
                name: "IX_SensorCapabilities_SensorId_MeasurementType",
                table: "SensorCapabilities",
                columns: new[] { "SensorId", "MeasurementType" },
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_Sensors_Category",
                table: "Sensors",
                column: "Category");

            migrationBuilder.CreateIndex(
                name: "IX_Sensors_IsActive",
                table: "Sensors",
                column: "IsActive");

            migrationBuilder.CreateIndex(
                name: "IX_Sensors_TenantId",
                table: "Sensors",
                column: "TenantId");

            migrationBuilder.CreateIndex(
                name: "IX_Sensors_TenantId_Code",
                table: "Sensors",
                columns: new[] { "TenantId", "Code" },
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_SyncedNodes_CloudNodeId",
                table: "SyncedNodes",
                column: "CloudNodeId",
                unique: true);

            migrationBuilder.CreateIndex(
                name: "IX_SyncedNodes_IsOnline",
                table: "SyncedNodes",
                column: "IsOnline");

            migrationBuilder.CreateIndex(
                name: "IX_SyncedNodes_NodeId",
                table: "SyncedNodes",
                column: "NodeId");

            migrationBuilder.CreateIndex(
                name: "IX_SyncedNodes_Source",
                table: "SyncedNodes",
                column: "Source");

            migrationBuilder.CreateIndex(
                name: "IX_SyncedReadings_MeasurementType",
                table: "SyncedReadings",
                column: "MeasurementType");

            migrationBuilder.CreateIndex(
                name: "IX_SyncedReadings_SensorCode",
                table: "SyncedReadings",
                column: "SensorCode");

            migrationBuilder.CreateIndex(
                name: "IX_SyncedReadings_SyncedNodeId",
                table: "SyncedReadings",
                column: "SyncedNodeId");

            migrationBuilder.CreateIndex(
                name: "IX_SyncedReadings_SyncedNodeId_Timestamp",
                table: "SyncedReadings",
                columns: new[] { "SyncedNodeId", "Timestamp" });

            migrationBuilder.CreateIndex(
                name: "IX_SyncedReadings_Timestamp",
                table: "SyncedReadings",
                column: "Timestamp");

            migrationBuilder.CreateIndex(
                name: "IX_Tenants_CloudApiKey",
                table: "Tenants",
                column: "CloudApiKey",
                unique: true,
                filter: "\"CloudApiKey\" IS NOT NULL");

            migrationBuilder.CreateIndex(
                name: "IX_Tenants_Name",
                table: "Tenants",
                column: "Name",
                unique: true);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "Alerts");

            migrationBuilder.DropTable(
                name: "NodeDebugLogs");

            migrationBuilder.DropTable(
                name: "Readings");

            migrationBuilder.DropTable(
                name: "SensorCapabilities");

            migrationBuilder.DropTable(
                name: "SyncedReadings");

            migrationBuilder.DropTable(
                name: "AlertTypes");

            migrationBuilder.DropTable(
                name: "NodeSensorAssignments");

            migrationBuilder.DropTable(
                name: "SyncedNodes");

            migrationBuilder.DropTable(
                name: "Nodes");

            migrationBuilder.DropTable(
                name: "Sensors");

            migrationBuilder.DropTable(
                name: "Hubs");

            migrationBuilder.DropTable(
                name: "Tenants");
        }
    }
}
