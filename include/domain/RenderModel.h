// RenderModel.h — everything the display layer needs to draw one frame.
// Pure C++; no Arduino. The display facade turns this into pixels; it never
// reaches back into the timer/state machine.
#pragma once

#include "domain/Duration.h"
#include "domain/TimerSnapshot.h"

namespace domain {

struct RenderModel {
    TimerPhase phase = TimerPhase::Idle;
    Duration display{};             // big numeric readout
    float fractionRemaining = 0.0F; // arc/bar fill, [0,1]
    bool ledOn = false;             // mirror of indicator state, for on-screen hint
    bool bleConnected = false;      // companion-app link status icon

    // Equality drives dirty detection: identical models => nothing to redraw.
    bool operator==(const RenderModel& o) const {
        return phase == o.phase && display == o.display &&
               sameFraction(fractionRemaining, o.fractionRemaining) &&
               ledOn == o.ledOn && bleConnected == o.bleConnected;
    }
    bool operator!=(const RenderModel& o) const { return !(*this == o); }

private:
    // Compare fractions at the granularity the arc can actually show (1 degree),
    // so sub-pixel jitter doesn't trigger needless partial refreshes.
    static bool sameFraction(float a, float b) {
        const int da = static_cast<int>(a * 360.0F + 0.5F);
        const int db = static_cast<int>(b * 360.0F + 0.5F);
        return da == db;
    }
};

} // namespace domain
