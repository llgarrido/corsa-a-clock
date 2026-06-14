#include "IsmChat.h"
#include "Settings.h"

#include <SPI.h>

static void copyText(char *destination, size_t destinationLength, const char *source)
{
  if (destination == nullptr || destinationLength == 0)
  {
    return;
  }

  size_t i = 0;
  if (source != nullptr)
  {
    for (; i < destinationLength - 1 && source[i] != '\0'; i++)
    {
      destination[i] = source[i];
    }
  }
  destination[i] = '\0';
}

static void sendCommand(Module &module, uint8_t command)
{
  module.SPItransferStream(&command, 1, true, nullptr, nullptr, 0, false);
}

static bool isAcceptedRadioByte(uint8_t value)
{
  return (value >= 32 && value <= 126) || value == 27 || value == '\n' || value == '\r';
}

static bool isPrintableRadioByte(uint8_t value)
{
  return value >= 32 && value <= 126;
}

static bool isAnsiSequenceFinalByte(uint8_t value)
{
  return value >= 0x40 && value <= 0x7E;
}

static uint16_t skipAnsiSequence(const uint8_t *buffer, uint16_t bufferLength, uint16_t index)
{
  if (index + 1 >= bufferLength || buffer[index + 1] != '[')
  {
    return index + 1;
  }

  index += 2;
  while (index < bufferLength && !isAnsiSequenceFinalByte(buffer[index]))
  {
    index++;
  }

  if (index < bufferLength)
  {
    index++;
  }

  return index;
}

IsmChat::IsmChat(uint8_t csPin, uint8_t gdo0Pin, uint8_t gdo2Pin, uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin)
    : _module(csPin, gdo0Pin, RADIOLIB_NC, gdo2Pin),
      _radio(&_module),
      _csPin(csPin),
      _sckPin(sckPin),
      _misoPin(misoPin),
      _mosiPin(mosiPin),
      _begun(false),
      _currentMessageLength(0),
      _hasMessage(false),
      _radioBufferLength(0),
      _radioBufferLastByteAt(0),
      _pendingPacketLength(0),
      _pendingPacketStartedAt(0)
{
  _currentMessage[0] = '\0';
  _latestMessage[0] = '\0';
  clearRadioBuffer();
}

byte IsmChat::begin()
{
  if (_begun)
  {
    return SUCCESS;
  }

  SPI.begin(_sckPin, _misoPin, _mosiPin, _csPin);
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  delay(1);

  byte error = configureRadio();
  if (error != SUCCESS)
  {
    return error;
  }

  enterReceiveMode();

  _begun = true;
  return SUCCESS;
}

bool IsmChat::hasMessage() const
{
  return _hasMessage;
}

void IsmChat::handle()
{
  if (!_begun)
  {
    return;
  }

  for (uint8_t packetCount = 0; packetCount < MaxPacketsPerHandle; packetCount++)
  {
    uint8_t packet[MaxPacketLength];
    uint8_t packetLength = 0;
    if (!receivePacket(packet, packetLength))
    {
      break;
    }
    for (uint8_t i = 0; i < packetLength; i++)
    {
      appendRadioByte(packet[i]);
    }
  }

  const unsigned long now = millis();
  if (_radioBufferLength > 0)
  {
    if (now - _radioBufferLastByteAt > RadioMessageSilenceMs)
    {
      if (_radioBuffer[0] == 27)
      {
        readMessageFromRadioBuffer();
      }
      else
      {
        readPlainMessageFromRadioBuffer();
      }
      clearRadioBuffer();
    }
  }
}

bool IsmChat::readMessage(char *buf, size_t len)
{
  if (buf == nullptr || len == 0 || !_hasMessage)
  {
    return false;
  }

  copyText(buf, len, _latestMessage);
  _latestMessage[0] = '\0';
  _hasMessage = false;
  return true;
}

