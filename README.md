# Corsa A Clock

Digital clock firmware for the dashboard of the Opel/Vauxhall/Chevrolet Corsa A
facelift introduced in 1990. The project recreates an LCD-style display on an
ST7735 TFT and combines date, time, day of week, temperature, humidity, Wi-Fi
time synchronization, and experimental ISM-band receiver checks in a compact
format designed for the car interior.

The firmware is built for Arduino/ESP32 and is organized around small view and
hardware classes. `corsa-a-clock.ino` wires the dependencies together, registers
the views, initializes hardware, and keeps the main loop focused on HTTP
service, RF service, buttons, and the active view update.

## Features

- ST7735 TFT screen used as a 10-column by 5-row virtual LCD.
- DS3231 RTC clock with 1 Hz SQW signal for stable display refreshes.
- Date, 24-hour time, day of week, temperature, and humidity display.
- DHT11 internal sensor for cabin climate readings.
- SHT30/SHT3x I2C probe for exterior climate readings.
- Two physical navigation buttons: view switching and action/enter.
- Temporary Wi-Fi access point to synchronize date and time from a web page.
- Basic captive portal redirects to the synchronization panel.
- CC1101 ISM-band receiver test path using Flipper Zero Sub-GHz chat packets.
- Embedded web resources generated into `src/Resources.h`.

## Project Direction

The current `IsmChat` feature is not the final RF product goal. It exists as a
practical receiver validation tool: by receiving known Flipper Zero Sub-GHz chat
messages, the firmware can prove that the CC1101 module, SPI wiring, selected
frequency, packet filtering, and display notification path are working.

The intended future direction is a generic TPMS system that is easy to attach to
the tyres. The RF work in this repository is therefore a stepping stone toward
reliable tyre pressure and temperature reception, decoding, and display inside
the Corsa A clock form factor.

## Hardware

Main components:

- Arduino-compatible ESP32.
- ST7735 TFT display.
- DS3231 RTC.
- DHT11 internal temperature and humidity sensor.
- FS400-SHT3X / SHT30 / SHT3x I2C exterior temperature and humidity sensor.
- CC1101 RF receiver module matching the configured ISM/SRD band.
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
| CC1101 CS | 25 |
| CC1101 GDO0 | 34 |
| CC1101 GDO2 | 36 |
| CC1101 SCK | 18 |
| CC1101 MISO | 19 |
| CC1101 MOSI | 23 |

The exterior SHT30/SHT3x sensor uses the default ESP32 `Wire` bus pins shown
above and I2C address `0x44` by default. If your module is configured for
`0x45`, change `SHT30_I2C_ADDRESS` in `corsa-a-clock.ino`.

The CC1101 receiver is configured at `Settings::IsmRx::FrequencyMhz`, currently
`433.92 MHz`. Use a CC1101 variant that supports the selected band and adjust
the frequency for your module and local radio regulations.

## Startup And Runtime Flow

Startup is defined in `setup()`:

1. Initialize the display and view registries.
2. Show the splash view.
3. Initialize buttons, RTC, climate sensors, and the CC1101 receiver.
4. Collect startup errors with `StartupDiagnostics`.
5. Show non-zero startup errors on the splash view.
6. Enter the first carousel view.

The main loop stays intentionally small:

1. Service the HTTP server when Wi-Fi is running.
2. Service the ISM receiver.
3. Show the chat notification view when a receiver test message arrives.
4. Consume TAB presses to move through carousel views.
5. Call `currentView->update()`.

## Views And Controls

- **Main view**: date, time, day of week, exterior temperature, and exterior
  humidity.
- **Climate view**: internal and exterior temperature/humidity comparison.
- **Settings view**: press ENTER to start or stop the Wi-Fi access point.
- **Chat notification view**: displays the latest receiver test message.
- **Splash view**: startup screen and diagnostic error display.

TAB switches between the main, climate, and settings views. ENTER is used by the
active view. Leaving the settings view stops the Wi-Fi server if it is running.
Pressing ENTER on the chat notification view closes it and returns to the
current carousel view.

