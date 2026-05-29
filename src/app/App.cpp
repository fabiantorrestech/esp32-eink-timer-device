#include "app/App.h"

namespace app {

using domain::InputCommand;
using domain::LedPolicy;
using domain::RefreshDecision;
using domain::RenderModel;
using domain::TimerPhase;
using hal::ButtonEvent;
using hal::IndicatorState;

void App::setup() {
    d_.display->begin();
    d_.clock->begin();
    d_.encoder->begin();
    d_.indicator->begin();
    d_.settings->begin();
    d_.ble->begin();
    for (auto& b : d_.buttons) {
        if (b.button) b.button->begin();
    }

    // Load persisted settings, writing back defaults on first boot / schema bump.
    domain::SettingsModel model;
    if (!d_.settings->load(model)) {
        d_.settings->save(model);
    }
    d_.sm->begin(model);

    // Remote control travels the same command path as the knob.
    d_.ble->setCommandSink(d_.ui);

    render(); // policy forces a full refresh on the first frame
}

void App::pollInputs() {
    // Encoder rotation: one Increment/Decrement per detent.
    const int detents = d_.encoder->poll();
    const InputCommand step =
        detents > 0 ? InputCommand::Increment : InputCommand::Decrement;
    for (int i = 0; i < (detents < 0 ? -detents : detents); ++i) {
        d_.ui->submit(step);
    }
    if (d_.encoder->takePress()) {
        d_.ui->submit(InputCommand::StartPause);
    }

    // Buttons: decode semantic events into their bound commands.
    for (auto& b : d_.buttons) {
        if (!b.button) continue;
        b.button->poll();
        switch (b.button->takeEvent()) {
            case ButtonEvent::Click: d_.ui->submit(b.onClick); break;
            case ButtonEvent::LongPress: d_.ui->submit(b.onLongPress); break;
            case ButtonEvent::DoubleClick:
            case ButtonEvent::None: break;
        }
    }
}

void App::tick() {
    d_.ui->submit(InputCommand::Tick);
}

void App::onFinished() {
    if (d_.sm->settings().ledPolicy == LedPolicy::BlinkOnFinish) {
        d_.indicator->set(IndicatorState::Blink);
    }
    d_.ble->notifyState(d_.sm->snapshot());
}

void App::loopStep() {
    pollInputs();

    if (d_.sm->takeJustFinished()) {
        onFinished();
    }

    // Reflect phase on the status LED (Finished blink is owned by onFinished()).
    const TimerPhase phase = d_.sm->phase();
    if (phase == TimerPhase::Running &&
        d_.sm->settings().ledPolicy == LedPolicy::OnWhileRunning) {
        d_.indicator->set(IndicatorState::On);
    } else if (phase != TimerPhase::Finished) {
        d_.indicator->set(IndicatorState::Off);
    }
    d_.indicator->tick();

    render();
}

void App::render() {
    const IndicatorState ledNow =
        (d_.sm->phase() == TimerPhase::Running &&
         d_.sm->settings().ledPolicy == LedPolicy::OnWhileRunning)
            ? IndicatorState::On
            : IndicatorState::Off;
    const bool ledOn = ledNow != IndicatorState::Off ||
                       d_.sm->phase() == TimerPhase::Finished;
    const bool connected = d_.ble->isConnected();

    const RenderModel model = d_.ui->buildModel(ledOn, connected);
    const RenderModel reference = haveLastModel_ ? lastModel_ : RenderModel{};

    const RefreshDecision decision = d_.refresh->decide(model, reference);
    switch (decision.kind) {
        case RefreshDecision::Kind::Full:
            d_.display->drawFull(model);
            d_.ble->notifyState(d_.sm->snapshot());
            break;
        case RefreshDecision::Kind::Partial:
            d_.display->drawPartial(model, decision.region);
            d_.ble->notifyState(d_.sm->snapshot());
            break;
        case RefreshDecision::Kind::None:
            break;
    }

    lastModel_ = model;
    haveLastModel_ = true;
}

} // namespace app
