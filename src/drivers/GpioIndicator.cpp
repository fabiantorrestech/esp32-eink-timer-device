#include "drivers/GpioIndicator.h"

#include <Arduino.h>

namespace drivers {

namespace {
constexpr uint32_t kBlinkPeriodMs = 500;
}

void GpioIndicator::begin() {
    pinMode(pin_, OUTPUT);
    write(false);
}

void GpioIndicator::write(bool on) {
    digitalWrite(pin_, (on == activeHigh_) ? HIGH : LOW);
}

void GpioIndicator::set(hal::IndicatorState state) {
    if (state_ == state) return;
    state_ = state;
    switch (state_) {
        case hal::IndicatorState::Off: write(false); break;
        case hal::IndicatorState::On: write(true); break;
        case hal::IndicatorState::Blink:
            blinkOn_ = true;
            write(true);
            lastToggleMs_ = millis();
            break;
    }
}

void GpioIndicator::tick() {
    if (state_ != hal::IndicatorState::Blink) return;
    const uint32_t now = millis();
    if (now - lastToggleMs_ >= kBlinkPeriodMs) {
        blinkOn_ = !blinkOn_;
        write(blinkOn_);
        lastToggleMs_ = now;
    }
}

} // namespace drivers
