#pragma once

#include <Arduino.h>

/**
 * @brief Provides simple validation helpers for user-supplied values.
 */
class Validator
{
public:
  /**
   * @brief Checks whether a string contains only decimal digits.
   *
   * Empty strings are considered invalid.
   *
   * @param value String value to validate.
   * @return true if the string contains one or more digits and no other
   * characters; false otherwise.
   */
  static bool isNumber(const String &value);

  /**
   * @brief Checks whether a value is within an inclusive range.
   *
   * @param value Value to validate.
   * @param min Inclusive lower bound.
   * @param max Inclusive upper bound.
   * @return true if @p value is greater than or equal to @p min and less than
   * or equal to @p max; false otherwise.
   */
  static bool isRange(uint value, uint min, uint max);
};
