#pragma once
#include "app/ITimerState.h"

namespace app {

// Frozen mid-count. StartPause resumes; Reset returns to idle.
class PausedState : public ITimerState {
public:
    domain::TimerPhase phase() const override { return domain::TimerPhase::Paused; }
    void onEnter(TimerStateMachine& sm) override;
    void onCommand(TimerStateMachine& sm, domain::InputCommand cmd) override;
};

} // namespace app
