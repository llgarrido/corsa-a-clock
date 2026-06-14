/*
  Corsa A Clock

  Digital clock project for an ESP32-based dashboard display with a
  retro-styled visual appearance inspired by classic car instruments.

  The sketch drives an ST7735 TFT screen, reads time from a DS3231 RTC,
  displays cabin temperature and humidity from an internal DHT11 sensor,
  reads an external SHT30/SHT3x I2C probe, and provides a button-controlled
  view system. A temporary WiFi access point with an embedded HTTP server can
  be started from the settings view to adjust the RTC date and time from a browser.

  Hardware:
    - ESP32 board
    - ST7735 TFT display
    - DS3231 real-time clock module
    - DHT11 internal temperature and humidity sensor
    - SHT30/SHT3x I2C external temperature and humidity sensor
    - Two navigation buttons
*/

#include "src/Display.h"
#include "src/Clock.h"
#include "src/ClimateSensors/ExternalClimateSensor.h"
#include "src/ClimateSensors/InternalClimateSensor.h"
#include "src/Button.h"
#include "src/IsmChat.h"
#include "src/StartupDiagnostics.h"
#include "src/View.h"
#include "src/ViewLocator.h"
#include "src/HttpServer.h"
#include "src/Views/ChatNotificationView.h"
#include "src/Views/ClimateView.h"
#include "src/Views/MainView.h"
#include "src/Views/SettingsView.h"
#include "src/Views/SplashScreenView.h"

/**
 * Identifiers for views that can be resolved directly by name.
 */
enum class ViewName : uint8_t
{
  /**
   * Initial splash screen shown during startup.
   */
  SplashScreen,

  /**
   * ISM chat message notification.
   */
  ChatNotification
};

static constexpr uint8_t viewId(ViewName name)
{
  return static_cast<uint8_t>(name);
}

// --- Display TFT ST7735 ---
#define DISPLAY_CS_PIN 5
#define DISPLAY_DC_PIN 16
#define DISPLAY_RST_PIN 17
#define DISPLAY_BL_PIN 32 // backlight

// --- RTC DS3231 ---
#define RTC_SQW_PIN 33

// --- Internal DHT11 ---
#define DHT_PIN 26

// --- SHT30/SHT3x ---
#define SHT30_I2C_ADDRESS 0x44

// --- Navigation buttons ---
#define KEY_TAB_PIN 14
#define KEY_ENTER_PIN 27

// --- CC1101 RF module ---
#define RF_MODULE_CS_PIN 25
#define RF_MODULE_GDO0_PIN 34
#define RF_MODULE_GDO2_PIN 36
#define RF_MODULE_SCK_PIN 18
#define RF_MODULE_MISO_PIN 19
#define RF_MODULE_MOSI_PIN 23

Display display(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_BL_PIN);
Clock rtc(RTC_SQW_PIN);
InternalClimateSensor internalClimateSensor(DHT_PIN);
ExternalClimateSensor externalClimateSensor(SHT30_I2C_ADDRESS);
Button buttonTab(KEY_TAB_PIN);
Button buttonEnter(KEY_ENTER_PIN);
HttpServer httpServer(rtc);
IsmChat ismChat(
    RF_MODULE_CS_PIN,
    RF_MODULE_GDO0_PIN,
    RF_MODULE_GDO2_PIN,
    RF_MODULE_SCK_PIN,
    RF_MODULE_MISO_PIN,
    RF_MODULE_MOSI_PIN);
SplashScreenView splashScreenView(display, buttonEnter);
ChatNotificationView chatNotificationView(display, buttonEnter, ismChat);
MainView mainView(display, buttonEnter, rtc, externalClimateSensor);
ClimateView climateView(display, buttonEnter, internalClimateSensor, externalClimateSensor);
SettingsView settingsView(display, buttonEnter, httpServer);
ViewLocator::NamedView namedViews[] = {
    {viewId(ViewName::SplashScreen), &splashScreenView},
    {viewId(ViewName::ChatNotification), &chatNotificationView}};
View *carouselViews[] = {&mainView, &climateView, &settingsView};
static constexpr uint8_t NAMED_VIEW_COUNT = sizeof(namedViews) / sizeof(namedViews[0]);
static constexpr uint8_t CAROUSEL_VIEW_COUNT = sizeof(carouselViews) / sizeof(carouselViews[0]);
View *currentView = nullptr;

void setup()
{
  display.begin();
  ViewLocator::registerNamedViews(namedViews, NAMED_VIEW_COUNT);
  currentView = ViewLocator::resolveView(viewId(ViewName::SplashScreen));
  ViewLocator::registerCarouselViews(carouselViews, CAROUSEL_VIEW_COUNT);
  buttonTab.begin();
  buttonEnter.begin();

  StartupDiagnostics::Result startupResults[] = {
      {"Clock", rtc.begin()},
      {"Int. Clima", internalClimateSensor.begin()},
      {"Ext. Clima", externalClimateSensor.begin()},
      {"ISM Chat", ismChat.begin()}};
  StartupDiagnostics::showErrors(
      *currentView,
      startupResults,
      sizeof(startupResults) / sizeof(startupResults[0]));

  currentView = ViewLocator::resolveNextCarouselView();
}

void loop()
{
  httpServer.handle();
  ismChat.handle();
  
  if (ismChat.hasMessage()) currentView = ViewLocator::resolveView(viewId(ViewName::ChatNotification));
  else if (buttonTab.pressed()) currentView = ViewLocator::resolveNextCarouselView();

  if ((currentView != nullptr) && (!currentView->update())) currentView = ViewLocator::resolveCurrentCarouselView();
}
