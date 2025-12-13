using System.Net.Http.Json;
using System.Text.Json;
using System.Text.Json.Serialization;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using myIoTGrid.Shared.Common.DTOs;
using myIoTGrid.Shared.Common.Options;
using myIoTGrid.Shared.Contracts.Services;

namespace myIoTGrid.Hub.Infrastructure.Cloud;

/// <summary>
/// HTTP client implementation for Grid.Cloud API communication.
/// Handles all Hub-to-Cloud sync operations.
/// </summary>
public class CloudApiClient : ICloudApiClient
{
    private readonly HttpClient _httpClient;
    private readonly CloudApiOptions _options;
    private readonly ILogger<CloudApiClient> _logger;
    private readonly JsonSerializerOptions _jsonOptions;

    public CloudApiClient(
        HttpClient httpClient,
        IOptions<CloudApiOptions> options,
        ILogger<CloudApiClient> logger)
    {
        _httpClient = httpClient;
        _options = options.Value;
        _logger = logger;

        // Configure HttpClient
        if (!string.IsNullOrEmpty(_options.BaseUrl))
        {
            _httpClient.BaseAddress = new Uri(_options.BaseUrl);
        }

        _httpClient.Timeout = TimeSpan.FromMinutes(_options.TimeoutMinutes);

        // Add API Key header if configured
        if (!string.IsNullOrEmpty(_options.ApiKey))
        {
            _httpClient.DefaultRequestHeaders.Add("X-Hub-ApiKey", _options.ApiKey);
        }

        _jsonOptions = new JsonSerializerOptions
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            PropertyNameCaseInsensitive = true,
            DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
        };
    }

    /// <inheritdoc />
    public bool IsConfigured =>
        _options.Enabled &&
        !string.IsNullOrEmpty(_options.BaseUrl) &&
        !string.IsNullOrEmpty(_options.ApiKey);

    /// <inheritdoc />
    public async Task<bool> TestConnectionAsync(CancellationToken ct = default)
    {
        if (!IsConfigured)
        {
            _logger.LogWarning("Cloud API is not configured");
            return false;
        }

        try
        {
            var response = await _httpClient.GetAsync("/health", ct);
            var isHealthy = response.IsSuccessStatusCode;

            if (isHealthy)
            {
                _logger.LogInformation("Successfully connected to Cloud API at {BaseUrl}", _options.BaseUrl);
            }
            else
            {
                _logger.LogWarning("Cloud API health check failed: {StatusCode}", response.StatusCode);
            }

            return isHealthy;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to connect to Cloud API at {BaseUrl}", _options.BaseUrl);
            return false;
        }
    }

    /// <inheritdoc />
    public async Task<SyncNodeResponseDto> UpsertNodeAsync(SyncNodeDto dto, CancellationToken ct = default)
    {
        if (!IsConfigured)
        {
            throw new InvalidOperationException("Cloud API is not configured");
        }

        _logger.LogInformation("Syncing node {NodeId} to Cloud", dto.NodeId);

        try
        {
            var response = await ExecuteWithRetryAsync(
                async () => await _httpClient.PostAsJsonAsync("/api/hub-sync/nodes", dto, _jsonOptions, ct),
                ct);

            response.EnsureSuccessStatusCode();

            var result = await response.Content.ReadFromJsonAsync<SyncNodeResponseDto>(_jsonOptions, ct);

            if (result == null)
            {
                throw new InvalidOperationException("Empty response from Cloud API");
            }

            _logger.LogInformation(
                "Node {NodeId} synced to Cloud: {Action} (CloudId: {CloudId})",
                dto.NodeId,
                result.WasCreated ? "Created" : "Updated",
                result.CloudId);

            return result;
        }
        catch (HttpRequestException ex)
        {
            _logger.LogError(ex, "Network error while syncing node {NodeId} to Cloud", dto.NodeId);
            throw new CloudSyncException($"Network error: {ex.Message}", ex);
        }
        catch (TaskCanceledException ex) when (!ct.IsCancellationRequested)
        {
            _logger.LogError(ex, "Timeout while syncing node {NodeId} to Cloud", dto.NodeId);
            throw new CloudSyncException("Sync timed out", ex);
        }
    }

    /// <inheritdoc />
    public async Task<SyncSensorsResponseDto> UpsertSensorsAsync(SyncSensorsDto dto, CancellationToken ct = default)
    {
        if (!IsConfigured)
        {
            throw new InvalidOperationException("Cloud API is not configured");
        }

        _logger.LogInformation(
            "Syncing {Count} sensors for node {NodeCloudId} to Cloud",
            dto.Sensors.Length,
            dto.NodeCloudId);

        try
        {
            // Endpoint per Konzept: POST /api/hub-sync/nodes/{cloudId}/sensors
            var response = await ExecuteWithRetryAsync(
                async () => await _httpClient.PostAsJsonAsync($"/api/hub-sync/nodes/{dto.NodeCloudId}/sensors", dto, _jsonOptions, ct),
                ct);

            response.EnsureSuccessStatusCode();

            var result = await response.Content.ReadFromJsonAsync<SyncSensorsResponseDto>(_jsonOptions, ct);

            if (result == null)
            {
                throw new InvalidOperationException("Empty response from Cloud API");
            }

            var created = result.Sensors.Count(s => s.WasCreated);
            var updated = result.Sensors.Length - created;

            _logger.LogInformation(
                "Sensors synced to Cloud: {Created} created, {Updated} updated",
                created,
                updated);

            return result;
        }
        catch (HttpRequestException ex)
        {
            _logger.LogError(ex, "Network error while syncing sensors to Cloud");
            throw new CloudSyncException($"Network error: {ex.Message}", ex);
        }
        catch (TaskCanceledException ex) when (!ct.IsCancellationRequested)
        {
            _logger.LogError(ex, "Timeout while syncing sensors to Cloud");
            throw new CloudSyncException("Sync timed out", ex);
        }
    }

    /// <inheritdoc />
    public async Task<SyncReadingsResponseDto> UploadReadingsAsync(SyncReadingsBatchDto dto, CancellationToken ct = default)
    {
        if (!IsConfigured)
        {
            throw new InvalidOperationException("Cloud API is not configured");
        }

        _logger.LogDebug(
            "Uploading batch of {Count} readings for node {NodeCloudId} to Cloud",
            dto.Readings.Length,
            dto.NodeCloudId);

        try
        {
            // Endpoint per Konzept: POST /api/hub-sync/nodes/{cloudId}/readings
            var response = await ExecuteWithRetryAsync(
                async () => await _httpClient.PostAsJsonAsync($"/api/hub-sync/nodes/{dto.NodeCloudId}/readings", dto, _jsonOptions, ct),
                ct);

            response.EnsureSuccessStatusCode();

            var result = await response.Content.ReadFromJsonAsync<SyncReadingsResponseDto>(_jsonOptions, ct);

            if (result == null)
            {
                // Default response if Cloud API doesn't return one
                result = new SyncReadingsResponseDto(dto.Readings.Length, 0);
            }

            _logger.LogDebug(
                "Readings batch uploaded: {Accepted} accepted, {Rejected} rejected",
                result.AcceptedCount,
                result.RejectedCount);

            return result;
        }
        catch (HttpRequestException ex)
        {
            _logger.LogError(ex, "Network error while uploading readings to Cloud");
            throw new CloudSyncException($"Network error: {ex.Message}", ex);
        }
        catch (TaskCanceledException ex) when (!ct.IsCancellationRequested)
        {
            _logger.LogError(ex, "Timeout while uploading readings to Cloud");
            throw new CloudSyncException("Sync timed out", ex);
        }
    }

    /// <summary>
    /// Executes an HTTP request with retry logic.
    /// </summary>
    private async Task<HttpResponseMessage> ExecuteWithRetryAsync(
        Func<Task<HttpResponseMessage>> action,
        CancellationToken ct)
    {
        var retries = 0;
        while (true)
        {
            try
            {
                return await action();
            }
            catch (HttpRequestException) when (retries < _options.RetryCount)
            {
                retries++;
                _logger.LogDebug(
                    "Retry {Retry}/{MaxRetries} after HTTP error",
                    retries,
                    _options.RetryCount);
                await Task.Delay(_options.RetryDelayMilliseconds, ct);
            }
            catch (TaskCanceledException) when (retries < _options.RetryCount && !ct.IsCancellationRequested)
            {
                retries++;
                _logger.LogDebug(
                    "Retry {Retry}/{MaxRetries} after timeout",
                    retries,
                    _options.RetryCount);
                await Task.Delay(_options.RetryDelayMilliseconds, ct);
            }
        }
    }
}

/// <summary>
/// Exception thrown when Cloud sync operations fail.
/// </summary>
public class CloudSyncException : Exception
{
    public CloudSyncException(string message) : base(message) { }
    public CloudSyncException(string message, Exception innerException) : base(message, innerException) { }
}
