/*
  Corsa A Clock

  Digital clock project for an ESP32-based dashboard display with a
  retro-styled visual appearance inspired by classic car instruments.

  The sketch drives an ST7735 TFT screen, reads time from a DS3231 RTC,
  displays temperature and humidity from a DHT11 sensor, and provides a
  button-controlled view system. A temporary WiFi access point with an
  embedded HTTP server can be started from the settings view to adjust the
  RTC date and time from a browser.

  Hardware:
    - ESP32 board
    - ST7735 TFT display
    - DS3231 real-time clock module
    - DHT11 internal temperature and humidity sensor
    - Two navigation buttons
*/

#include "src/Display.h"
#include "src/Clock.h"
#include "src/InternalClimateSensor.h"
#include "src/Button.h"
#include "src/View.h"
#include "src/ViewLocator.h"
#include "src/HttpServer.h"
#include "src/Views/MainView.h"
#include "src/Views/SettingsView.h"
#include "src/Views/SplashScreenView.h"

// --- Display TFT ST7735 ---
#define DISPLAY_CS_PIN 5
#define DISPLAY_DC_PIN 16
#define DISPLAY_RST_PIN 17
#define DISPLAY_BL_PIN 32 // backlight

// --- RTC DS3231 ---
#define RTC_SQW_PIN 33

// --- Internal DHT11 ---
#define DHT_PIN 26

// --- Navigation buttons ---
#define KEY_TAB_PIN 14
#define KEY_ENTER_PIN 27

Display display(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_BL_PIN);
Clock rtc(RTC_SQW_PIN);
InternalClimateSensor internalClimateSensor(DHT_PIN);
Button buttonTab(KEY_TAB_PIN);
Button buttonEnter(KEY_ENTER_PIN);
HttpServer httpServer(rtc);
SplashScreenView splashScreenView(display, buttonEnter);
MainView mainView(display, buttonEnter, rtc, climateSensor);
SettingsView settingsView(display, buttonEnter, httpServer);
ViewLocator::NamedView namedViews[] = {
    {ViewName::SplashScreen, &splashScreenView}};
View *carouselViews[] = {&mainView, &settingsView};
View *currentView = nullptr;

void setup()
{
  display.begin();
  ViewLocator::registerNamedViews(namedViews, 1);
  currentView = ViewLocator::resolveView(ViewName::SplashScreen);
  ViewLocator::registerCarouselViews(carouselViews, 2);
  buttonTab.begin();
  buttonEnter.begin();
  byte rtcInitializationResult = rtc.begin();
  byte internalClimateSensorInitializationError = internalClimateSensor.begin();
  {
    char errorValue[12];
    if (rtcInitializationResult != 0)
    {
      snprintf(errorValue, sizeof(errorValue), "%u", rtcInitializationResult);
      currentView->showError("Clock", errorValue);
    }
    if (internalClimateSensorInitializationError != 0)
    {
      snprintf(errorValue, sizeof(errorValue), "%d", internalClimateSensorInitializationError);
      currentView->showError("Internal", errorValue);
    }
    {
    }
  }
  currentView = ViewLocator::resolveNextCarouselView();
}

void loop()
{
  httpServer.handle();

  if (buttonTab.pressed())
  {
    currentView = ViewLocator::resolveNextCarouselView();
  }

  if (currentView != nullptr)
  {
    currentView->update();
  }
}
