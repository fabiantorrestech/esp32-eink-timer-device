// IIndicator.h — status indicator contract (LED on device; later WS2812).
#pragma once

#include "hal/HalTypes.h"

namespace hal {

class IIndicator {
public:
    virtual ~IIndicator() = default;

    virtual void begin() = 0;
    virtual void set(IndicatorState state) = 0;

    // Service any non-blocking blink animation; call from the loop/task.
    virtual void tick() = 0;
};

} // namespace hal
