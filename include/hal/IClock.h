// IClock.h — wall-clock + alarm contract (DS3231 on device).
#pragma once

#include <cstdint>

#include "domain/Duration.h"

namespace hal {

class IClock {
public:
    virtual ~IClock() = default;

    virtual void begin() = 0;

    // Current time as Unix epoch seconds.
    virtual uint32_t now() const = 0;
    virtual void setTime(uint32_t epochSeconds) = 0;

    // Schedule the alarm to fire `fromNow` ahead (drives ext0 deep-sleep wake).
    virtual void setAlarm(domain::Duration fromNow) = 0;
    virtual void clearAlarm() = 0;
    virtual bool alarmFired() const = 0;

    // On-die temperature in Celsius (DS3231 exposes this for free).
    virtual float temperatureC() const = 0;
};

} // namespace hal
