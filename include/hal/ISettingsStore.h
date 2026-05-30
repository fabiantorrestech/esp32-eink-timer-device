// ISettingsStore.h — persistence contract (Repository over NVS on device).
#pragma once

#include "domain/SettingsModel.h"

namespace hal {

class ISettingsStore {
public:
    virtual ~ISettingsStore() = default;

    virtual void begin() = 0;

    // Load into `out`. Returns false if nothing stored / schema mismatch, in
    // which case `out` is left at its defaults (caller may then save()).
    virtual bool load(domain::SettingsModel& out) = 0;
    virtual void save(const domain::SettingsModel& in) = 0;
};

} // namespace hal
