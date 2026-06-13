#include "ExternalClimateSensor.h"
#include <math.h>

static constexpr unsigned long READ_INTERVAL = 2000;
static constexpr unsigned long MEASUREMENT_DELAY = 20;
static constexpr uint8_t MEASURE_HIGH_REPEATABILITY = 0x24;
static constexpr uint8_t MEASURE_CLOCK_STRETCHING_DISABLED = 0x00;
static constexpr uint8_t READ_LENGTH = 6;

ExternalClimateSensor::ExternalClimateSensor(uint8_t address, TwoWire &wire) : _wire(wire), _address(address),
                                                                               _lastRead(0), _measurementStarted(0), _temperature(0.0), _humidity(0), _valid(false), _measurementPending(false) {}

bool ExternalClimateSensor::isCrcValid(uint8_t msb, uint8_t lsb, uint8_t expectedCrc)
{
  uint8_t crc = CRC_INITIAL_VALUE;
  uint8_t data[] = {msb, lsb};

  for (uint8_t i = 0; i < 2; i++)
  {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; bit++)
    {
      if ((crc & 0x80) != 0)
        crc = (crc << 1) ^ CRC_POLYNOMIAL;
      else
        crc <<= 1;
    }
  }

  return crc == expectedCrc;
}

byte ExternalClimateSensor::startMeasurement()
{
  unsigned long now = millis();

  _wire.beginTransmission(_address);
  _wire.write(MEASURE_HIGH_REPEATABILITY);
  _wire.write(MEASURE_CLOCK_STRETCHING_DISABLED);
  int error = _wire.endTransmission();
  if (error != 0)
    return ClimateSensor::ERROR_COMMUNICATION_FAILED;

  _lastRead = now;
  _measurementStarted = now;
  _measurementPending = true;
  return ClimateSensor::SUCCESS;
}

byte ExternalClimateSensor::readMeasurement()
{
  _measurementPending = false;

  if (_wire.requestFrom((int)_address, (int)READ_LENGTH) != READ_LENGTH)
    return ClimateSensor::ERROR_INCOMPLETE_READ;

  uint8_t data[READ_LENGTH];
  for (uint8_t i = 0; i < READ_LENGTH; i++)
    data[i] = _wire.read();

  if (!isCrcValid(data[0], data[1], data[2]) || !isCrcValid(data[3], data[4], data[5]))
    return ClimateSensor::ERROR_CHECKSUM;

  uint16_t rawTemperature = ((uint16_t)data[0] << 8) | data[1];
  uint16_t rawHumidity = ((uint16_t)data[3] << 8) | data[4];
  float temperature = -45.0f + (175.0f * rawTemperature / 65535.0f);
  float humidity = 100.0f * rawHumidity / 65535.0f;

  if (isnan(temperature) || isnan(humidity) || humidity < 0.0f || humidity > 100.0f)
    return ClimateSensor::ERROR_INVALID_READING;

  _temperature = temperature;
  _humidity = (byte)(humidity + 0.5f);
  _valid = true;
  return ClimateSensor::SUCCESS;
}

byte ExternalClimateSensor::begin()
{
  _valid = false;
  _measurementPending = false;
  _wire.begin();

  byte error = startMeasurement();
  if (error != 0)
    return error;

  delay(MEASUREMENT_DELAY);
  return readMeasurement();
}

void ExternalClimateSensor::updateIfNeeded()
{
  unsigned long now = millis();

  if (_measurementPending)
  {
    if (now - _measurementStarted < MEASUREMENT_DELAY)
      return;

    readMeasurement();
    return;
  }

  if (now - _lastRead < READ_INTERVAL)
    return;

  startMeasurement();
}

bool ExternalClimateSensor::hasReadout()
{
  updateIfNeeded();
  return _valid;
}

float ExternalClimateSensor::getTemperature()
{
  updateIfNeeded();
  return _temperature;
}

byte ExternalClimateSensor::getHumidity()
{
  updateIfNeeded();
  return _humidity;
}
