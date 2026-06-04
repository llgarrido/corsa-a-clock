#include "Button.h"
#include "Settings.h"

Button::Button(uint8_t pin)
    : _pin(pin), _flag(false), _lastPress(0) {}

void Button::begin()
{
  pinMode(_pin, INPUT_PULLUP);
  attachInterruptArg(digitalPinToInterrupt(_pin), _handleInterrupt, this, FALLING);
}

bool Button::pressed()
{
  noInterrupts();
  bool wasPressed = _flag;
  _flag = false;
  interrupts();

  if (!wasPressed)
    return false;

  unsigned long now = millis();
  if (_lastPress != 0 && now - _lastPress < Settings::Button::DebounceDelay)
    return false;

  _lastPress = now;
  return true;
}

void Button::clear()
{
  noInterrupts();
  _flag = false;
  interrupts();
}

void IRAM_ATTR Button::_onInterrupt()
{
  _flag = true;
}

void IRAM_ATTR Button::_handleInterrupt(void *arg)
{
  static_cast<Button *>(arg)->_onInterrupt();
}