If the RTC reports lost power, the clock keeps the 1 Hz refresh signal active
but treats the date/time as invalid until it is synchronized. Invalid date/time
values are shown as placeholders such as `----------` and `--:--:--`.

## Wi-Fi Configuration

The device starts an access point only from the settings view. Public Wi-Fi
settings live in `src/Settings.h`:

| Setting | Value |
| --- | --- |
| SSID | `CORSA-A` |
| Local IP | `192.168.1.1` |
| Channel | `11` |
| Maximum clients | `4` |

The password is read from `src/Secrets.h` when that file exists. If it does not
exist, the firmware falls back to `src/Secrets.example.h` and uses the
placeholder password `change-me`.

To configure a device-specific password:

1. Copy `src/Secrets.example.h` to `src/Secrets.h`.
2. Change the value of `Secrets::WiFiAp::Password`.

`src/Secrets.h` is ignored by Git so local credentials do not need to be
committed.

After connecting to the device network, open:

```text
http://192.168.1.1/
```

The web page reads the client's local time from the browser and sends it to the
device to synchronize the RTC.

## ISM Receiver Test

`IsmChat` configures the CC1101 for a known Flipper Zero Sub-GHz chat preset so
the receiver can be tested with predictable packets:

- GFSK modulation.
- `9.99 kbps` bit rate.
- `19.04 kHz` frequency deviation.
- `102 kHz` receiver bandwidth.
- `433.92 MHz` default carrier frequency.
- Messages up to `64` characters.

The receiver checks packet CRC, RSSI, and LQI before accepting data:

- `Settings::IsmRx::MinPacketRssiDbm = -100.0`
- `Settings::IsmRx::MaxPacketLqi = 80`

This path is useful for hardware bring-up and RF confidence testing. It is
expected to evolve or be replaced as the project moves toward generic TPMS
message reception and decoding.

## Web Resource Workflow

The page served by the ESP32 lives in `static-files/`:

- `index.html`
- `index.js`
- `styles.css`
- `corsa.jpg`

The minified files are written to `static-files/min/`, and `src/Resources.h` is
generated from the web resources so they can be embedded in program memory.
Edit the source files in `static-files/`, then run the full VS Code task before
compiling:

- `Minify HTML, JS and CSS`

The minification tasks use `npx` packages (`html-minifier-terser`, `terser`, and
`clean-css-cli`), so Node.js/npm must be available. If minified files are
missing, the resource generator falls back to the source files in
`static-files/`.

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

The generator prefers minified resources from `static-files/min/` when they
exist. `src/Resources.h` is generated automatically; avoid editing it by hand.

## Arduino Dependencies

The project uses these libraries:

- `Adafruit_GFX`
- `Adafruit_ST7735`
- `RTClib`
- `SimpleDHT`
- `RadioLib`
- `WiFi`
- `WebServer`
- `DNSServer`

The last three are part of the ESP32 Arduino environment.

## Structure

```text
corsa-a-clock.ino          Main firmware and dependency composition
src/                       Domain classes, views, hardware, and HTTP server
src/ClimateSensors/        Internal and external climate sensors
src/Views/                 Display views
static-files/              Source web interface
static-files/min/          Minified web interface
tools/Generate-HtmlResources.ps1
src/Resources.h            Generated embedded web resources
```

## Development Notes

- The display is a 10-column by 5-row virtual LCD.
- `DataPipe` owns display text formatting for dates, time, temperature, and
  humidity.
- The settings view stops the Wi-Fi server when leaving the view.
- Button interrupts are kept minimal; debouncing is applied when the main loop
  consumes the pending press.
- The SHT30/SHT3x driver starts a measurement and reads it after a short delay
  to keep periodic reads light.
- `Display.cpp` defines the supported character map. Unsupported characters are
  ignored by the display.
- When adding a new view, register it in `corsa-a-clock.ino` and use computed
  array counts instead of hardcoded counts.
