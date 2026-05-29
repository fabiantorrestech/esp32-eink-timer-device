// Esp32RotaryInput.h — IRotaryInput over ESP32Encoder (hardware PCNT).
// Adapter: turns raw quadrature counts into detents and edge-detects the push.
// esp32dev build only.
#pragma once

#include <ESP32Encoder.h>

#include "hal/IRotaryInput.h"

namespace drivers {

class Esp32RotaryInput : public hal::IRotaryInput {
public:
    void begin() override;
    int poll() override;
    bool takePress() override;

private:
    static constexpr int kCountsPerDetent = 4; // full-quad transitions per click

    ESP32Encoder encoder_;
    int64_t lastCount_ = 0;
    bool lastSwitchLow_ = false; // active-low (INPUT_PULLUP)
    bool pressPending_ = false;
};

} // namespace drivers
