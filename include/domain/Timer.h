// Timer.h — the visual-timer time accounting core.
// Pure C++; no Arduino. The running/paused lifecycle lives in the state
// machine; this class only owns the time math (configure, tick, query).
#pragma once

#include "domain/Duration.h"

namespace domain {

enum class TimerMode : uint8_t {
    Countdown = 0, // classic visual timer: disk shrinks as time elapses
    CountUp,       // counts elapsed up to an optional target
};

class Timer {
public:
    Timer() = default;
    explicit Timer(Duration total, TimerMode mode = TimerMode::Countdown)
        : total_(total), mode_(mode) {}

    void setTotal(Duration total) { total_ = total; }
    void setMode(TimerMode mode) { mode_ = mode; }
    void reset() { elapsed_ = Duration(0); }

    // Advance by one tick (one second). No-ops once finished.
    void tick() {
        if (isFinished()) return;
        elapsed_ = elapsed_ + Duration(1);
        if (mode_ == TimerMode::Countdown && elapsed_ > total_) {
            elapsed_ = total_;
        }
    }

    // Add/subtract from the configured total while idle (encoder adjustments).
    // Duration's constructor floors negatives at 0; then we clamp into range.
    void adjustTotal(int32_t deltaSeconds, Duration lo, Duration hi) {
        total_ = Duration(total_.seconds() + deltaSeconds).clamped(lo, hi);
    }

    Duration total() const { return total_; }
    TimerMode mode() const { return mode_; }
    Duration elapsed() const { return elapsed_; }

    Duration remaining() const {
        return (total_ > elapsed_) ? (total_ - elapsed_) : Duration(0);
    }

    // The value the user reads: countdown shows remaining, count-up shows elapsed.
    Duration displayValue() const {
        return mode_ == TimerMode::Countdown ? remaining() : elapsed_;
    }

    bool isFinished() const {
        if (total_.isZero()) return false;
        return elapsed_ >= total_;
    }

    // Fraction of the visual that should still be filled, in [0.0, 1.0].
    // For both modes the disk is "full" at start and empty when finished.
    float fractionRemaining() const {
        if (total_.isZero()) return 0.0F;
        const float rem = static_cast<float>(remaining().seconds());
        return rem / static_cast<float>(total_.seconds());
    }

private:
    Duration total_{};
    Duration elapsed_{};
    TimerMode mode_ = TimerMode::Countdown;
};

} // namespace domain
