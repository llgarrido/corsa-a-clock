#include "ClimateSensor.h"

static constexpr unsigned long CLIMATE_SENSOR_READ_INTERVAL = 2000;

ClimateSensor::ClimateSensor(uint8_t pin) : _dht(pin), _pin(pin),
                                            _lastRead(0), _temperature(0.0), _humidity(0), _valid(false) {}

int ClimateSensor::read()
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

int ClimateSensor::begin()
{
  _valid = false;
  return read();
}

void ClimateSensor::updateIfNeeded()
{
  unsigned long now = millis();
  if (now - _lastRead < CLIMATE_SENSOR_READ_INTERVAL)
    return;
  read();
}

bool ClimateSensor::hasReadout()
{
  updateIfNeeded();
  return _valid;
}

float ClimateSensor::getTemperature()
{
  updateIfNeeded();
  return _temperature;
}

byte ClimateSensor::getHumidity()
{
  updateIfNeeded();
  return _humidity;
}
