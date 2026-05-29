#include "app/TimerStateMachine.h"

namespace app {

using domain::Duration;
using domain::InputCommand;
using domain::TimerPhase;
using domain::TimerSnapshot;

TimerStateMachine::TimerStateMachine() {
    timer_.setTotal(settings_.defaultDuration);
    timer_.setMode(settings_.mode);
}

void TimerStateMachine::begin(const domain::SettingsModel& settings) {
    settings_ = settings;
    timer_.setMode(settings_.mode);
    timer_.setTotal(settings_.defaultDuration);
    timer_.reset();
    current_ = &idle_;
    current_->onEnter(*this);
}

ITimerState* TimerStateMachine::stateFor(TimerPhase phase) {
    switch (phase) {
        case TimerPhase::Idle: return &idle_;
        case TimerPhase::Setting: return &setting_;
        case TimerPhase::Running: return &running_;
        case TimerPhase::Paused: return &paused_;
        case TimerPhase::Finished: return &finished_;
    }
    return &idle_;
}

void TimerStateMachine::changeTo(TimerPhase phase) {
    current_ = stateFor(phase);
    current_->onEnter(*this);
}

void TimerStateMachine::dispatch(InputCommand cmd) {
    if (cmd == InputCommand::Tick) {
        current_->onTick(*this);
        return;
    }
    if (cmd == InputCommand::None) return;
    current_->onCommand(*this, cmd);
}

void TimerStateMachine::adjust(int detents) {
    timer_.adjustTotal(detents * kStepSeconds, kMinDuration, kMaxDuration);
}

TimerSnapshot TimerStateMachine::snapshot() const {
    TimerSnapshot s;
    s.phase = current_->phase();
    s.display = timer_.displayValue();
    s.total = timer_.total();
    s.fractionRemaining = timer_.fractionRemaining();
    return s;
}

bool TimerStateMachine::takeJustFinished() {
    const bool v = justFinished_;
    justFinished_ = false;
    return v;
}

} // namespace app
