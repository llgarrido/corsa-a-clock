#pragma once

#include <Wire.h>
#include "RTClib.h"

/**
 * @brief A class to handle RTC DS3231 with 1Hz square wave interrupt for timekeeping.
 * The class provides methods to initialize the RTC, check for time updates, get the current time
 */
class Clock
{

public:
  /**
   * @brief Construct a new Clock object.
   * @param sqwPin The pin connected to the SQW output of the RTC.
   */
  Clock(uint8_t sqwPin);

  /**
   * @brief Initialize the RTC.
   * @return 0 if successful, 1 if RTC initialization failed, 2 if RTC lost power and needs to be set.
   */
  byte begin();

  /**
   * @brief Check if the RTC has a valid date/time.
   * @return true if the RTC date/time can be trusted, false otherwise.
   */
  bool isValid();

  /**
   * @brief Check if a time update is available.
   * @return true if a time update is available, false otherwise.
   */
  bool tick();

  /**
   * @brief Get the current time from the RTC.
   * @return The current date and time.
   */
  DateTime now();

  /**
   * @brief Adjust the RTC time.
   * @param dateTime The new date and time.
   * @return 0 if successful, 1 if RTC initialization failed, 2 if RTC still reports lost power after adjustment.
   */
  byte adjust(const DateTime &dateTime);

private:
  RTC_DS3231 _rtc;
  uint8_t _sqwPin;
  bool _isValid;

  static volatile bool _tick;
  static portMUX_TYPE _tickMux;
  static void IRAM_ATTR _onSquareWave();
};
