// TimerSnapshot.h — immutable view of timer state for rendering and BLE.
// Pure C++; no Arduino.
#pragma once

#include "domain/Duration.h"

namespace domain {

// High-level phase, decoupled from the concrete state classes so consumers
// (display, BLE) don't depend on the application layer.
enum class TimerPhase : uint8_t {
    Idle = 0,
    Setting,
    Running,
    Paused,
    Finished,
};

struct TimerSnapshot {
    TimerPhase phase = TimerPhase::Idle;
    Duration display{};        // value the user reads (remaining or elapsed)
    Duration total{};          // configured duration
    float fractionRemaining = 0.0F; // [0,1] for the visual arc/bar
};

} // namespace domain