byte IsmChat::configureRadio()
{
  int radioState = _radio.begin(
      Settings::IsmRx::FrequencyMhz,
      BitRateKbps,
      FrequencyDeviationKhz,
      ReceiverBandwidthKhz,
      10,
      PreambleLengthBits);
  if (radioState != RADIOLIB_ERR_NONE)
  {
    if (radioState == RADIOLIB_ERR_CHIP_NOT_FOUND)
    {
      return ERROR_CHIP_NOT_FOUND;
    }

    return ERROR_RADIO_BEGIN;
  }

  writeFlipperPreset();
  return SUCCESS;
}

void IsmChat::writeFlipperPreset()
{
  _radio.setFrequency(Settings::IsmRx::FrequencyMhz);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_FIFOTHR, 0x47);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_PKTCTRL1, 0x04);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_PKTCTRL0, 0x05);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_FSCTRL1, 0x06);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_SYNC1, 0x46);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_SYNC0, 0x4C);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_ADDR, 0x00);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_PKTLEN, 0x00);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_MDMCFG4, 0xC8);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_MDMCFG3, 0x93);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_MDMCFG2, 0x12);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_DEVIATN, 0x34);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_MCSM0, 0x18);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_FOCCFG, 0x16);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_AGCCTRL2, 0x43);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_AGCCTRL1, 0x40);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_AGCCTRL0, 0x91);
  _module.SPIwriteRegister(RADIOLIB_CC1101_REG_WORCTRL, 0xFB);
}

void IsmChat::enterReceiveMode()
{
  sendCommand(_module, RADIOLIB_CC1101_CMD_IDLE);
  sendCommand(_module, RADIOLIB_CC1101_CMD_RX);
}

void IsmChat::flushReceiveFifo()
{
  sendCommand(_module, RADIOLIB_CC1101_CMD_IDLE);
  sendCommand(_module, RADIOLIB_CC1101_CMD_FLUSH_RX);
  sendCommand(_module, RADIOLIB_CC1101_CMD_RX);
}

float IsmChat::packetRssiDbm(uint8_t rawRssi)
{
  if (rawRssi >= 128)
  {
    return (((float)rawRssi - 256.0f) / 2.0f) - RssiOffsetDb;
  }

  return ((float)rawRssi / 2.0f) - RssiOffsetDb;
}

bool IsmChat::receivePacket(uint8_t *packet, uint8_t &packetLength)
{
  uint8_t rxBytesStatus = _module.SPIreadRegister(RADIOLIB_CC1101_REG_RXBYTES | RADIOLIB_CC1101_CMD_ACCESS_STATUS_REG);
  if ((rxBytesStatus & 0x80) != 0)
  {
    flushReceiveFifo();
    clearRadioBuffer();
    _pendingPacketLength = 0;
    _pendingPacketStartedAt = 0;
    return false;
  }

  uint8_t rxBytes = rxBytesStatus & 0x7F;
  if (_pendingPacketLength == 0)
  {
    if (rxBytes == 0)
    {
      return false;
    }

    uint8_t declaredLength = _module.SPIreadRegister(RADIOLIB_CC1101_REG_FIFO);
    if (declaredLength == 0 || declaredLength > MaxPacketLength)
    {
      flushReceiveFifo();
      clearRadioBuffer();
      _pendingPacketLength = 0;
      _pendingPacketStartedAt = 0;
      return false;
    }

    _pendingPacketLength = declaredLength;
    _pendingPacketStartedAt = millis();
  }

  rxBytesStatus = _module.SPIreadRegister(RADIOLIB_CC1101_REG_RXBYTES | RADIOLIB_CC1101_CMD_ACCESS_STATUS_REG);
  if ((rxBytesStatus & 0x80) != 0)
  {
    flushReceiveFifo();
    clearRadioBuffer();
    _pendingPacketLength = 0;
    _pendingPacketStartedAt = 0;
    return false;
  }

  rxBytes = rxBytesStatus & 0x7F;
  if (rxBytes < _pendingPacketLength + PacketStatusLength)
  {
    if (millis() - _pendingPacketStartedAt > PacketReadTimeoutMs)
    {
      flushReceiveFifo();
      clearRadioBuffer();
      _pendingPacketLength = 0;
      _pendingPacketStartedAt = 0;
    }

    return false;
  }

  packetLength = _pendingPacketLength;
  _module.SPIreadRegisterBurst(RADIOLIB_CC1101_REG_FIFO | RADIOLIB_CC1101_CMD_BURST, packetLength, packet);

  uint8_t status[2];
  _module.SPIreadRegisterBurst(RADIOLIB_CC1101_REG_FIFO | RADIOLIB_CC1101_CMD_BURST, sizeof(status), status);

  enterReceiveMode();

  _pendingPacketLength = 0;
  _pendingPacketStartedAt = 0;
  if ((status[1] & PacketStatusCrcOkMask) == 0)
  {
    clearRadioBuffer();
    return false;
  }

  const float rssiDbm = packetRssiDbm(status[0]);
  const uint8_t lqi = status[1] & PacketStatusLqiMask;
  if (rssiDbm < Settings::IsmRx::MinPacketRssiDbm || lqi > Settings::IsmRx::MaxPacketLqi)
  {
    clearRadioBuffer();
    return false;
  }

  return true;
}

