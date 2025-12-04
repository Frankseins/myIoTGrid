using myIoTGrid.Hub.Shared.DTOs.Chart;

namespace myIoTGrid.Hub.Service.Interfaces;

/// <summary>
/// Service for chart data and readings list.
/// </summary>
public interface IChartService
{
    /// <summary>
    /// Get chart data for a specific widget.
    /// </summary>
    Task<ChartDataDto?> GetChartDataAsync(
        Guid nodeId,
        Guid assignmentId,
        string measurementType,
        ChartInterval interval,
        CancellationToken ct = default);

    /// <summary>
    /// Get paginated readings list for a widget.
    /// </summary>
    Task<ReadingsListDto> GetReadingsListAsync(
        Guid nodeId,
        Guid assignmentId,
        string measurementType,
        ReadingsListRequestDto request,
        CancellationToken ct = default);

    /// <summary>
    /// Export readings to CSV.
    /// </summary>
    Task<byte[]> ExportToCsvAsync(
        Guid nodeId,
        Guid assignmentId,
        string measurementType,
        DateTime? from,
        DateTime? to,
        CancellationToken ct = default);
}
