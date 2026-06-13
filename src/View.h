#pragma once

#include "Button.h"
#include "Display.h"

/**
 * @brief Base class for all display views in the application.
 *
 * Owns the common view lifecycle and gives derived views access to the shared
 * display and enter button without taking ownership of either dependency.
 */
class View
{
public:
  /**
   * @brief Creates a view using the shared display and enter button.
   * @param display Display used to render the view.
   * @param enterButton Button used by the view for user input.
   */
  View(Display &display, Button &enterButton);

  /**
   * @brief Starts the view lifecycle.
   *
   * Draws the default background and then calls the initialization hook.
   */
  void initialize();

  /**
   * @brief Ends the view lifecycle.
   *
   * Calls the destruction hook so derived views can release transient state.
   */
  void destroy();

  /**
   * @brief Updates the view.
   *
   * Derived views can override this to redraw changing content or react to
   * input.
   *
   * @return true to keep the view active, false when it should be closed.
   */
  virtual bool update();

  /**
   * @brief Shows a blocking error popup.
   * @param label Error source or category to display.
   * @param value Error value or detail to display.
   */
  void showError(const char *label, const char *value);

protected:
  /**
   * @brief Provides access to the shared display.
   * @return Reference to the display passed to the constructor.
   */
  Display &display();

  /**
   * @brief Provides access to the shared enter button.
   * @return Reference to the enter button passed to the constructor.
   */
  Button &enterButton();

  /**
   * @brief Draws the static background for the view.
   *
   * Derived views can override this to replace the default CORSA background.
   */
  virtual void drawBackground();

  /**
   * @brief Hook called after the background is drawn during initialization.
   */
  virtual void onInitialized();

  /**
   * @brief Hook called when the view is destroyed.
   */
  virtual void onDestroy();

private:
  /// Display used by this view. Stored by reference and not owned.
  Display &_display;

  /// Enter button used by this view. Stored by reference and not owned.
  Button &_enterButton;
};
