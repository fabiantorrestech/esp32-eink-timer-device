#pragma once
#include "app/ITimerState.h"

namespace app {

// Explicit settings/duration-editing screen.
class SettingState : public ITimerState {
public:
    domain::TimerPhase phase() const override { return domain::TimerPhase::Setting; }
    void onEnter(TimerStateMachine& sm) override;
    void onCommand(TimerStateMachine& sm, domain::InputCommand cmd) override;
};

} // namespace app
