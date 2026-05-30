// FakeClock.h — in-memory IClock for host tests (advanceable virtual time).
#pragma once

#include "hal/IClock.h"

namespace fakes {

class FakeClock : public hal::IClock {
public:
    void begin() override {}
    uint32_t now() const override { return epoch_; }
    void setTime(uint32_t epochSeconds) override { epoch_ = epochSeconds; }
    void setAlarm(domain::Duration fromNow) override {
        alarmAt_ = epoch_ + static_cast<uint32_t>(fromNow.seconds());
        alarmSet_ = true;
    }
    void clearAlarm() override { alarmSet_ = false; }
    bool alarmFired() const override { return alarmSet_ && epoch_ >= alarmAt_; }
    float temperatureC() const override { return temp_; }

    // Test helpers.
    void advance(uint32_t seconds) { epoch_ += seconds; }
    void setTemperature(float c) { temp_ = c; }

private:
    uint32_t epoch_ = 0;
    uint32_t alarmAt_ = 0;
    bool alarmSet_ = false;
    float temp_ = 25.0F;
};

} // namespace fakes
