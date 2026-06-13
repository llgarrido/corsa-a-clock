#pragma once

#include "ClimateSensor.h"
#include <SimpleDHT.h>

/**
 * Internal climate sensor including cabin temperature and humidity.
 */
class InternalClimateSensor : public ClimateSensor
{
public:
  /**
   * Constructor for InternalClimateSensor class.
   * @param pin GPIO pin connected to the DHT11 sensor data line.
   */
  InternalClimateSensor(uint8_t pin);

  byte begin() override;
  bool hasReadout() override;
  float getTemperature() override;
  byte getHumidity() override;

private:
  byte normalizeError(int error);
  byte read();
  void updateIfNeeded();

  SimpleDHT11 _dht;
  uint8_t _pin;
  unsigned long _lastRead;
  float _temperature;
  byte _humidity;
  bool _valid;
};
