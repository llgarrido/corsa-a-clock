#pragma once

#include <Arduino.h>
#include "RTClib.h"

/**
 * @brief Provides helpers for formatting dates, times, and sensor readings.
 *
 * DataPipe centralizes the text representation used by the clock display.
 * All methods write to buffers provided by the caller, which retains ownership
 * of those buffers.
 */
class DataPipe
{
public:
  /**
   * @brief Formats a DateTime as an ISO-style date string.
   *
   * @param dateTime Date and time value to format.
   * @param buf Destination buffer.
   * @param len Destination buffer size, including the null terminator.
   */
  static void formatDate(const DateTime &dateTime, char *buf, uint8_t len);

  /**
   * @brief Formats a DateTime as a 24-hour time string.
   *
   * @param dateTime Date and time value to format.
   * @param buf Destination buffer.
   * @param len Destination buffer size, including the null terminator.
   */
  static void formatTime(const DateTime &dateTime, char *buf, uint8_t len);

  /**
   * @brief Formats a temperature reading for display.
   *
   * When @p valid is false, the numeric value is replaced with a placeholder.
   *
   * @param valid Indicates whether @p temperature contains a usable reading.
   * @param temperature Temperature in degrees Celsius.
   * @param buf Destination buffer.
   * @param len Destination buffer size, including the null terminator.
   */
  static void formatTemperature(bool valid, float temperature, char *buf, uint8_t len);

  /**
   * @brief Formats a relative humidity reading for display.
   *
   * When @p valid is false, the numeric value is replaced with a placeholder.
   *
   * @param valid Indicates whether @p humidity contains a usable reading.
   * @param humidity Relative humidity percentage.
   * @param buf Destination buffer.
   * @param len Destination buffer size, including the null terminator.
   */
  static void formatHumidity(bool valid, byte humidity, char *buf, uint8_t len);

  /**
   * @brief Returns the English weekday name for a DateTime.
   *
   * @param dateTime Date and time value whose weekday will be used.
   * @return Pointer to a static uppercase weekday string.
   */
  static const char *dayOfWeek(const DateTime &dateTime);

private:
  static const char *DAYS[7];
  static bool isDefaultDateTime(const DateTime &dateTime);
};
