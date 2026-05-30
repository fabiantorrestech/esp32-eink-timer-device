// Ds3231Clock.h — IClock over the DS3231 RTC (RTClib). esp32dev build only.
#pragma once

#include <RTClib.h>

#include "hal/IClock.h"

namespace drivers {

class Ds3231Clock : public hal::IClock {
public:
    void begin() override;
    uint32_t now() const override;
    void setTime(uint32_t epochSeconds) override;
    void setAlarm(domain::Duration fromNow) override;
    void clearAlarm() override;
    bool alarmFired() const override;
    float temperatureC() const override;

private:
    mutable RTC_DS3231 rtc_;
    bool ok_ = false;
};

} // namespace drivers
