#include "View.h"
#include "Settings.h"

View::View(Display &display, Button &enterButton)
    : _display(display), _enterButton(enterButton) {}

void View::initialize()
{
  _enterButton.clear();
  drawBackground();
  onInitialized();
}

void View::destroy()
{
  onDestroy();
}

void View::update() {}

void View::showError(const char *label, const char *value)
{
  drawBackground();
  display().write(Display::Center, 0, label);
  display().write(Display::Center, 1, "Error");
  display().write(Display::Center, 2, value);
  delay(Settings::MessagePopupDelay);
}

Display &View::display()
{
  return _display;
}

Button &View::enterButton()
{
  return _enterButton;
}

void View::drawBackground()
{
  _display.clear();
  _display.write(0, 4, Settings::Display::Footer);
}

void View::onInitialized() {}

void View::onDestroy() {}
