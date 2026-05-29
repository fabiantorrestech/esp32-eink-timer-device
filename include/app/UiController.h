// UiController.h — Command sink + RenderModel builder.
// Receives uniform InputCommands (from encoder, buttons, or BLE) and forwards
// them to the state machine, and turns a TimerSnapshot into a RenderModel.
// Pure C++; no Arduino.
#pragma once

#include "app/TimerStateMachine.h"
#include "domain/InputCommand.h"
#include "domain/RenderModel.h"

namespace app {

class UiController : public domain::ICommandSink {
public:
    explicit UiController(TimerStateMachine& sm) : sm_(sm) {}

    // ICommandSink — single entry point for every input source.
    void submit(domain::InputCommand cmd) override { sm_.dispatch(cmd); }

    // Build the frame to render from current state plus peripheral status.
    domain::RenderModel buildModel(bool ledOn, bool bleConnected) const {
        const domain::TimerSnapshot s = sm_.snapshot();
        domain::RenderModel m;
        m.phase = s.phase;
        m.display = s.display;
        m.fractionRemaining = s.fractionRemaining;
        m.ledOn = ledOn;
        m.bleConnected = bleConnected;
        return m;
    }

private:
    TimerStateMachine& sm_;
};

} // namespace app
