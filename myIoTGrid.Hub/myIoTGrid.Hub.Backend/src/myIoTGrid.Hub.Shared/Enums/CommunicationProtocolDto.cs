namespace myIoTGrid.Hub.Shared.Enums;

/// <summary>
/// Communication protocol used by sensors.
/// Determines pin configuration and data handling.
/// </summary>
public enum CommunicationProtocolDto
{
    /// <summary>I²C Bus (SDA, SCL)</summary>
    I2C = 1,

    /// <summary>SPI Bus (MISO, MOSI, SCK, CS)</summary>
    SPI = 2,

    /// <summary>Dallas 1-Wire (single data pin)</summary>
    OneWire = 3,

    /// <summary>Analog input (ADC)</summary>
    Analog = 4,

    /// <summary>UART Serial (TX, RX)</summary>
    UART = 5,

    /// <summary>Digital GPIO (single pin)</summary>
    Digital = 6,

    /// <summary>Ultrasonic (Trigger, Echo)</summary>
    UltraSonic = 7
}
