// States.cpp — implementations of the five timer states (State pattern).
// Separated from the headers so they can call into the full TimerStateMachine
// definition without a circular include.
#include "app/TimerStateMachine.h"

namespace app {

using domain::InputCommand;
using domain::TimerPhase;

// ---------------------------------------------------------------- Idle -----
void IdleState::onEnter(TimerStateMachine& sm) {
    // Reload the configured duration and clear any prior progress.
    sm.timer().setTotal(sm.settings().defaultDuration);
    sm.timer().reset();
}

void IdleState::onCommand(TimerStateMachine& sm, InputCommand cmd) {
    switch (cmd) {
        case InputCommand::StartPause: sm.changeTo(TimerPhase::Running); break;
        case InputCommand::EnterMenu: sm.changeTo(TimerPhase::Setting); break;
        case InputCommand::Increment: sm.adjust(+1); break;
        case InputCommand::Decrement: sm.adjust(-1); break;
        default: break; // Reset/Back/Tick: no-op while idle
    }
}

// ------------------------------------------------------------- Setting -----
void SettingState::onEnter(TimerStateMachine& sm) { (void)sm; }

void SettingState::onCommand(TimerStateMachine& sm, InputCommand cmd) {
    switch (cmd) {
        case InputCommand::Increment: sm.adjust(+1); break;
        case InputCommand::Decrement: sm.adjust(-1); break;
        case InputCommand::StartPause: sm.changeTo(TimerPhase::Running); break;
        case InputCommand::Back:
        case InputCommand::Reset: sm.changeTo(TimerPhase::Idle); break;
        default: break;
    }
}

// ------------------------------------------------------------- Running -----
void RunningState::onEnter(TimerStateMachine& sm) {
    // Fresh start (not a resume from pause) begins from a clean elapsed.
    if (sm.timer().elapsed().isZero() || sm.timer().isFinished()) {
        sm.timer().reset();
    }
}

void RunningState::onCommand(TimerStateMachine& sm, InputCommand cmd) {
    switch (cmd) {
        case InputCommand::StartPause: sm.changeTo(TimerPhase::Paused); break;
        case InputCommand::Reset:
        case InputCommand::Back: sm.changeTo(TimerPhase::Idle); break;
        default: break; // ignore duration edits while running
    }
}

void RunningState::onTick(TimerStateMachine& sm) {
    sm.timer().tick();
    if (sm.timer().isFinished()) {
        sm.markFinished();
        sm.changeTo(TimerPhase::Finished);
    }
}

// -------------------------------------------------------------- Paused -----
void PausedState::onEnter(TimerStateMachine& sm) { (void)sm; }

void PausedState::onCommand(TimerStateMachine& sm, InputCommand cmd) {
    switch (cmd) {
        case InputCommand::StartPause: sm.changeTo(TimerPhase::Running); break;
        case InputCommand::Reset:
        case InputCommand::Back: sm.changeTo(TimerPhase::Idle); break;
        default: break;
    }
}

// ------------------------------------------------------------ Finished -----
void FinishedState::onEnter(TimerStateMachine& sm) { (void)sm; }

void FinishedState::onCommand(TimerStateMachine& sm, InputCommand cmd) {
    // Any acknowledgement returns to idle.
    switch (cmd) {
        case InputCommand::StartPause:
        case InputCommand::Reset:
        case InputCommand::Back:
        case InputCommand::EnterMenu: sm.changeTo(TimerPhase::Idle); break;
        default: break;
    }
}

} // namespace app
