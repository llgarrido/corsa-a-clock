#pragma once

#include "../Button.h"
#include "../Display.h"
#include "../View.h"

/**
 * Splash screen shown while the application is starting.
 */
class SplashScreenView : public View
{
public:
  /**
   * Creates a splash screen view.
   *
   * @param display Display used to render the loading message.
   * @param enterButton Button inherited by the base view.
   */
  SplashScreenView(Display &display, Button &enterButton);

protected:
  void onInitialized() override;
};
