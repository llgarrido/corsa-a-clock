# Agent Guide

This project is an Arduino/ESP32 firmware for a Corsa A dashboard clock. Keep
changes small, hardware-aware, and consistent with the existing class-based
layout.

## Architecture

- `corsa-a-clock.ino` is the composition root. It defines hardware pins,
  creates global dependencies, registers views, initializes components, and
  runs the main loop.
- `src/View.*` defines the common view lifecycle. `ViewLocator` owns view
  switching and calls `initialize()` / `destroy()` when the active view changes.
- `src/Views/` contains concrete UI screens:
- `Display` is a 10-column by 5-row virtual LCD over an ST7735 TFT. Prefer
  `display().write(col, row, text)` when placing fixed-width values.
- `DataPipe` owns display text formatting for dates, time, temperature, and
  humidity. Use it instead of duplicating formatting in views.
- `Settings` is the central constants container. Secrets come from
  `src/Secrets.h` when present, otherwise `src/Secrets.example.h`.
- `src/Resources.h` is generated from `static-files/`; do not edit it by hand.

## Firmware Flow

Startup order lives in `setup()`:

1. Initialize display and view registries.
2. Show the splash view.
3. Initialize buttons and hardware services.
4. Collect startup errors with `InitializationResult`.
5. Show non-zero startup errors on the splash view.
6. Enter the first carousel view.

The `loop()` should stay simple: service the HTTP server, consume navigation
button presses, and call `currentView->update()`.

## View Responsibilities

- Views decide when to redraw their own display content.
- Hardware classes decide when to read or update hardware state.
- Avoid building whole display rows when fixed coordinates are clearer.
- Keep labels short enough for the 10-column display.
- If a view needs periodic refresh, use the same rollover-safe pattern:

```cpp
unsigned long now = millis();
if (now - _lastRefresh < REFRESH_INTERVAL)
  return;
```

- Use `Clock::tick()` only when the view is actually driven by the clock tick,
  because calling it consumes the flag.

## Sensor Rules

- `begin()` may block briefly to perform an initial hardware read.
- Periodic reads after initialization should avoid blocking the main loop when
  practical.
- Use rollover-safe `millis()` comparisons for intervals.

## Display Rules

- The virtual display is 10 columns by 5 rows.
- The supported font lives in `Display.cpp`; update `charMap`,
- `Display::write(TextAlignment, row, text)` rewrites a full logical row.
  `Display::write(col, row, text)` is better for fixed fields.
- Unsupported characters are ignored by the display, so keep text within the
  known character set.

## Web Resource Workflow

- Edit browser UI sources in `static-files/`.
- Run the VS Code task `Minify HTML, JS and CSS` before committing generated
  resource updates.
- The task uses `npx` packages and then runs
  `tools/Generate-HtmlResources.ps1`.
- `src/Resources.h` is generated; never hand-edit it.

## Code Style

- Use `#pragma once` in headers.
- Include `Arduino.h` when using Arduino types such as `byte` or `uint8_t`.
- Prefer `static constexpr` for file-local constants.
- Keep object dependencies injected by reference when classes do not own them.
- Avoid dynamic allocation.
- Keep interrupt handlers tiny. Set flags in ISRs and consume them in normal
  code.
- Use `snprintf` with caller-owned buffers for formatted text.
- Keep comments useful and concise. Public classes and public methods should
  have short Doxygen-style comments when the surrounding file already follows
  that style.

## Git And Generated Files

- `src/Secrets.h` is local-only and ignored by Git.
- Treat `src/Resources.h` as generated output tied to `static-files/`.
- When adding a new view, register it in `corsa-a-clock.ino` and use computed
  array counts instead of hardcoded counts.
