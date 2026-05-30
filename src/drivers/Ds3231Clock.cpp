#include "drivers/Ds3231Clock.h"

#include <Wire.h>

#include "Pins.h"

namespace drivers {

void Ds3231Clock::begin() {
    Wire.begin(pins::kI2cSda, pins::kI2cScl);
    ok_ = rtc_.begin(&Wire);
    if (!ok_) return;

    // Use the INT/SQW pin for alarm interrupts (drives ext0 deep-sleep wake).
    rtc_.writeSqwPinMode(DS3231_OFF);
    rtc_.disableAlarm(2);
    rtc_.clearAlarm(1);
    pinMode(pins::kRtcInt, INPUT_PULLUP);
}

uint32_t Ds3231Clock::now() const {
    if (!ok_) return 0;
    return rtc_.now().unixtime();
}

void Ds3231Clock::setTime(uint32_t epochSeconds) {
    if (!ok_) return;
    // RTClib's DateTime(uint32_t) already takes a Unix epoch (it is the
    // counterpart to now().unixtime()), so pass the value straight through.
    rtc_.adjust(DateTime(epochSeconds));
}

void Ds3231Clock::setAlarm(domain::Duration fromNow) {
    if (!ok_) return;
    const DateTime target = rtc_.now() + TimeSpan(fromNow.seconds());
    rtc_.clearAlarm(1);
    rtc_.setAlarm1(target, DS3231_A1_Date); // match day/hour/minute/second
}

void Ds3231Clock::clearAlarm() {
    if (!ok_) return;
    rtc_.clearAlarm(1);
}

bool Ds3231Clock::alarmFired() const {
    if (!ok_) return false;
    return rtc_.alarmFired(1);
}

float Ds3231Clock::temperatureC() const {
    if (!ok_) return 0.0F;
    return rtc_.getTemperature();
}

} // namespace drivers
