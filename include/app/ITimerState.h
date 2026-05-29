// ITimerState.h — State pattern interface for the timer lifecycle.
// Pure C++; no Arduino.
#pragma once

#include "domain/InputCommand.h"
#include "domain/TimerSnapshot.h"

namespace app {

class TimerStateMachine; // owns the states and the shared Timer

class ITimerState {
public:
    virtual ~ITimerState() = default;

    virtual domain::TimerPhase phase() const = 0;

    // Called once when this state becomes current.
    virtual void onEnter(TimerStateMachine& sm) = 0;

    // Handle a user/remote command. May request a transition via `sm`.
    virtual void onCommand(TimerStateMachine& sm, domain::InputCommand cmd) = 0;

    // 1 Hz time advance. Default: ignore.
    virtual void onTick(TimerStateMachine& sm) { (void)sm; }
};

} // namespace app
