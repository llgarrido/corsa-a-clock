# Corsa A Clock

Digital clock for the dashboard of the Opel/Vauxhall/Chevrolet Corsa A facelift introduced in 1990. The project recreates an LCD-style display on an ST7735 TFT and combines date, time, day of week, temperature, and humidity in a compact format designed for the car interior.

The firmware is built for Arduino/ESP32 and organizes the interface into views: a main screen, a climate comparison screen, a Wi-Fi settings screen, and a splash/error screen.

## Features

- DS3231 RTC clock with 1 Hz SQW signal for stable refreshes.
- Date, time, day of week, temperature, and humidity display.
- DHT11 internal sensor for cabin climate readings.
- SHT30/SHT3x I2C probe for exterior climate readings.
- ST7735 TFT screen used as a 10-column by 5-row virtual LCD.
- Two physical navigation buttons: view switching and action/enter.
- Temporary Wi-Fi access point to synchronize date and time from a web page.
- Basic captive portal to redirect automatically to the synchronization panel.
- Embedded web resources in `src/Resources.h`.

## Hardware

Main components:

- Arduino-compatible ESP32.
- ST7735 TFT display.
- DS3231 RTC.
- DHT11 internal sensor.
- FS400-SHT3X / SHT30 I2C exterior temperature and humidity sensor.
- 2 navigation push buttons.

Pins used by the firmware:

| Function | Pin |
| --- | ---: |
| TFT CS | 5 |
| TFT DC | 16 |
| TFT RST | 17 |
| TFT backlight | 32 |
| RTC SQW | 33 |
| DHT11 internal data | 26 |
| I2C SDA | 21 |
| I2C SCL | 22 |
| TAB button | 14 |
| ENTER button | 27 |

The exterior SHT30/SHT3x sensor uses I2C address `0x44` by default. If your module is configured for `0x45`, change `SHT30_I2C_ADDRESS` in `corsa-a-clock.ino`.

## Wi-Fi Configuration

The device starts an access point only from the settings view. Public Wi-Fi settings live in `src/Settings.h`:

- SSID: `CORSA-A`
- Local IP: `192.168.1.1`
- Channel: `11`

The password is read from `src/Secrets.h` when that file exists. If it does not exist, the firmware falls back to `src/Secrets.example.h` and uses the placeholder password `change-me`.

To configure a device-specific password:

1. Copy `src/Secrets.example.h` to `src/Secrets.h`.
2. Change the value of `Secrets::WiFiAp::Password`.

`src/Secrets.h` is ignored by Git so local credentials do not need to be committed.

## Usage

1. On startup, the display, buttons, RTC, and DHT11 sensor are initialized.
2. If clock or sensor errors are detected, they are shown on the screen.
3. The main view shows date, time, day, temperature, and humidity.
4. The climate view shows internal and exterior temperature/humidity readings.
5. The TAB button switches between the main view, climate view, and settings view.
6. In the settings view, ENTER starts or stops the Wi-Fi access point.
7. After connecting to the device network, open `http://192.168.1.1/` to synchronize the time.

If the RTC reports lost power, the clock keeps the 1 Hz refresh signal active but treats the date/time as invalid until it is synchronized.

## Web Resource Workflow

The page served by the ESP32 lives in `static-files/`:

- `index.html`
- `index.js`
- `styles.css`
- `corsa.jpg`

The minified files are written to `static-files/min/`, and `src/Resources.h` is generated from the web resources so they can be embedded in program memory. Edit the source files in `static-files/`, then run the full VS Code task before compiling:

- `Minify HTML, JS and CSS`

That task runs these steps in order:

1. `Minify HTML`
2. `Minify JavaScript`
3. `Minify CSS`
4. `Generate Resources.h`

To generate only `src/Resources.h`, run the VS Code task:

- `Generate Resources.h`

Or run the generator directly:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/Generate-HtmlResources.ps1
```

The generator prefers minified resources from `static-files/min/` when they exist. `src/Resources.h` is generated automatically; avoid editing it by hand.

## Arduino Dependencies

The project uses these libraries:

- `Adafruit_GFX`
- `Adafruit_ST7735`
- `RTClib`
- `SimpleDHT`
- `WiFi`
- `WebServer`
- `DNSServer`

The last three are part of the ESP32 Arduino environment.

## Structure

```text
corsa-a-clock.ino          Main firmware
src/                       Domain classes, views, hardware, and HTTP server
src/Views/                 Display views
static-files/              Source web interface
static-files/min/          Minified web interface
tools/Generate-HtmlResources.ps1
src/Resources.h            Generated embedded web resources
```

## Development Notes

- The settings view stops the Wi-Fi server when leaving the view.
- The synchronization endpoint accepts years from 2024 to 2099.
- Button interrupts are kept minimal; debouncing is applied when the main loop consumes the pending press.
