#include "HttpServer.h"
#include "Resources.h"
#include "Settings.h"
#include "Validator.h"

HttpServer::HttpServer(Clock &clock)
    : _webServer(80),
      _clock(clock),
      _running(false),
      _routesConfigured(false)
{
  _ip[0] = '\0';
}

void HttpServer::setDateTime()
{
  if (!_webServer.hasArg("yyyy") || !_webServer.hasArg("MM") || !_webServer.hasArg("dd") || !_webServer.hasArg("HH") || !_webServer.hasArg("mm") || !_webServer.hasArg("ss"))
  {
    _webServer.send(StatusCode::BadRequest, MimeType::Text, "Missing parameter");
    return;
  }

  String yearArg = _webServer.arg("yyyy");
  String monthArg = _webServer.arg("MM");
  String dayArg = _webServer.arg("dd");
  String hourArg = _webServer.arg("HH");
  String minuteArg = _webServer.arg("mm");
  String secondArg = _webServer.arg("ss");

  if (!Validator::isNumber(yearArg) ||
      !Validator::isNumber(monthArg) ||
      !Validator::isNumber(dayArg) ||
      !Validator::isNumber(hourArg) ||
      !Validator::isNumber(minuteArg) ||
      !Validator::isNumber(secondArg))
  {
    _webServer.send(StatusCode::BadRequest, MimeType::Text, "Invalid numeric value");
    return;
  }

  uint year = yearArg.toInt();
  uint month = monthArg.toInt();
  uint day = dayArg.toInt();
  uint hour = hourArg.toInt();
  uint minute = minuteArg.toInt();
  uint second = secondArg.toInt();
  if (!Validator::isRange(year, 2024, 2099) ||
      !Validator::isRange(month, 1, 12) ||
      !Validator::isRange(day, 1, 31) ||
      !Validator::isRange(hour, 0, 23) ||
      !Validator::isRange(minute, 0, 59) ||
      !Validator::isRange(second, 0, 59))
  {
    _webServer.send(StatusCode::BadRequest, MimeType::Text, "Value out of range");
    return;
  }

  DateTime dateTime(year, month, day, hour, minute, second);
  byte adjustResult = _clock.adjust(dateTime);
  if (adjustResult != 0)
  {
    _webServer.send(StatusCode::InternalServerError, MimeType::Text, "RTC unavailable");
    return;
  }

  _webServer.send(StatusCode::Ok);
}

void HttpServer::_handleRedirect()
{
  char location[24];
  snprintf(location, sizeof(location), "http://%s/", _ip);
  _webServer.sendHeader("Location", location, true);
  _webServer.send(StatusCode::Redirect);
}

void HttpServer::_configureRoutes()
{
  if (_routesConfigured)
    return;

  // Captive portal detection: redirect with 302 to trigger the popup.
  _webServer.on("/canonical.html", [this]()
                { _handleRedirect(); }); // Firefox
  _webServer.on("/generate_204", [this]()
                { _handleRedirect(); }); // Android
  _webServer.on("/hotspot-detect.html", [this]()
                { _handleRedirect(); }); // iOS
  _webServer.on("/connecttest.txt", [this]()
                { _handleRedirect(); }); // Windows
  _webServer.on("/check_network_status.txt", [this]()
                { _handleRedirect(); }); // NetworkManager GNOME

  _webServer.on("/", [this]()
                { _webServer.send_P(StatusCode::Ok, MimeType::Html, Resources::index); });
  _webServer.on("/index.js", [this]()
                { _webServer.send_P(StatusCode::Ok, MimeType::JavaScript, Resources::script); });
  _webServer.on("/styles.css", [this]()
                { _webServer.send_P(StatusCode::Ok, MimeType::Css, Resources::styles); });
  _webServer.on("/corsa.jpg", [this]()
                { _webServer.send_P(StatusCode::Ok, MimeType::Jpeg, reinterpret_cast<const char *>(Resources::corsa), sizeof(Resources::corsa)); });
  _webServer.on("/set", [this]()
                { setDateTime(); });

  _webServer.onNotFound([this]()
                        { _handleRedirect(); });

  _routesConfigured = true;
}

void HttpServer::_updateIp()
{
  IPAddress ip = WiFi.softAPIP();
  snprintf(_ip, sizeof(_ip), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}

byte HttpServer::start()
{
  if (_running)
    return 0;

  _configureRoutes();
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);

  bool apConfigured = WiFi.softAPConfig(Settings::WiFiAp::Ip, Settings::WiFiAp::Ip, IPAddress(255, 255, 255, 0));
  if (!apConfigured)
  {
    WiFi.mode(WIFI_OFF);
    return 1;
  }

  bool apStarted = WiFi.softAP(Settings::WiFiAp::Ssid, Settings::WiFiAp::Password, Settings::WiFiAp::Channel, false, ApMaxClients);
  if (!apStarted)
  {
    WiFi.mode(WIFI_OFF);
    return 2;
  }

  // Captive Portal: redirect all DNS requests to our IP.
  _dns.start(53, "*", Settings::WiFiAp::Ip);
  _updateIp();
  _webServer.begin();
  _running = true;
  return 0;
}

byte HttpServer::stop()
{
  if (!_running)
    return 0;

  _webServer.stop();
  _dns.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  _running = WiFi.getMode() != WIFI_OFF;
  return (_running) ? 1 : 0;
}

void HttpServer::handle()
{
  if (!_running)
    return;

  _dns.processNextRequest();
  _webServer.handleClient();
}

bool HttpServer::isRunning() const
{
  return _running;
}

const char *HttpServer::ip() const
{
  return _ip;
}
