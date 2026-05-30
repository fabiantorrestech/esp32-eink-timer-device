// FakeDisplay.h — in-memory IDisplay for host tests (records draw calls).
#pragma once

#include "domain/DirtyRegion.h"
#include "hal/IDisplay.h"

namespace fakes {

class FakeDisplay : public hal::IDisplay {
public:
    void begin() override { ++beginCount; }
    void drawFull(const domain::RenderModel& model) override {
        ++fullCount;
        lastModel = model;
    }
    void drawPartial(const domain::RenderModel& model,
                     const domain::DirtyRegion& region) override {
        ++partialCount;
        lastModel = model;
        lastRegion = region;
    }
    void hibernate() override { ++hibernateCount; }

    int beginCount = 0;
    int fullCount = 0;
    int partialCount = 0;
    int hibernateCount = 0;
    domain::RenderModel lastModel{};
    domain::DirtyRegion lastRegion{};
};

} // namespace fakes
