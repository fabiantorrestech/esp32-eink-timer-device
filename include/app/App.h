// App.h — application orchestrator.
// Holds injected HAL interfaces (Dependency Inversion) and drives the
// poll -> dispatch -> render cycle. Constructed by the composition root in
// main.cpp; contains no concrete driver types.
#pragma once

#include <vector>

#include "app/TimerStateMachine.h"
#include "app/UiController.h"
#include "domain/IRefreshPolicy.h"
#include "domain/InputCommand.h"
#include "domain/RenderModel.h"
#include "hal/IBleService.h"
#include "hal/IButtonInput.h"
#include "hal/IClock.h"
#include "hal/IDisplay.h"
#include "hal/IIndicator.h"
#include "hal/IPower.h"
#include "hal/IRotaryInput.h"
#include "hal/ISettingsStore.h"

namespace app {

// Binds a physical button to the commands it emits (OCP: add a button = add a
// binding at the composition root, no App changes).
struct ButtonBinding {
    hal::IButtonInput* button = nullptr;
    domain::InputCommand onClick = domain::InputCommand::None;
    domain::InputCommand onLongPress = domain::InputCommand::None;
};

class App {
public:
    struct Deps {
        hal::IDisplay* display = nullptr;
        hal::IClock* clock = nullptr;
        hal::IRotaryInput* encoder = nullptr;
        hal::IIndicator* indicator = nullptr;
        hal::ISettingsStore* settings = nullptr;
        hal::IBleService* ble = nullptr;
        hal::IPower* power = nullptr;
        TimerStateMachine* sm = nullptr;
        UiController* ui = nullptr;
        domain::IRefreshPolicy* refresh = nullptr;
        std::vector<ButtonBinding> buttons{};
    };

    explicit App(const Deps& deps) : d_(deps) {}

    // One-time init: begin drivers, load settings, initial full render.
    void setup();

    // Poll inputs, dispatch commands, render if needed. Call frequently.
    void loopStep();

    // 1 Hz time advance (call from the timer task / scheduler).
    void tick();

private:
    void pollInputs();
    void render();
    void onFinished();

    Deps d_;
    domain::RenderModel lastModel_{};
    bool haveLastModel_ = false;
};

} // namespace app
