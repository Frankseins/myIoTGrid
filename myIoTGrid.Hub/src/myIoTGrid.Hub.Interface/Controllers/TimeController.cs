using Microsoft.AspNetCore.Mvc;

namespace myIoTGrid.Hub.Interface.Controllers;

/// <summary>
/// REST API Controller für Zeit-Synchronisation
/// Ermöglicht ESP32-Sensoren die Zeit vom Hub zu holen (wenn kein Internet verfügbar)
/// </summary>
[ApiController]
[Route("api/[controller]")]
[Produces("application/json")]
public class TimeController : ControllerBase
{
    /// <summary>
    /// Gibt die aktuelle Server-Zeit zurück
    /// </summary>
    /// <returns>Aktuelle UTC-Zeit und Unix-Timestamp</returns>
    [HttpGet]
    [ProducesResponseType(typeof(TimeResponse), StatusCodes.Status200OK)]
    public IActionResult GetTime()
    {
        var now = DateTime.UtcNow;
        var unixTimestamp = ((DateTimeOffset)now).ToUnixTimeSeconds();

        return Ok(new TimeResponse(
            Utc: now,
            UnixTimestamp: unixTimestamp,
            Timezone: "UTC"
        ));
    }
}

/// <summary>
/// Time Response DTO
/// </summary>
/// <param name="Utc">UTC Zeit als ISO 8601 String</param>
/// <param name="UnixTimestamp">Unix Timestamp in Sekunden</param>
/// <param name="Timezone">Zeitzone (immer UTC)</param>
public record TimeResponse(
    DateTime Utc,
    long UnixTimestamp,
    string Timezone
);
