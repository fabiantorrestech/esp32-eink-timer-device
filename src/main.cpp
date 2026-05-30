// main.cpp — composition root.
//
// This is the ONLY translation unit that names concrete driver types. It
// constructs the object graph, injects HAL interfaces into the application
// (Dependency Inversion), and runs the loop.
//
// Concurrency: the skeleton uses a single super-loop with a millis()-based 1 Hz
// tick for clarity and obvious correctness. The target topology (UiTask /
// RenderTask / TimerTask + FreeRTOS queues, app tasks pinned to core 1, BLE on
// core 0) is documented in ARCHITECTURE.md §5 and is the next step.
#include <Arduino.h>

#include <vector>

#include "Pins.h"
#include "app/App.h"
#include "app/HybridRefreshPolicy.h"
#include "app/TimerStateMachine.h"
#include "app/UiController.h"
#include "drivers/Ds3231Clock.h"
#include "drivers/EspPowerManager.h"
#include "drivers/Esp32RotaryInput.h"
#include "drivers/GpioIndicator.h"
#include "drivers/GxEpdDisplay.h"
#include "drivers/NimBleService.h"
#include "drivers/NvsSettingsStore.h"
#include "drivers/OneButtonInput.h"

namespace {

// --- concrete drivers (composition root owns their lifetime) ---------------
drivers::GxEpdDisplay g_display;
drivers::Ds3231Clock g_clock;
drivers::Esp32RotaryInput g_encoder;
drivers::GpioIndicator g_statusLed(pins::kStatusLed);
drivers::NvsSettingsStore g_settings;
drivers::NimBleService g_ble;
drivers::EspPowerManager g_power;

drivers::OneButtonInput g_startButton(pins::kButtonStart);
drivers::OneButtonInput g_menuButton(pins::kButtonMenu);
drivers::OneButtonInput g_backButton(pins::kButtonBack);

// --- application layer ------------------------------------------------------
app::TimerStateMachine g_sm;
app::UiController g_ui(g_sm);
app::HybridRefreshPolicy g_refresh;

app::App* g_app = nullptr;

constexpr uint32_t kTickPeriodMs = 1000; // 1 Hz time advance
constexpr uint32_t kPollDelayMs = 5;     // input poll cadence
uint32_t g_lastTickMs = 0;

} // namespace

void setup() {
    Serial.begin(115200);

    using domain::InputCommand;
    app::App::Deps deps;
    deps.display = &g_display;
    deps.clock = &g_clock;
    deps.encoder = &g_encoder;
    deps.indicator = &g_statusLed;
    deps.settings = &g_settings;
    deps.ble = &g_ble;
    deps.power = &g_power;
    deps.sm = &g_sm;
    deps.ui = &g_ui;
    deps.refresh = &g_refresh;
    deps.buttons = {
        {&g_startButton, InputCommand::StartPause, InputCommand::Reset},
        {&g_menuButton, InputCommand::EnterMenu, InputCommand::None},
        {&g_backButton, InputCommand::Back, InputCommand::Reset},
    };

    static app::App app(deps);
    g_app = &app;
    g_app->setup();

    g_lastTickMs = millis();
}

void loop() {
    g_app->loopStep();

    const uint32_t now = millis();
    if (now - g_lastTickMs >= kTickPeriodMs) {
        g_lastTickMs += kTickPeriodMs;
        g_app->tick();
    }

    delay(kPollDelayMs);
}
