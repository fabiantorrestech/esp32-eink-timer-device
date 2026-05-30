#pragma once
#include "app/ITimerState.h"

namespace app {

// Timer reached zero. Holds a high-contrast frame; any action returns to idle.
class FinishedState : public ITimerState {
public:
    domain::TimerPhase phase() const override { return domain::TimerPhase::Finished; }
    void onEnter(TimerStateMachine& sm) override;
    void onCommand(TimerStateMachine& sm, domain::InputCommand cmd) override;
};

} // namespace app
