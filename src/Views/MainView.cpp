#include "MainView.h"
#include "../DataPipe.h"

MainView::MainView(Display &display, Button &enterButton, Clock &clock, ClimateSensor &externalClimateSensor)
    : View(display, enterButton), _clock(clock), _externalClimateSensor(externalClimateSensor) {}

void MainView::onInitialized()
{
  displayDateTime();
  displayClimate();
}

bool MainView::update()
{
  if (_clock.tick())
  {
    displayDateTime();
    displayClimate();
  }

  return true;
}

void MainView::displayDateTime()
{
  char date[11], time[11];
  DateTime now = _clock.now();
  DataPipe::formatDate(now, date, sizeof(date));
  DataPipe::formatTime(now, time, sizeof(time));
  const char *dayOfWeek = DataPipe::dayOfWeek(now);
  display().write(Display::Center, 0, date);
  display().write(Display::Center, 1, time);
  display().write(Display::Center, 2, dayOfWeek);
}

void MainView::displayClimate()
{
  bool isValid = _externalClimateSensor.hasReadout();
  char temperature[6];
  char humidity[5];
  DataPipe::formatTemperature(isValid, _externalClimateSensor.getTemperature(), temperature, sizeof(temperature));
  DataPipe::formatHumidity(isValid, _externalClimateSensor.getHumidity(), humidity, sizeof(humidity));
  display().write(0, 3, temperature);
  display().write(6, 3, humidity);
}
