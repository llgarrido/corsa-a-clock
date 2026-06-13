#include "ClimateView.h"
#include "../DataPipe.h"

ClimateView::ClimateView(Display &display, Button &enterButton, ClimateSensor &internalClimateSensor, ClimateSensor &externalClimateSensor)
    : View(display, enterButton), _internalClimateSensor(internalClimateSensor), _externalClimateSensor(externalClimateSensor), _lastRefresh(0) {}

void ClimateView::onInitialized()
{
  displayClimate();
}

void ClimateView::update()
{
  unsigned long now = millis();
  if (now - _lastRefresh < REFRESH_INTERVAL)
    return;

  displayClimate();
}

void ClimateView::displayClimate()
{
  bool internalValid = _internalClimateSensor.hasReadout();
  bool externalValid = _externalClimateSensor.hasReadout();
  char value[6];

  display().write(INTERNAL_COLUMN, 0, "Int.");
  display().write(EXTERNAL_COLUMN, 0, "Ext.");

  DataPipe::formatTemperature(internalValid, _internalClimateSensor.getTemperature(), value, sizeof(value));
  display().write(INTERNAL_COLUMN, 1, value);

  DataPipe::formatTemperature(externalValid, _externalClimateSensor.getTemperature(), value, sizeof(value));
  display().write(EXTERNAL_COLUMN, 1, value);

  DataPipe::formatHumidity(internalValid, _internalClimateSensor.getHumidity(), value, sizeof(value));
  display().write(INTERNAL_COLUMN, 2, value);

  DataPipe::formatHumidity(externalValid, _externalClimateSensor.getHumidity(), value, sizeof(value));
  display().write(EXTERNAL_COLUMN, 2, value);

  _lastRefresh = millis();
}
