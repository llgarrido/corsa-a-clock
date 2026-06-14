#pragma once

#include <Arduino.h>

#include "Button.h"
#include "View.h"

/**
 * Displays diagnostics and handles runtime recovery actions.
 */
class Diagnostics
{
public:
  /**
   * Startup result for a component that can report initialization errors.
   */
  struct StartupResult
  {
    /**
     * Component label shown on the splash screen when initialization fails.
     */
    const char *label;

    /**
     * Error code returned by the component initializer, or 0 on success.
     */
    byte error;
  };

  Diagnostics() = delete;

  /**
   * Shows non-zero startup results on the provided view.
   * @param diagnosticsView View used to show diagnostics.
   * @param results Startup results to inspect.
   * @param count Number of entries in results.
   */
  static void showStartupErrors(View &diagnosticsView, const StartupResult results[], size_t count);
