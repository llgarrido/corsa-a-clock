#include "InternalClimateSensor.h"

static constexpr unsigned long READ_INTERVAL = 2000;

InternalClimateSensor::InternalClimateSensor(uint8_t pin) : _dht(pin), _pin(pin),
                                                            _lastRead(0), _temperature(0.0), _humidity(0), _valid(false) {}

byte InternalClimateSensor::normalizeError(int error)
{
  switch (SimpleDHTErrCode(error))
  {
  case SimpleDHTErrSuccess:
    return ClimateSensor::SUCCESS;

  case SimpleDHTErrStartLow:
    return ClimateSensor::ERROR_START_LOW;

  case SimpleDHTErrStartHigh:
    return ClimateSensor::ERROR_START_HIGH;

  case SimpleDHTErrDataLow:
    return ClimateSensor::ERROR_DATA_LOW;

  case SimpleDHTErrDataRead:
    return ClimateSensor::ERROR_DATA_READ;

  case SimpleDHTErrDataEOF:
    return ClimateSensor::ERROR_DATA_EOF;

  case SimpleDHTErrDataChecksum:
    return ClimateSensor::ERROR_CHECKSUM;

  case SimpleDHTErrZeroSamples:
    return ClimateSensor::ERROR_ZERO_SAMPLES;

  case SimpleDHTErrNoPin:
    return ClimateSensor::ERROR_NO_PIN;

  case SimpleDHTErrPinMode:
    return ClimateSensor::ERROR_PIN_MODE;

  default:
    return ClimateSensor::ERROR_COMMUNICATION_FAILED;
  }
}

byte InternalClimateSensor::read()
{
  float temperature = 0.0;
  float humidity = 0.0;
  int error = _dht.read2(_pin, &temperature, &humidity, NULL);
  _lastRead = millis();
  if (error != SimpleDHTErrSuccess)
    return normalizeError(error);
  _temperature = temperature;
  _humidity = (byte)humidity;
  _valid = true;
  return ClimateSensor::SUCCESS;
}

byte InternalClimateSensor::begin()
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
