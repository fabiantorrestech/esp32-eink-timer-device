// IRotaryInput.h — rotary encoder contract (hardware PCNT on device).
#pragma once

namespace hal {

class IRotaryInput {
public:
    virtual ~IRotaryInput() = default;

    virtual void begin() = 0;

    // Detents moved since the last call (+CW / -CCW). Resets the accumulator.
    virtual int poll() = 0;

    // Consume a pending push of the encoder shaft (edge-triggered).
    virtual bool takePress() = 0;
};

} // namespace hal
