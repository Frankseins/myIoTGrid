using myIoTGrid.Hub.Shared.DTOs.Dashboard;

namespace myIoTGrid.Hub.Service.Interfaces;

/// <summary>
/// Service Interface for Dashboard data.
/// Provides location-grouped sensor widgets with sparkline data.
/// </summary>
public interface IDashboardService
{
    /// <summary>
    /// Returns dashboard data grouped by location with sparkline data.
    /// </summary>
    /// <param name="period">Time period for sparkline data (Hour, Day, Week)</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Dashboard with locations and sensor widgets</returns>
    Task<LocationDashboardDto> GetLocationDashboardAsync(
        SparklinePeriod period = SparklinePeriod.Day,
        CancellationToken ct = default);

    /// <summary>
    /// Returns dashboard data with filters applied.
    /// </summary>
    /// <param name="filter">Filter options</param>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Filtered dashboard with locations and sensor widgets</returns>
    Task<LocationDashboardDto> GetFilteredDashboardAsync(
        DashboardFilterDto filter,
        CancellationToken ct = default);

    /// <summary>
    /// Returns available filter options for the dashboard.
    /// </summary>
    /// <param name="ct">Cancellation token</param>
    /// <returns>Available locations and measurement types</returns>
    Task<DashboardFilterOptionsDto> GetFilterOptionsAsync(CancellationToken ct = default);
}
