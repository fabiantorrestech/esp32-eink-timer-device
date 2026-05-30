#pragma once
#include "app/ITimerState.h"

namespace app {

// Showing the configured duration, waiting to start. Encoder pre-sets duration.
class IdleState : public ITimerState {
public:
    domain::TimerPhase phase() const override { return domain::TimerPhase::Idle; }
    void onEnter(TimerStateMachine& sm) override;
    void onCommand(TimerStateMachine& sm, domain::InputCommand cmd) override;
};

} // namespace app
