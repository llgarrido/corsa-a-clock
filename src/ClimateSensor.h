#pragma once

#include <Arduino.h>

/**
 * Common interface for climate sensors that provide temperature and humidity.
 */
class ClimateSensor
{
public:
  static constexpr byte SUCCESS = 0;
  static constexpr byte ERROR_COMMUNICATION_FAILED = 1;
  static constexpr byte ERROR_INCOMPLETE_READ = 2;
  static constexpr byte ERROR_CHECKSUM = 3;
  static constexpr byte ERROR_INVALID_READING = 4;
  static constexpr byte ERROR_START_LOW = 5;
  static constexpr byte ERROR_START_HIGH = 6;
  static constexpr byte ERROR_DATA_LOW = 7;
  static constexpr byte ERROR_DATA_READ = 8;
  static constexpr byte ERROR_DATA_EOF = 9;
  static constexpr byte ERROR_ZERO_SAMPLES = 10;
  static constexpr byte ERROR_NO_PIN = 11;
  static constexpr byte ERROR_PIN_MODE = 12;

  virtual ~ClimateSensor() = default;

  /**
   * Initializes the sensor. Must be called before any other methods.
   * @return Initialization result:
   * 0 = success.
   * 1 = communication with the sensor failed.
   * 2 = sensor response was incomplete or timed out.
   * 3 = sensor checksum or CRC validation failed.
   * 4 = sensor returned invalid values.
   * 5 = start low signal timed out.
   * 6 = start high signal timed out.
   * 7 = data low signal timed out.
   * 8 = data bit read timed out.
   * 9 = data ended before the expected payload was read.
   * 10 = no samples were read from the sensor.
   * 11 = sensor pin was not configured.
   * 12 = sensor pin mode could not be configured.
   */
  virtual byte begin() = 0;

  /**
   * Checks if a valid readout is available.
   * @return true if a valid readout is available, false otherwise.
   */
  virtual bool hasReadout() = 0;

  /**
   * Gets the temperature from the sensor.
   * @return The temperature in degrees Celsius.
   */
  virtual float getTemperature() = 0;

  /**
   * Gets the humidity from the sensor.
   * @return The humidity as a percentage.
   */
  virtual byte getHumidity() = 0;
};
