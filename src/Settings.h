#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#if __has_include("Secrets.h")
#include "Secrets.h"
#else
#include "Secrets.example.h"
#endif

/**
 * @brief Groups the application's global configuration values.
 *
 * Settings acts as a constants container to avoid magic values spread across
 * the codebase.
 */
class Settings
{
public:
  /**
   * @brief Duration for modal message popups.
   */
  inline static const unsigned long MessagePopupDelay = 8000;

  /**
   * @brief Configuration for physical buttons.
   */
  class Button
  {
  public:
    /**
     * @brief Minimum time after an interrupt before accepting a button press.
     */
    inline static const unsigned long DebounceDelay = 200;
  };

  /**
   * @brief Configuration for runtime diagnostics and recovery actions.
   */
  class Diagnostics
  {
  public:
    /**
     * @brief Time both navigation buttons must be held before restarting.
     */
    inline static const unsigned long RestartHoldDelay = 2000;
  };

  /**
   * @brief Configuration for the virtual LCD display.
   */
  class Display
  {
  public:
    /**
     * @brief Footer text shown on the last row of the device display.
     */
    inline static const char *const Footer = "~~~~ CORSA";

    /**
     * @brief Color for active pixels (bright yellow): #f7ff14.
     */
    inline static const uint16_t OnColor = 0xF7E2;

    /**
     * @brief Color for inactive pixels (dark yellow): #0f0f01.
     */
    inline static const uint16_t OffColor = 0x0860;
  };

  /**
   * @brief Configuration for the WiFi access point created by the device.
   */
  class WiFiAp
  {
  public:
    /**
     * @brief WiFi channel used by the access point.
     */
    inline static const int Channel = 11;

    /**
     * @brief Network name advertised by the access point.
     */
    inline static const char *const Ssid = "CORSA-A";

    /**
     * @brief Password used to connect to the WiFi network.
     */
    inline static const char *const Password = Secrets::WiFiAp::Password;

    /**
     * @brief Local IP address assigned to the access point.
     */
    inline static const IPAddress Ip = IPAddress(192, 168, 1, 1);
  };

  /**
   * @brief Configuration for the ISM-band chat receiver.
   */
  class IsmRx
  {
  public:
    /**
     * @brief RF carrier frequency used by the CC1101 receiver, in MHz.
     *
     * This value must be selected for the deployment region's ISM/SRD radio
     * regulations and for the actual frequency band supported by the CC1101
     * module variant installed in the device.
     */
    inline static constexpr float FrequencyMhz = 433.92f;

    /**
     * @brief Minimum accepted packet RSSI, in dBm.
     *
     * Packets below this signal level are treated as likely noise or distant
     * traffic and are ignored by the ISM chat receiver.
     */
    inline static constexpr float MinPacketRssiDbm = -100.0f;

    /**
     * @brief Maximum accepted CC1101 packet LQI.
     *
     * Lower LQI is better on the CC1101. Packets above this value are treated
     * as low-quality reception and are ignored by the ISM chat receiver.
     */
    inline static constexpr uint8_t MaxPacketLqi = 80;
  };
};
