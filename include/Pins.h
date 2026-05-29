// Pins.h — single source of truth for the GPIO map.
//
// This header encodes the practical ESP32-WROOM-32 constraints (see
// ARCHITECTURE.md §9):
//   * Buttons avoid the boot-strapping pins GPIO0/2/12/15.
//   * Deep-sleep wake pins are RTC-capable (0,2,4,12-15,25-27,32-39).
//   * GPIO34-39 are input-only and need EXTERNAL pull-ups.
//   * An ADC1 pin is reserved for future battery sensing (ADC2 dies with WiFi).
//
// Only the hardware drivers (and the composition root) include this file; the
// pure domain never does.
#pragma once

#include <cstdint>

namespace pins {

// --- SPI e-paper (Waveshare 4.2", SSD1683) --------------------------------
// Uses VSPI default pins for SCK/MOSI; control lines on dedicated GPIOs.
inline constexpr uint8_t kEpdSck = 18;
inline constexpr uint8_t kEpdMosi = 23;
inline constexpr uint8_t kEpdCs = 5;
inline constexpr uint8_t kEpdDc = 17;
inline constexpr uint8_t kEpdRst = 16;
inline constexpr uint8_t kEpdBusy = 4; // RTC-capable; can also wake on refresh-done if needed

// --- I2C (DS3231 RTC) -----------------------------------------------------
inline constexpr uint8_t kI2cSda = 21;
inline constexpr uint8_t kI2cScl = 22;
// DS3231 SQW/INT -> ext0 deep-sleep wake. Active-low, open-drain: needs pull-up.
inline constexpr uint8_t kRtcInt = 27; // RTC-capable

// --- Rotary encoder (hardware PCNT) ---------------------------------------
// Input-only pins: provide EXTERNAL pull-ups + small debounce caps.
inline constexpr uint8_t kEncoderA = 34;
inline constexpr uint8_t kEncoderB = 35;
inline constexpr uint8_t kEncoderSw = 32; // RTC-capable -> can be an ext1 wake source

// --- Push buttons ---------------------------------------------------------
// RTC-capable so they can wake from deep sleep via ext1. Use INPUT_PULLUP.
inline constexpr uint8_t kButtonStart = 33;
inline constexpr uint8_t kButtonMenu = 25;
inline constexpr uint8_t kButtonBack = 26;

// --- Indicators -----------------------------------------------------------
inline constexpr uint8_t kStatusLed = 2;   // on-board LED; series resistor on board
inline constexpr uint8_t kLedStripData = 13; // -> SN74AHCT125N -> 5V WS2812 (future)

// --- Reserved (future) ----------------------------------------------------
// Battery sense MUST be on ADC1 (GPIO32-39) so it keeps working with WiFi on.
inline constexpr uint8_t kBatterySenseAdc1 = 39;

// Bitmask of RTC GPIOs allowed to wake the device via ext1 (buttons + encoder push).
inline constexpr uint64_t kExt1WakeMask =
    (1ULL << kEncoderSw) | (1ULL << kButtonStart) |
    (1ULL << kButtonMenu) | (1ULL << kButtonBack);

} // namespace pins
