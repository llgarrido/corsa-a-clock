#include "SplashScreenView.h"

SplashScreenView::SplashScreenView(Display &display, Button &enterButton)
    : View(display, enterButton) {}

void SplashScreenView::onInitialized()
{
  display().write(Display::Center, 2, "LOADING");
}
