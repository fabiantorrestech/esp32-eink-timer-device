// NvsSettingsStore.h — ISettingsStore over ESP32 NVS (Preferences).
// Repository implementation; esp32dev build only.
#pragma once

#include <Preferences.h>

#include "hal/ISettingsStore.h"

namespace drivers {

class NvsSettingsStore : public hal::ISettingsStore {
public:
    void begin() override {}
    bool load(domain::SettingsModel& out) override;
    void save(const domain::SettingsModel& in) override;

private:
    static constexpr const char* kNamespace = "vtimer";
    Preferences prefs_;
};

} // namespace drivers
