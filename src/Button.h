#pragma once

#include <Arduino.h>

/**
 * @brief A class to handle button input with debouncing.
 */
class Button
{
public:
  /**
   * @brief Constructor for the Button class.
   * @param pin The GPIO pin number where the button is connected.
   */
  explicit Button(uint8_t pin);

  /**
   * @brief Initializes the button by setting up the pin mode and interrupt.
   * Must be called in the setup() function before using the button.
   */
  void begin();

  /**
   * @brief Checks if the button has been pressed since the last check.
   * @return true if the button was pressed, false otherwise.
   */
  bool pressed();

  /**
   * @brief Checks whether the button is currently held down.
   * @return true if the button input is active, false otherwise.
   */
  bool down() const;

  /**
   * @brief Discards any pending press event.
   */
  void clear();

private:
  uint8_t _pin;
  volatile bool _flag;
  unsigned long _lastPress;

  void IRAM_ATTR _onInterrupt();
  static void IRAM_ATTR _handleInterrupt(void *arg);
};
