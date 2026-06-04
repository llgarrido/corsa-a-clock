#pragma once

#include <SimpleDHT.h>

/**
 * Climate sensor including temperature and humidity.
 */
class ClimateSensor
{
public:
  /**
   * Constructor for ClimateSensor class.
   * @param pin GPIO pin connected to the DHT11 sensor data line
   */
  ClimateSensor(uint8_t pin);

  /**
   * Initializes the sensor. Must be called before any other methods.
   * @return 0 on success, non-zero error code on failure.
   */
  int begin();

  /**
   * Checks if a valid readout is available.
   * @return true if a valid readout is available, false otherwise.
   */
  bool hasReadout();

  /**
   * Gets the temperature from the sensor.
   * @return The temperature in degrees Celsius.
   */
  float getTemperature();

  /**
   * Gets the humidity from the sensor.
   * @return The humidity as a percentage.
   */
  byte getHumidity();

private:
  int read();
  void updateIfNeeded();

  SimpleDHT11 _dht;
  uint8_t _pin;
  unsigned long _lastRead;
  float _temperature;
  byte _humidity;
  bool _valid;
};
