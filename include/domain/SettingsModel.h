// SettingsModel.h — persisted user preferences.
// Pure C++; no Arduino. Persisted via ISettingsStore (NVS on device).
#pragma once

#include <cstdint>

#include "domain/Duration.h"
#include "domain/Timer.h"

namespace domain {

// LED behaviour policy when a timer finishes / runs.
enum class LedPolicy : uint8_t {
    Off = 0,
    OnWhileRunning,
    BlinkOnFinish,
};

struct SettingsModel {
    // Bump when the on-flash layout changes; load() migrates/falls back on mismatch.
    static constexpr uint16_t kSchemaVersion = 1;

    uint16_t schemaVersion = kSchemaVersion;
    Duration defaultDuration = Duration::fromMinutes(25); // pomodoro-ish default
    TimerMode mode = TimerMode::Countdown;
    LedPolicy ledPolicy = LedPolicy::BlinkOnFinish;
    bool soundEnabled = false;

    bool isCompatible() const { return schemaVersion == kSchemaVersion; }
};

} // namespace domain
