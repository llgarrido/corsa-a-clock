#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

/**
 * Virtual LCD 16x2 (HD44780) style display with 5x8 pixel font.
 */
class Display
{

public:
  /**
   * Horizontal alignment options for text written to a full display row.
   */
  enum TextAlignment
  {
    /// Start text at the first column.
    Left,
    /// Center text within the available columns.
    Center,
    /// End text at the last occupied column.
    Right
  };

  /**
   * Constructor for Display class.
   * @param cs Chip select pin for the TFT display
   * @param dc Data/command pin for the TFT display
   * @param rst Reset pin for the TFT display
   * @param blPin Backlight control pin for the TFT display
   */
  Display(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t blPin);

  /**
   * Clears the internal buffer.
   */
  void clearBuffer();

  /**
   * Initializes the display. Must be called before any other methods.
   */
  byte begin();

  /**
   * Clears the display.
   */
  void clear();

  /**
   * Controls the backlight.
   * @param on true to turn on, false to turn off
   */
  void backlight(bool on);

  /**
   * Writes a string to the display at the specified column and row.
   * @param col Column index (0-based)
   * @param row Row index (0-based)
   * @param text Null-terminated string to write
   */
  void write(int16_t col, int16_t row, const char *text);

  /**
   * Writes a string to the display at the specified row using text alignment.
   * @param alignment Text alignment (Left, Center, Right)
   * @param row Row index (0-based)
   * @param text Null-terminated string to write
   */
  void write(TextAlignment alignment, int16_t row, const char *text);

private:
  /// Number of character rows in the virtual display
  static constexpr uint8_t ROWS = 5;

  /// Number of character columns in the virtual display
  static constexpr uint8_t COLS = 10;

  /// Physical TFT display object
  Adafruit_ST7735 _tft;

  /// Backlight control pin
  uint8_t _backlightPin;

  /// Buffer to track current characters on the display for efficient updates
  char _buffer[ROWS][COLS + 1];

  /**
   * Draws a single character at the specified pixel coordinates using the provided bitmap.
   * @param x Top-left x coordinate in pixels
   * @param y Top-left y coordinate in pixels
   * @param bitmap 8-byte array representing the 5x8 character bitmap
   * @param onColor Color to use for "on" pixels
   * @param offColor Color to use for "off" pixels
   */
  void drawChar(int16_t x, int16_t y, const uint8_t *bitmap, uint16_t onColor, uint16_t offColor);

  /**
   * Maps a character to its corresponding index in the charMap.
   * @params x Top-left x coordinate in pixels
   * @params y Top-left y coordinate in pixels
   * @params character Character to draw
   * @params onColor Color to use for "on" pixels
   * @params offColor Color to use for "off" pixels
   */
  void drawChar(int16_t x, int16_t y, char character, uint16_t onColor, uint16_t offColor);

  /**
   * Converts a character to its corresponding index in the charMap.
   * Supports digits (0-9), uppercase letters (A-Z), space, hyphen, equals, colon, asterisk, vertical bar, hash, dot,
   * and the '~' monochrome separator inspired by classic Opel merchandising.
   * @param character Character to convert
   * @return Index in charMap (0-44) or -1 if character is not supported
   */
  int charToIndex(char character);
};
