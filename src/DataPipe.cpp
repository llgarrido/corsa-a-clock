#include "DataPipe.h"

const char *DataPipe::DAYS[7] = {
    "SUNDAY",
    "MONDAY",
    "TUESDAY",
    "WEDNESDAY",
    "THURSDAY",
    "FRIDAY",
    "SATURDAY"};

void DataPipe::formatDate(const DateTime &dateTime, char *buf, uint8_t len)
{
  if (isDefaultDateTime(dateTime))
  {
    snprintf(buf, len, "----------");
    return;
  }

  snprintf(buf, len, "%04d-%02d-%02d", dateTime.year(), dateTime.month(), dateTime.day());
}

void DataPipe::formatTime(const DateTime &dateTime, char *buf, uint8_t len)
{
  if (isDefaultDateTime(dateTime))
  {
    snprintf(buf, len, "--:--:--");
    return;
  }

  snprintf(buf, len, "%02d:%02d:%02d", dateTime.hour(), dateTime.minute(), dateTime.second());
}

void DataPipe::formatTemperature(bool valid, float temperature, char *buf, uint8_t len)
{
  if (valid)
  {
    int roundedTemperature = (int)(temperature >= 0 ? temperature + 0.5f : temperature - 0.5f);
    if (roundedTemperature < -10)
    {
      snprintf(buf, len, " LO*C");
    }
    else if (roundedTemperature > 99)
    {
      snprintf(buf, len, " HI*C");
    }
    else
    {
      snprintf(buf, len, "%3d*C", roundedTemperature);
    }
  }
  else
  {
    snprintf(buf, len, " --*C");
  }
}

void DataPipe::formatHumidity(bool valid, byte humidity, char *buf, uint8_t len)
{
  char value[4];
  if (valid)
  {
    if (humidity > 99)
    {
      snprintf(value, sizeof(value), "HI");
    }
    else
    {
      snprintf(value, sizeof(value), "%2u", (unsigned int)humidity);
    }
  }
  else
  {
    snprintf(value, sizeof(value), "--");
  }
  snprintf(buf, len, "%sRH", value);
}

const char *DataPipe::dayOfWeek(const DateTime &dateTime)
{
  if (isDefaultDateTime(dateTime))
    return "-";

  return DAYS[dateTime.dayOfTheWeek()];
}

bool DataPipe::isDefaultDateTime(const DateTime &dateTime)
{
  return dateTime.year() == 2000;
}
