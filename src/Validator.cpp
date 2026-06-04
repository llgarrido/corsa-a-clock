#include "Validator.h"

bool Validator::isNumber(const String &value)
{
  if (value.length() == 0)
    return false;

  for (size_t i = 0; i < value.length(); i++)
  {
    if (!isDigit(value[i]))
      return false;
  }

  return true;
}

bool Validator::isRange(uint value, uint min, uint max)
{
  return value >= min && value <= max;
}
