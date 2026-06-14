#pragma once

#include <Arduino.h>

#include "View.h"

/**
 * Displays startup diagnostics for components that return error codes.
 */
class StartupDiagnostics
{
public:
  /**
   * Startup result for a component that can report initialization errors.
   */
  struct Result
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

  StartupDiagnostics() = delete;

  /**
   * Shows non-zero startup results on the provided view.
   * @param diagnosticsView View used to show diagnostics.
   * @param results Startup results to inspect.
   * @param count Number of entries in results.
   */
  static void showErrors(View &diagnosticsView, const Result results[], size_t count);
};
