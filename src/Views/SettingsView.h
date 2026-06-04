#pragma once

#include "../Button.h"
#include "../Display.h"
#include "../HttpServer.h"
#include "../View.h"

/**
 * View that manages the settings screen for the HTTP endpoint.
 *
 * Shows the current endpoint state and lets the user start or stop it with the
 * enter button.
 */
class SettingsView : public View
{
public:
  /**
   * Creates a settings view.
   *
   * @param display Display used to render the settings screens.
   * @param enterButton Button used to start or stop the endpoint.
   * @param httpServer HTTP server controlled by this view.
   */
  SettingsView(Display &display, Button &enterButton, HttpServer &httpServer);
  void update() override;

protected:
  void onInitialized() override;
  void showCurrentState();
  void onDestroy() override;

private:
  HttpServer &_httpServer;

  void startEndpoint();
  void stopEndpoint();
  void showStartPrompt();
  void showEndpointInformation();
};
