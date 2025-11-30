namespace myIoTGrid.Hub.Domain.Enums;

/// <summary>
/// Communication protocol for sensor hardware.
/// Defines how the sensor communicates with the microcontroller.
/// </summary>
public enum CommunicationProtocol
{
    /// <summary>I2C bus communication (SDA/SCL)</summary>
    I2C = 1,

    /// <summary>SPI bus communication</summary>
    SPI = 2,

    /// <summary>OneWire protocol (e.g., DS18B20)</summary>
    OneWire = 3,

    /// <summary>Analog input (ADC)</summary>
    Analog = 4,

    /// <summary>UART serial communication</summary>
    UART = 5,

    /// <summary>Digital GPIO (e.g., DHT22)</summary>
    Digital = 6,

    /// <summary>Ultrasonic sensor (Trigger/Echo)</summary>
    UltraSonic = 7
}
