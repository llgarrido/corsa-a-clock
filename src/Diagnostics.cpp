#include "Diagnostics.h"

#include "Settings.h"

void Diagnostics::showStartupErrors(View &diagnosticsView, const StartupResult results[], size_t count)
{
  char errorValue[12];
  for (size_t i = 0; i < count; i++)
  {
    if (results[i].error == 0)
      continue;

    snprintf(errorValue, sizeof(errorValue), "%u", (unsigned int)results[i].error);
    diagnosticsView.showError(results[i].label, errorValue);
  }
}

bool Diagnostics::handleRestart(Button &firstButton, Button &secondButton)
{
  static bool holding = false;
  static unsigned long heldSince = 0;

  if (!firstButton.down() || !secondButton.down())
  {
    holding = false;
    heldSince = 0;
    return false;
  }

  firstButton.clear();
  secondButton.clear();

  unsigned long now = millis();
  if (!holding)
  {
    holding = true;
    heldSince = now;
  }

  if (now - heldSince >= Settings::Diagnostics::RestartHoldDelay)
    ESP.restart();

  return true;
}
