#include "InternalClimateSensor.h"

static constexpr unsigned long READ_INTERVAL = 2000;

InternalClimateSensor::InternalClimateSensor(uint8_t pin) : _dht(pin), _pin(pin),
                                                            _lastRead(0), _temperature(0.0), _humidity(0), _valid(false) {}

int InternalClimateSensor::read()
{
  float temperature = 0.0;
  float humidity = 0.0;
  int error = _dht.read2(_pin, &temperature, &humidity, NULL);
  _lastRead = millis();
  if (error != SimpleDHTErrSuccess)
    return error;
  _temperature = temperature;
  _humidity = (byte)humidity;
  _valid = true;
  return error;
}

int InternalClimateSensor::begin()
{
  _valid = false;
  return read();
}

void InternalClimateSensor::updateIfNeeded()
{
  unsigned long now = millis();
  if (now - _lastRead < READ_INTERVAL)
    return;
  read();
}

bool InternalClimateSensor::hasReadout()
{
  updateIfNeeded();
  return _valid;
}

float InternalClimateSensor::getTemperature()
{
  updateIfNeeded();
  return _temperature;
}

byte InternalClimateSensor::getHumidity()
{
  updateIfNeeded();
  return _humidity;
}
