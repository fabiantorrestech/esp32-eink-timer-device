// FakeSettingsStore.h — in-memory ISettingsStore for host tests.
#pragma once

#include "hal/ISettingsStore.h"

namespace fakes {

class FakeSettingsStore : public hal::ISettingsStore {
public:
    void begin() override {}
    bool load(domain::SettingsModel& out) override {
        if (!hasData_) return false;
        out = stored_;
        return out.isCompatible();
    }
    void save(const domain::SettingsModel& in) override {
        stored_ = in;
        hasData_ = true;
        ++saveCount;
    }

    // Test helpers.
    void seed(const domain::SettingsModel& m) {
        stored_ = m;
        hasData_ = true;
    }
    void clear() { hasData_ = false; }

    int saveCount = 0;

private:
    domain::SettingsModel stored_{};
    bool hasData_ = false;
};

} // namespace fakes
