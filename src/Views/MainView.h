#pragma once

#include "../Button.h"
#include "../ClimateSensor.h"
#include "../Clock.h"
#include "../Display.h"
#include "../View.h"

/**
 * Main clock view that displays the current date/time and climate readout.
 */
class MainView : public View
{
public:
  /**
   * Creates the main clock view.
   *
   * @param display Display used to render the date, time and climate readout.
   * @param enterButton Button inherited by the base view.
   * @param clock Clock used to read the current date and time.
   * @param climateSensor Sensor used to read temperature and humidity.
   */
  MainView(Display &display, Button &enterButton, Clock &clock, ClimateSensor &climateSensor);

  void update() override;

protected:
  void onInitialized() override;

private:
  void displayDateTime();
  void displayClimate();

  Clock &_clock;
  ClimateSensor &_climateSensor;
};
