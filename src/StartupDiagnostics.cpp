#include "StartupDiagnostics.h"

void StartupDiagnostics::showErrors(View &diagnosticsView, const Result results[], size_t count)
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
