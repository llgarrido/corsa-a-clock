#pragma once

#include <Arduino.h>

/**
 * Common interface for climate sensors that provide temperature and humidity.
 */
class ClimateSensor
{
public:
  virtual ~ClimateSensor() = default;

  /**
   * Initializes the sensor. Must be called before any other methods.
   * @return 0 on success, non-zero error code on failure.
   */
  virtual int begin() = 0;

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
