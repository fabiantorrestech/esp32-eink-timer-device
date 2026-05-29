#pragma once
#include "app/ITimerState.h"

namespace app {

// Counting. Ticks advance the timer; finishing transitions to Finished.
class RunningState : public ITimerState {
public:
    domain::TimerPhase phase() const override { return domain::TimerPhase::Running; }
    void onEnter(TimerStateMachine& sm) override;
    void onCommand(TimerStateMachine& sm, domain::InputCommand cmd) override;
    void onTick(TimerStateMachine& sm) override;
};

} // namespace app
