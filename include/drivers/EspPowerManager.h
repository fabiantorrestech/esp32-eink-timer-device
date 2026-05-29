// EspPowerManager.h — IPower over the ESP32 deep-sleep API.
// Owns every esp_sleep_* call. esp32dev build only.
#pragma once

#include "hal/IPower.h"

namespace drivers {

class EspPowerManager : public hal::IPower {
public:
    hal::WakeReason wakeReason() const override;
    void enterDeepSleep(const hal::WakeSources& sources) override;
};

} // namespace drivers
