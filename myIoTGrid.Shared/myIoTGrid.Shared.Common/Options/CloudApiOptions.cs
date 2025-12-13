namespace myIoTGrid.Shared.Common.Options;

/// <summary>
/// Configuration options for Grid.Cloud API communication.
/// </summary>
public class CloudApiOptions
{
    /// <summary>Configuration section name</summary>
    public const string SectionName = "Cloud";

    /// <summary>Whether Cloud sync is enabled</summary>
    public bool Enabled { get; set; } = true;

    /// <summary>Base URL of the Cloud API (e.g., "https://api.myiotgrid.cloud")</summary>
    public string BaseUrl { get; set; } = "https://api.myiotgrid.cloud";

    /// <summary>API Key for authentication (Hub-specific)</summary>
    public string ApiKey { get; set; } = string.Empty;

    /// <summary>Timeout for sync operations in minutes</summary>
    public int TimeoutMinutes { get; set; } = 5;

    /// <summary>Number of retries for failed requests</summary>
    public int RetryCount { get; set; } = 3;

    /// <summary>Delay between retries in milliseconds</summary>
    public int RetryDelayMilliseconds { get; set; } = 1000;

    /// <summary>Number of readings to upload per batch</summary>
    public int ReadingBatchSize { get; set; } = 1000;
}
