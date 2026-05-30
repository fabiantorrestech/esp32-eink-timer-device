#include "drivers/Esp32RotaryInput.h"

#include <Arduino.h>

#include "Pins.h"

namespace drivers {

void Esp32RotaryInput::begin() {
    // A/B are input-only GPIOs (34/35) with EXTERNAL pull-ups; don't rely on
    // internal weak pulls here.
    ESP32Encoder::useInternalWeakPullResistors = puType::none;
    encoder_.attachFullQuad(pins::kEncoderA, pins::kEncoderB);
    encoder_.clearCount();
    lastCount_ = 0;

    pinMode(pins::kEncoderSw, INPUT_PULLUP);
    lastSwitchLow_ = (digitalRead(pins::kEncoderSw) == LOW);
}

int Esp32RotaryInput::poll() {
    // Edge-detect the push button (active-low) and latch it for takePress().
    const bool low = (digitalRead(pins::kEncoderSw) == LOW);
    if (low && !lastSwitchLow_) {
        pressPending_ = true;
    }
    lastSwitchLow_ = low;

    // Convert raw quadrature delta into whole detents, keeping the remainder.
    const int64_t count = encoder_.getCount();
    const int64_t rawDelta = count - lastCount_;
    const int detents = static_cast<int>(rawDelta / kCountsPerDetent);
    lastCount_ += static_cast<int64_t>(detents) * kCountsPerDetent;
    return detents;
}

bool Esp32RotaryInput::takePress() {
    const bool p = pressPending_;
    pressPending_ = false;
    return p;
}

} // namespace drivers
