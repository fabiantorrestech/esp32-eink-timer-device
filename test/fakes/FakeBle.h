// FakeBle.h — in-memory IBleService for host tests.
#pragma once

#include "hal/IBleService.h"

namespace fakes {

class FakeBle : public hal::IBleService {
public:
    void begin() override {}
    void notifyState(const domain::TimerSnapshot& snapshot) override {
        ++notifyCount;
        lastSnapshot = snapshot;
    }
    void setCommandSink(domain::ICommandSink* sink) override { sink_ = sink; }
    bool isConnected() const override { return connected; }

    // Simulate a remote write from the companion app.
    void inject(domain::InputCommand cmd) {
        if (sink_) sink_->submit(cmd);
    }

    bool connected = false;
    int notifyCount = 0;
    domain::TimerSnapshot lastSnapshot{};

private:
    domain::ICommandSink* sink_ = nullptr;
};

} // namespace fakes
