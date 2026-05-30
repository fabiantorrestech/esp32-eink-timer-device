// TimerStateMachine.h — owns the Timer and drives the State objects.
// Pure C++; no Arduino. Unit-tested on the host.
#pragma once

#include "app/ITimerState.h"
#include "app/states/FinishedState.h"
#include "app/states/IdleState.h"
#include "app/states/PausedState.h"
#include "app/states/RunningState.h"
#include "app/states/SettingState.h"
#include "domain/SettingsModel.h"
#include "domain/Timer.h"
#include "domain/TimerSnapshot.h"

namespace app {

class TimerStateMachine {
public:
    // Duration editing limits and per-detent step.
    static constexpr domain::Duration kMinDuration = domain::Duration(60);        // 1 min
    static constexpr domain::Duration kMaxDuration = domain::Duration(4 * 3600);  // 4 h
    static constexpr int32_t kStepSeconds = 60;

    TimerStateMachine();

    // Initialise from persisted settings (sets default duration & mode).
    void begin(const domain::SettingsModel& settings);

    // Route a command to the current state (Tick is dispatched to onTick).
    void dispatch(domain::InputCommand cmd);

    // --- queries ---
    domain::TimerPhase phase() const { return current_->phase(); }
    domain::TimerSnapshot snapshot() const;

    // True for exactly one dispatch after the timer transitions into Finished,
    // so the App can fire the alarm/LED once. Cleared by takeJustFinished().
    bool takeJustFinished();

    // --- used by state objects ---
    domain::Timer& timer() { return timer_; }
    const domain::SettingsModel& settings() const { return settings_; }
    void changeTo(domain::TimerPhase phase);
    void adjust(int detents); // +/- detents * step, clamped to [min,max]
    void markFinished() { justFinished_ = true; }

private:
    ITimerState* stateFor(domain::TimerPhase phase);

    domain::Timer timer_{};
    domain::SettingsModel settings_{};

    IdleState idle_{};
    SettingState setting_{};
    RunningState running_{};
    PausedState paused_{};
    FinishedState finished_{};

    ITimerState* current_ = &idle_;
    bool justFinished_ = false;
};

} // namespace app
