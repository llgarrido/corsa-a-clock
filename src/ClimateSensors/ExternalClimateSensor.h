#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "ClimateSensor.h"

/**
 * SHT30/SHT3x I2C climate sensor including temperature and humidity.
 */
class ExternalClimateSensor : public ClimateSensor
{
public:
  /**
   * Constructor for the SHT30/SHT3x climate sensor class.
   * @param address I2C address for the SHT30/SHT3x sensor.
   * @param wire I2C bus used to communicate with the sensor.
   */
  ExternalClimateSensor(uint8_t address = 0x44, TwoWire &wire = Wire);

  byte begin() override;
  bool hasReadout() override;
  float getTemperature() override;
  byte getHumidity() override;

private:
  static constexpr uint8_t CRC_POLYNOMIAL = 0x31;
  static constexpr uint8_t CRC_INITIAL_VALUE = 0xFF;

  byte startMeasurement();
  byte readMeasurement();
  void updateIfNeeded();
  bool isCrcValid(uint8_t msb, uint8_t lsb, uint8_t expectedCrc);

  TwoWire &_wire;
  uint8_t _address;
  unsigned long _lastRead;
  unsigned long _measurementStarted;
  float _temperature;
  byte _humidity;
  bool _valid;
  bool _measurementPending;
};
