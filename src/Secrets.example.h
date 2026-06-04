#pragma once

/**
 * @brief Example local secrets file for public source control.
 *
 * Copy this file to Secrets.h and replace the placeholder values with local
 * credentials before building a device-specific firmware.
 */
class Secrets
{
public:
  /**
   * @brief Example secrets used by the WiFi access point.
   */
  class WiFiAp
  {
  public:
    /**
     * @brief Placeholder password used to connect to the WiFi access point.
     */
    inline static const char *const Password = "change-me";
  };
};
