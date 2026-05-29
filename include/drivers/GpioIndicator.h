// GpioIndicator.h — IIndicator over a plain GPIO LED (non-blocking blink).
// esp32dev build only. (A future WS2812 strip via the SN74AHCT125N level
// shifter would be a separate IIndicator implementation.)
#pragma once

#include <cstdint>

#include "hal/IIndicator.h"

namespace drivers {

class GpioIndicator : public hal::IIndicator {
public:
    explicit GpioIndicator(uint8_t pin, bool activeHigh = true)
        : pin_(pin), activeHigh_(activeHigh) {}

    void begin() override;
    void set(hal::IndicatorState state) override;
    void tick() override;

private:
    void write(bool on);

    uint8_t pin_;
    bool activeHigh_;
    hal::IndicatorState state_ = hal::IndicatorState::Off;
    bool blinkOn_ = false;
    uint32_t lastToggleMs_ = 0;
};

} // namespace drivers
