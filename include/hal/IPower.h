// IPower.h — deep-sleep / wake contract.
// Owns all esp_sleep_* calls on device; the app just requests sleep and reads
// why it woke.
#pragma once

#include "hal/HalTypes.h"

namespace hal {

class IPower {
public:
    virtual ~IPower() = default;

    // Read why we woke (call early in setup()).
    virtual WakeReason wakeReason() const = 0;

    // Arm the given wake sources and enter deep sleep. Does not return.
    virtual void enterDeepSleep(const WakeSources& sources) = 0;
};

} // namespace hal
