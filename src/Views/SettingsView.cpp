#include "SettingsView.h"
#include "../Settings.h"

SettingsView::SettingsView(Display &display, Button &enterButton, HttpServer &httpServer)
    : View(display, enterButton), _httpServer(httpServer) {}

void SettingsView::onInitialized()
{
  showCurrentState();
}

void SettingsView::showCurrentState()
{
  if (_httpServer.isRunning())
    showEndpointInformation();
  else
    showStartPrompt();
}

void SettingsView::startEndpoint()
{
  drawBackground();
  display().write(Display::Center, 1, "STARTING");
  display().write(Display::Center, 2, "ENDPOINT");
  byte startResult = _httpServer.start();
  if (startResult != 0)
  {
    char errorValue[4];
    snprintf(errorValue, sizeof(errorValue), "%u", startResult);
    showError("WIFI START", errorValue);
  }
}

void SettingsView::stopEndpoint()
{
  drawBackground();
  display().write(Display::Center, 1, "STOPPING");
  display().write(Display::Center, 2, "ENDPOINT");
  byte stopResult = _httpServer.stop();
  if (stopResult != 0)
  {
    char errorValue[4];
    snprintf(errorValue, sizeof(errorValue), "%u", stopResult);
    showError("WIFI STOP", errorValue);
  }
}

void SettingsView::update()
{
  if (!enterButton().pressed())
    return;
  if (_httpServer.isRunning())
    stopEndpoint();
  else
    startEndpoint();
  showCurrentState();
}

void SettingsView::onDestroy()
{
  if (_httpServer.isRunning())
    stopEndpoint();
}

void SettingsView::showStartPrompt()
{
  display().clear();
  display().write(Display::Center, 0, "WI-FI");
  display().write(Display::Center, 1, "PRESS");
  display().write(Display::Center, 2, "ENTER");
  display().write(Display::Center, 3, "TO SERVE");
  display().write(Display::Center, 4, "ENDPOINT");
}

void SettingsView::showEndpointInformation()
{
  drawBackground();
  display().write(Display::Center, 0, "WI-FI");
  display().write(Display::Center, 1, Settings::WiFiAp::Ssid);
  display().write(Display::Center, 2, "Password");
  display().write(Display::Center, 3, Settings::WiFiAp::Password);
}
