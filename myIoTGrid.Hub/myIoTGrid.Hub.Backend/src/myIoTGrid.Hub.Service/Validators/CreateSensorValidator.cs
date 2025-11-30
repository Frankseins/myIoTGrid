using FluentValidation;
using myIoTGrid.Hub.Shared.DTOs;

namespace myIoTGrid.Hub.Service.Validators;

/// <summary>
/// Validator for CreateSensorDto (physical sensor instance)
/// New model: Uses Guid SensorTypeId instead of string.
/// </summary>
public class CreateSensorValidator : AbstractValidator<CreateSensorDto>
{
    public CreateSensorValidator()
    {
        RuleFor(x => x.SensorTypeId)
            .NotEmpty()
            .WithMessage("SensorTypeId is required");

        RuleFor(x => x.Name)
            .NotEmpty()
            .WithMessage("Name is required")
            .MaximumLength(200)
            .WithMessage("Name must not exceed 200 characters");

        When(x => x.Description != null, () =>
        {
            RuleFor(x => x.Description)
                .MaximumLength(1000)
                .WithMessage("Description must not exceed 1000 characters");
        });

        When(x => x.SerialNumber != null, () =>
        {
            RuleFor(x => x.SerialNumber)
                .MaximumLength(100)
                .WithMessage("SerialNumber must not exceed 100 characters");
        });

        When(x => x.IntervalSecondsOverride.HasValue, () =>
        {
            RuleFor(x => x.IntervalSecondsOverride!.Value)
                .GreaterThan(0)
                .WithMessage("IntervalSecondsOverride must be greater than 0")
                .LessThanOrEqualTo(86400)
                .WithMessage("IntervalSecondsOverride must not exceed 86400 seconds (24 hours)");
        });
    }
}

/// <summary>
/// Validator for UpdateSensorDto
/// </summary>
public class UpdateSensorValidator : AbstractValidator<UpdateSensorDto>
{
    public UpdateSensorValidator()
    {
        When(x => x.Name != null, () =>
        {
            RuleFor(x => x.Name)
                .MaximumLength(200)
                .WithMessage("Name must not exceed 200 characters");
        });

        When(x => x.Description != null, () =>
        {
            RuleFor(x => x.Description)
                .MaximumLength(1000)
                .WithMessage("Description must not exceed 1000 characters");
        });

        When(x => x.SerialNumber != null, () =>
        {
            RuleFor(x => x.SerialNumber)
                .MaximumLength(100)
                .WithMessage("SerialNumber must not exceed 100 characters");
        });

        When(x => x.IntervalSecondsOverride.HasValue, () =>
        {
            RuleFor(x => x.IntervalSecondsOverride!.Value)
                .GreaterThan(0)
                .WithMessage("IntervalSecondsOverride must be greater than 0")
                .LessThanOrEqualTo(86400)
                .WithMessage("IntervalSecondsOverride must not exceed 86400 seconds (24 hours)");
        });
    }
}

/// <summary>
/// Validator for CalibrateSensorDto
/// </summary>
public class CalibrateSensorValidator : AbstractValidator<CalibrateSensorDto>
{
    public CalibrateSensorValidator()
    {
        RuleFor(x => x.OffsetCorrection)
            .Must(v => !double.IsNaN(v) && !double.IsInfinity(v))
            .WithMessage("OffsetCorrection must be a valid number");

        RuleFor(x => x.GainCorrection)
            .Must(v => !double.IsNaN(v) && !double.IsInfinity(v))
            .WithMessage("GainCorrection must be a valid number")
            .NotEqual(0)
            .WithMessage("GainCorrection cannot be zero");

        When(x => x.CalibrationNotes != null, () =>
        {
            RuleFor(x => x.CalibrationNotes)
                .MaximumLength(1000)
                .WithMessage("CalibrationNotes must not exceed 1000 characters");
        });

        When(x => x.CalibrationDueAt.HasValue, () =>
        {
            RuleFor(x => x.CalibrationDueAt!.Value)
                .GreaterThan(DateTime.UtcNow)
                .WithMessage("CalibrationDueAt must be in the future");
        });
    }
}

/// <summary>
/// Validator for LocationDto
/// </summary>
public class LocationValidator : AbstractValidator<LocationDto>
{
    public LocationValidator()
    {
        When(x => x.Name != null, () =>
        {
            RuleFor(x => x.Name)
                .MaximumLength(200)
                .WithMessage("Location name must not exceed 200 characters");
        });

        When(x => x.Latitude.HasValue, () =>
        {
            RuleFor(x => x.Latitude!.Value)
                .InclusiveBetween(-90, 90)
                .WithMessage("Latitude must be between -90 and 90");
        });

        When(x => x.Longitude.HasValue, () =>
        {
            RuleFor(x => x.Longitude!.Value)
                .InclusiveBetween(-180, 180)
                .WithMessage("Longitude must be between -180 and 180");
        });

        // If one coordinate is provided, both must be provided
        RuleFor(x => x)
            .Must(x => (x.Latitude.HasValue && x.Longitude.HasValue) ||
                       (!x.Latitude.HasValue && !x.Longitude.HasValue))
            .WithMessage("Both Latitude and Longitude must be provided together, or neither");
    }
}