void IsmChat::appendRadioByte(uint8_t value)
{
  if (!isAcceptedRadioByte(value))
  {
    return;
  }

  if (_radioBufferLength == 0 && value != 27 && !isPrintableRadioByte(value))
  {
    return;
  }

  if (_radioBufferLength == 1 && _radioBuffer[0] == 27 && value != '[')
  {
    clearRadioBuffer();
    return;
  }

  if (_radioBufferLength >= RadioBufferLength - 1)
  {
    clearRadioBuffer();
    return;
  }

  _radioBuffer[_radioBufferLength++] = value;
  _radioBuffer[_radioBufferLength] = '\0';
  _radioBufferLastByteAt = millis();

  if (value != '\n')
  {
    return;
  }

  if (_radioBuffer[0] == 27)
  {
    if (_radioBufferLength >= 3 && _radioBuffer[1] == '[' && _radioBuffer[2] == '0')
    {
      readMessageFromRadioBuffer();
      clearRadioBuffer();
    }
    return;
  }

  readPlainMessageFromRadioBuffer();
  clearRadioBuffer();
}

void IsmChat::readPlainMessageFromRadioBuffer()
{
  _currentMessageLength = 0;
  for (uint16_t i = 0; i < _radioBufferLength && _currentMessageLength < MaxMessageLength; i++)
  {
    char c = (char)_radioBuffer[i];
    if (c == '\r' || c == '\n')
    {
      break;
    }
    if (c < 32 || c > 126)
    {
      continue;
    }
    _currentMessage[_currentMessageLength++] = c;
  }
  _currentMessage[_currentMessageLength] = '\0';
  finalizeCurrentMessage();
}

void IsmChat::clearRadioBuffer()
{
  _radioBufferLastByteAt = 0;
  _radioBufferLength = 0;
  _radioBuffer[0] = '\0';
}

void IsmChat::readMessageFromRadioBuffer()
{
  _currentMessageLength = 0;
  for (uint16_t i = 0; i < _radioBufferLength && _currentMessageLength < MaxMessageLength;)
  {
    uint8_t value = _radioBuffer[i];
    if (value == 27)
    {
      i = skipAnsiSequence(_radioBuffer, _radioBufferLength, i);
      continue;
    }

    char c = (char)value;
    if (c == '\r' || c == '\n')
    {
      break;
    }
    if (c < 32 || c > 126)
    {
      i++;
      continue;
    }
    _currentMessage[_currentMessageLength++] = c;
    i++;
  }
  _currentMessage[_currentMessageLength] = '\0';
  finalizeCurrentMessage();
}

void IsmChat::finalizeCurrentMessage()
{
  if (_currentMessageLength == 0)
  {
    return;
  }

  copyText(_latestMessage, sizeof(_latestMessage), _currentMessage);
  _hasMessage = true;

  _currentMessageLength = 0;
  _currentMessage[0] = '\0';
}
