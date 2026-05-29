// IButtonInput.h — single push-button contract.
// Drivers debounce and decode semantic events (OneButton on device).
#pragma once

#include "hal/HalTypes.h"

namespace hal {

class IButtonInput {
public:
    virtual ~IButtonInput() = default;

    virtual void begin() = 0;

    // Service the debounce/decode FSM; call frequently (e.g. every few ms).
    virtual void poll() = 0;

    // Consume the next decoded event, or ButtonEvent::None if there isn't one.
    virtual ButtonEvent takeEvent() = 0;
};

} // namespace hal
