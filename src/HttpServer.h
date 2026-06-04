#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "Clock.h"

/**
 * @brief A simple HTTP server that serves a web page to set the RTC time and displays the current time.
 */
class HttpServer
{
public:
  /**
   * @brief Construct a new HttpServer object.
   * @param clock A reference to the Clock object used to get and set the RTC time
   */
  HttpServer(Clock &clock);

  /**
   * @brief Start the HTTP server.
   * @return 0 on success, 1 if the access point configuration failed, 2 if the access point failed to start
   */
  byte start();

  /**
   * @brief Stop the HTTP server.
   * @return 0 on success, 1 if the server failed to stop
   */
  byte stop();

  /**
   * @brief Handle incoming HTTP requests.
   */
  void handle();

  /**
   * @brief Check if the server is currently running.
   */
  bool isRunning() const;

  /**
   * @brief Get the IP address of the server.
   * @return The IP address as a string
   */
  const char *ip() const;

private:
  class MimeType
  {
  public:
    inline static const char *const Css = "text/css; charset=utf-8";
    inline static const char *const Html = "text/html; charset=utf-8";
    inline static const char *const JavaScript = "application/javascript; charset=utf-8";
    inline static const char *const Jpeg = "image/jpeg";
    inline static const char *const Text = "text/plain";
  };

  enum StatusCode
  {
    Ok = 200,
    Redirect = 302,
    BadRequest = 400,
    InternalServerError = 500,
  };

  inline static const uint8_t ApMaxClients = 4;

  WebServer _webServer;
  DNSServer _dns;
  Clock &_clock;
  char _ip[16];
  bool _running;
  bool _routesConfigured;
  void _configureRoutes();
  void _updateIp();
  void setDateTime();
  void _handleRedirect();
};
