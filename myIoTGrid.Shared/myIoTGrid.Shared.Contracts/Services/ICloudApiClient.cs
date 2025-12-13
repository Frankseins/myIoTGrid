using myIoTGrid.Shared.Common.DTOs;

namespace myIoTGrid.Shared.Contracts.Services;

/// <summary>
/// HTTP client interface for Grid.Cloud API communication.
/// Used by ManualCloudSyncService to upload data to Cloud.
/// </summary>
public interface ICloudApiClient
{
    /// <summary>
    /// Gets whether the Cloud API is configured and reachable.
    /// </summary>
    bool IsConfigured { get; }

    /// <summary>
    /// Tests the connection to the Cloud API.
    /// </summary>
    /// <param name="ct">Cancellation token</param>
    /// <returns>True if connection successful</returns>
    Task<bool> TestConnectionAsync(CancellationToken ct = default);

    /// <summary>
    /// Upserts a Node configuration to the Cloud.
    /// </summary>
    /// <param name="dto">Node configuration to sync</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Response with CloudId and action taken</returns>
    Task<SyncNodeResponseDto> UpsertNodeAsync(SyncNodeDto dto, CancellationToken ct = default);

    /// <summary>
    /// Upserts Sensor configurations to the Cloud.
    /// </summary>
    /// <param name="dto">Sensors configuration to sync</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Response with CloudIds for each sensor</returns>
    Task<SyncSensorsResponseDto> UpsertSensorsAsync(SyncSensorsDto dto, CancellationToken ct = default);

    /// <summary>
    /// Uploads a batch of readings to the Cloud.
    /// </summary>
    /// <param name="dto">Batch of readings to upload</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Response with acceptance statistics</returns>
    Task<SyncReadingsResponseDto> UploadReadingsAsync(SyncReadingsBatchDto dto, CancellationToken ct = default);
}
