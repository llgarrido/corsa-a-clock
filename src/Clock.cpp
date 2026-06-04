#include "Clock.h"

volatile bool Clock::_tick = false;
portMUX_TYPE Clock::_tickMux = portMUX_INITIALIZER_UNLOCKED;

Clock::Clock(uint8_t sqwPin)
    : _sqwPin(sqwPin),
      _isValid(false) {}

void IRAM_ATTR Clock::_onSquareWave()
{
  portENTER_CRITICAL_ISR(&_tickMux);
  _tick = true;
  portEXIT_CRITICAL_ISR(&_tickMux);
}

byte Clock::begin()
{
  _isValid = false;

  if (!_rtc.begin())
    return 1;

  _rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  pinMode(_sqwPin, INPUT_PULLUP);
  detachInterrupt(digitalPinToInterrupt(_sqwPin));
  attachInterrupt(digitalPinToInterrupt(_sqwPin), _onSquareWave, FALLING);

  portENTER_CRITICAL(&_tickMux);
  _tick = true;
  portEXIT_CRITICAL(&_tickMux);

  if (_rtc.lostPower())
    return 2;

  _isValid = true;
  return 0;
}

bool Clock::isValid()
{
  return _isValid;
}

bool Clock::tick()
{
  bool ticked;

  portENTER_CRITICAL(&_tickMux);
  ticked = _tick;
  _tick = false;
  portEXIT_CRITICAL(&_tickMux);

  return ticked;
}

DateTime Clock::now()
{
  if (!_isValid)
    return DateTime();

  return _rtc.now();
}

byte Clock::adjust(const DateTime &dateTime)
{
  bool needsInitialization = !isValid();
  if (needsInitialization)
  {
    byte beginResult = begin();
    if (beginResult == 1)
      return beginResult;
  }

  _rtc.adjust(dateTime);

  if (needsInitialization)
  {
    // If begin() reported lostPower(), adjust() clears that state; run begin()
    // again so the clock can start returning RTC values.
    byte beginResult = begin();
    if (beginResult != 0)
      return beginResult;
  }

  portENTER_CRITICAL(&_tickMux);
  _tick = true;
  portEXIT_CRITICAL(&_tickMux);

  return 0;
}
