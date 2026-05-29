// Duration.h — value object for a span of time in whole seconds.
// Pure C++; no Arduino. Unit-tested on the host.
#pragma once

#include <cstdint>

namespace domain {

class Duration {
public:
    constexpr Duration() = default;
    explicit constexpr Duration(int32_t seconds) : seconds_(seconds < 0 ? 0 : seconds) {}

    static constexpr Duration fromSeconds(int32_t s) { return Duration(s); }
    static constexpr Duration fromMinutes(int32_t m) { return Duration(m * 60); }

    constexpr int32_t seconds() const { return seconds_; }
    constexpr int32_t minutesPart() const { return (seconds_ / 60) % 60; }
    constexpr int32_t secondsPart() const { return seconds_ % 60; }
    constexpr int32_t hoursPart() const { return seconds_ / 3600; }
    constexpr int32_t totalMinutes() const { return seconds_ / 60; }
    constexpr bool isZero() const { return seconds_ == 0; }

    // Saturating arithmetic — never goes below zero.
    Duration operator+(const Duration& o) const { return Duration(seconds_ + o.seconds_); }
    Duration operator-(const Duration& o) const { return Duration(seconds_ - o.seconds_); }
    bool operator==(const Duration& o) const { return seconds_ == o.seconds_; }
    bool operator!=(const Duration& o) const { return seconds_ != o.seconds_; }
    bool operator<(const Duration& o) const { return seconds_ < o.seconds_; }
    bool operator>(const Duration& o) const { return seconds_ > o.seconds_; }
    bool operator<=(const Duration& o) const { return seconds_ <= o.seconds_; }
    bool operator>=(const Duration& o) const { return seconds_ >= o.seconds_; }

    // Clamp into [lo, hi] inclusive.
    Duration clamped(const Duration& lo, const Duration& hi) const {
        if (seconds_ < lo.seconds_) return lo;
        if (seconds_ > hi.seconds_) return hi;
        return *this;
    }

    // Formats into the caller's buffer (>= 9 bytes). Returns the buffer.
    // < 1 hour -> "MM:SS"; otherwise "H:MM:SS".
    const char* format(char* buf, int bufLen) const;

private:
    int32_t seconds_ = 0;
};

} // namespace domain
