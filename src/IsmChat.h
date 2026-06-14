#pragma once

#include <Arduino.h>
#include <RadioLib.h>

/**
 * @brief Receives Flipper Zero Sub-GHz chat messages through a CC1101.
 *
 * Compatible with both the Flipper Zero Sub-GHz chat from the CLI and the
 * Enhanced Sub-GHz Chat app. The Flipper chat helper uses the GFSK 9.99 kbps
 * async preset. Messages sent from the app end in LF; CLI chat messages end
 * after a receive silence gap.
 */
class IsmChat
{
public:
  /**
   * @brief Maximum length of a decoded chat message.
   */
  static constexpr uint8_t MaxMessageLength = 64;

  /**
   * @brief Operation completed successfully.
   *
   * Returned when the receiver is already initialized or when the CC1101 was
   * configured and placed in receive mode.
   */
  static constexpr byte SUCCESS = 0;

  /**
   * @brief RadioLib could not detect the CC1101 on the SPI bus.
   *
   * Check the module power, SPI wiring, chip select pin, and GDO pin mapping.
   */
  static constexpr byte ERROR_CHIP_NOT_FOUND = 1;

  /**
   * @brief RadioLib detected the CC1101 but failed while starting the radio.
   *
   * This usually points to an unsupported radio state or a configuration/write
   * failure while applying the receiver preset.
   */
  static constexpr byte ERROR_RADIO_BEGIN = 2;

  /**
   * @brief Construct a new ISM chat receiver.
   * @param csPin Chip select pin for the RF module.
   * @param gdo0Pin GDO0 interrupt pin for packet reception.
   * @param gdo2Pin GDO2 pin for the RF module.
   * @param sckPin SPI clock pin.
   * @param misoPin SPI MISO pin.
   * @param mosiPin SPI MOSI pin.
   */
  IsmChat(uint8_t csPin, uint8_t gdo0Pin, uint8_t gdo2Pin, uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin);

  /**
   * @brief Initialize the RF receiver.
   * @return SUCCESS, ERROR_CHIP_NOT_FOUND, or ERROR_RADIO_BEGIN.
   */
  byte begin();

  /**
   * @brief Check whether a decoded chat message is waiting to be read.
   * @return true when readMessage() can copy a pending message.
   */
  bool hasMessage() const;

  /**
   * @brief Service the RF receiver and decode any complete chat messages.
   *
   * Call this frequently from the main loop after begin() succeeds.
   */
  void handle();

  /**
   * @brief Copy and consume the latest decoded chat message.
   * @param buf Destination buffer for the null-terminated message text.
   * @param len Length of the destination buffer, including the terminator.
   * @return true when a message was copied; false when no message is pending
   * or the destination buffer is invalid.
   */
  bool readMessage(char *buf, size_t len);

private:
  static constexpr uint8_t MaxPacketLength = 64;
  static constexpr unsigned long RadioMessageSilenceMs = 500;
  static constexpr uint8_t MaxPacketsPerHandle = 4;
  static constexpr float BitRateKbps = 9.99f;
  static constexpr float FrequencyDeviationKhz = 19.04f;
  static constexpr float ReceiverBandwidthKhz = 102.0f;
  static constexpr uint8_t PreambleLengthBits = 32;
  static constexpr uint16_t RadioBufferLength = 256;
  static constexpr uint8_t PacketStatusLength = 2;
  static constexpr uint8_t PacketStatusCrcOkMask = 0x80;
  static constexpr uint8_t PacketStatusLqiMask = 0x7F;
  static constexpr float RssiOffsetDb = 74.0f;
  static constexpr unsigned long PacketReadTimeoutMs = 100;

  Module _module;
  CC1101 _radio;
  uint8_t _csPin;
  uint8_t _sckPin;
  uint8_t _misoPin;
  uint8_t _mosiPin;
  bool _begun;
  char _currentMessage[MaxMessageLength + 1];
  char _latestMessage[MaxMessageLength + 1];
  uint8_t _radioBuffer[RadioBufferLength];
  uint8_t _currentMessageLength;
  bool _hasMessage;
  uint16_t _radioBufferLength;
  unsigned long _radioBufferLastByteAt;
  uint8_t _pendingPacketLength;
  unsigned long _pendingPacketStartedAt;

  byte configureRadio();
  void writeFlipperPreset();
  void enterReceiveMode();
  void flushReceiveFifo();
  static float packetRssiDbm(uint8_t rawRssi);
  bool receivePacket(uint8_t *packet, uint8_t &packetLength);
  void appendRadioByte(uint8_t value);
  void clearRadioBuffer();
  void readMessageFromRadioBuffer();
  void readPlainMessageFromRadioBuffer();
  void finalizeCurrentMessage();
};
