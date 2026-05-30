# ESP32 E-Paper Visual Timer

[![CI](https://github.com/fabiantorrestech/esp32-eink-timer-device/actions/workflows/ci.yml/badge.svg)](https://github.com/fabiantorrestech/esp32-eink-timer-device/actions/workflows/ci.yml)

A physical **visual timer** (Time Timer–style shrinking arc/bar) built on an
ESP32-WROOM-32 driving a Waveshare 4.2" 400×300 e-paper display. It is the
embedded sibling of the *visualtimer* Android app.

This repository currently contains the **architecture and a compilable
skeleton**: clean HAL interfaces, a pure (host-testable) domain core, an
application state machine, stubbed hardware drivers, a BLE GATT stub, and unit
tests. It compiles for `esp32dev` and the domain logic is covered by native
Unity tests — ready to be fleshed out into full firmware.

> Full design rationale (SOLID layering, design patterns, e-paper refresh
> strategy, concurrency, power management, hardware gotchas) lives in
> [`ARCHITECTURE.md`](ARCHITECTURE.md).

## Hardware

| Component | Part | Notes |
|---|---|---|
| MCU | ESP32-WROOM-32 (USB-C, CP2102) | classic dual-core, FreeRTOS |
| Display | Waveshare 4.2" 400×300 B/W e-paper | SSD1683 (V2) — `GxEPD2_420_GDEY042T81` |
| RTC | DS3231 (I2C `0x68`) | temp-compensated, battery-backed, alarm/SQW |
| Input | rotary encoder (+push), push buttons | encoder via hardware PCNT |
| Indicator | status LED + (future) 5V WS2812 via SN74AHCT125N | level-shifted 3.3→5V |
| Power | USB-C now; LiPo + BMS later | deep-sleep friendly |

See [`include/Pins.h`](include/Pins.h) for the GPIO map and the strapping-pin /
RTC-wake / ADC1 constraints it encodes.

## Build & test

```bash
# Install PlatformIO (one time)
pip install platformio

# Compile-green gate for the hardware target
pio run -e esp32dev

# Run the pure-domain unit tests on the host (no hardware required)
pio test -e native

# Flash + monitor (board attached)
pio run -e esp32dev -t upload -t monitor
```

## Layout

```
include/hal/      HAL interfaces (IDisplay, IClock, IRotaryInput, ...)
include/domain/   pure C++ domain (Duration, Timer, RenderModel, IRefreshPolicy)
include/app/      App, TimerStateMachine + states, UiController, HybridRefreshPolicy
include/Pins.h    GPIO map (single source of truth)
src/domain/       domain implementations (Arduino-free, native-tested)
src/app/          application orchestration
src/drivers/      concrete HAL drivers (GxEPD2, DS3231, encoder, NimBLE, ...)
src/main.cpp      composition root (only file that names concrete drivers)
test/             Unity tests + in-memory HAL fakes
```

## Status

Skeleton stage. Domain logic (timer math, state transitions, refresh policy) is
implemented and tested; hardware drivers have minimal compiling bodies. The
connectivity layer (BLE companion app, Focusmate / Google Calendar) is
abstracted behind interfaces, with a basic BLE GATT stub in place.
