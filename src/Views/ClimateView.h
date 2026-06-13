#pragma once

#include "../Button.h"
#include "../Display.h"
#include "../ClimateSensor.h"
#include "../View.h"

/**
 * View that displays internal and exterior climate sensor readings.
 */
class ClimateView : public View
{
public:
  /**
   * Creates a climate comparison view.
   *
   * @param display Display used to render the climate readings.
   * @param enterButton Button inherited by the base view.
   * @param internalClimateSensor Sensor used to read cabin temperature and humidity.
   * @param externalClimateSensor Sensor used to read exterior temperature and humidity.
   */
  ClimateView(Display &display, Button &enterButton, ClimateSensor &internalClimateSensor, ClimateSensor &externalClimateSensor);

  bool update() override;

protected:
  void onInitialized() override;

private:
  static constexpr unsigned long REFRESH_INTERVAL = 1000;
  static constexpr uint8_t INTERNAL_COLUMN = 0;
  static constexpr uint8_t EXTERNAL_COLUMN = 5;

  void displayClimate();

  ClimateSensor &_internalClimateSensor;
  ClimateSensor &_externalClimateSensor;
  unsigned long _lastRefresh;
};
