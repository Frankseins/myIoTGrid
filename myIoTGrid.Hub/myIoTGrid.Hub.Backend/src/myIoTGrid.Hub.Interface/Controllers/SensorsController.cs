using Microsoft.AspNetCore.Mvc;
using myIoTGrid.Hub.Service.Interfaces;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Interface.Controllers;

/// <summary>
/// REST API Controller for Sensors (Concrete Instances).
/// Individual sensor instances with calibration settings.
/// </summary>
[ApiController]
[Route("api/[controller]")]
[Produces("application/json")]
public class SensorsController : ControllerBase
{
    private readonly ISensorService _sensorService;

    public SensorsController(ISensorService sensorService)
    {
        _sensorService = sensorService;
    }

    /// <summary>
    /// Returns all Sensors for the current tenant
    /// </summary>
    /// <param name="ct">Cancellation Token</param>
    /// <returns>List of Sensors</returns>
    [HttpGet]
    [ProducesResponseType(typeof(IEnumerable<SensorDto>), StatusCodes.Status200OK)]
    public async Task<IActionResult> GetAll(CancellationToken ct)
    {
        var sensors = await _sensorService.GetAllAsync(ct);
        return Ok(sensors);
    }

    /// <summary>
    /// Returns a Sensor by ID
    /// </summary>
    /// <param name="id">Sensor-ID</param>
    /// <param name="ct">Cancellation Token</param>
    /// <returns>The Sensor</returns>
    [HttpGet("{id:guid}")]
    [ProducesResponseType(typeof(SensorDto), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<IActionResult> GetById(Guid id, CancellationToken ct)
    {
        var sensor = await _sensorService.GetByIdAsync(id, ct);

        if (sensor == null)
            return NotFound();

        return Ok(sensor);
    }

    /// <summary>
    /// Returns Sensors by SensorType
    /// </summary>
    /// <param name="sensorTypeId">SensorType-ID</param>
    /// <param name="ct">Cancellation Token</param>
    /// <returns>List of Sensors</returns>
    [HttpGet("by-type/{sensorTypeId:guid}")]
    [ProducesResponseType(typeof(IEnumerable<SensorDto>), StatusCodes.Status200OK)]
    public async Task<IActionResult> GetBySensorType(Guid sensorTypeId, CancellationToken ct)
    {
        var sensors = await _sensorService.GetBySensorTypeAsync(sensorTypeId, ct);
        return Ok(sensors);
    }

    /// <summary>
    /// Creates a new Sensor instance
    /// </summary>
    /// <param name="dto">Sensor data</param>
    /// <param name="ct">Cancellation Token</param>
    /// <returns>The created Sensor</returns>
    [HttpPost]
    [ProducesResponseType(typeof(SensorDto), StatusCodes.Status201Created)]
    [ProducesResponseType(typeof(ProblemDetails), StatusCodes.Status400BadRequest)]
    public async Task<IActionResult> Create([FromBody] CreateSensorDto dto, CancellationToken ct)
    {
        try
        {
            var sensor = await _sensorService.CreateAsync(dto, ct);
            return CreatedAtAction(nameof(GetById), new { id = sensor.Id }, sensor);
        }
        catch (InvalidOperationException ex)
        {
            return BadRequest(new ProblemDetails
            {
                Title = "Invalid Request",
                Detail = ex.Message
            });
        }
    }

    /// <summary>
    /// Updates a Sensor
    /// </summary>
    /// <param name="id">Sensor-ID</param>
    /// <param name="dto">Update data</param>
    /// <param name="ct">Cancellation Token</param>
    /// <returns>The updated Sensor</returns>
    [HttpPut("{id:guid}")]
    [ProducesResponseType(typeof(SensorDto), StatusCodes.Status200OK)]
    [ProducesResponseType(typeof(ProblemDetails), StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<IActionResult> Update(Guid id, [FromBody] UpdateSensorDto dto, CancellationToken ct)
    {
        try
        {
            var sensor = await _sensorService.UpdateAsync(id, dto, ct);
            return Ok(sensor);
        }
        catch (InvalidOperationException ex)
        {
            return NotFound(new ProblemDetails
            {
                Title = "Not Found",
                Detail = ex.Message
            });
        }
    }

    /// <summary>
    /// Calibrates a Sensor with offset and gain corrections
    /// </summary>
    /// <param name="id">Sensor-ID</param>
    /// <param name="dto">Calibration data</param>
    /// <param name="ct">Cancellation Token</param>
    /// <returns>The calibrated Sensor</returns>
    /// <response code="200">Sensor successfully calibrated</response>
    /// <response code="404">Sensor not found</response>
    [HttpPost("{id:guid}/calibrate")]
    [ProducesResponseType(typeof(SensorDto), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<IActionResult> Calibrate(Guid id, [FromBody] CalibrateSensorDto dto, CancellationToken ct)
    {
        try
        {
            var sensor = await _sensorService.CalibrateAsync(id, dto, ct);
            return Ok(sensor);
        }
        catch (InvalidOperationException ex)
        {
            return NotFound(new ProblemDetails
            {
                Title = "Not Found",
                Detail = ex.Message
            });
        }
    }

    /// <summary>
    /// Deletes a Sensor
    /// </summary>
    /// <param name="id">Sensor-ID</param>
    /// <param name="ct">Cancellation Token</param>
    /// <returns>No content</returns>
    /// <response code="204">Sensor successfully deleted</response>
    /// <response code="400">Sensor has active assignments</response>
    /// <response code="404">Sensor not found</response>
    [HttpDelete("{id:guid}")]
    [ProducesResponseType(StatusCodes.Status204NoContent)]
    [ProducesResponseType(typeof(ProblemDetails), StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<IActionResult> Delete(Guid id, CancellationToken ct)
    {
        try
        {
            await _sensorService.DeleteAsync(id, ct);
            return NoContent();
        }
        catch (InvalidOperationException ex) when (ex.Message.Contains("assignment"))
        {
            return BadRequest(new ProblemDetails
            {
                Title = "Invalid Request",
                Detail = ex.Message
            });
        }
        catch (InvalidOperationException ex)
        {
            return NotFound(new ProblemDetails
            {
                Title = "Not Found",
                Detail = ex.Message
            });
        }
    }
}
