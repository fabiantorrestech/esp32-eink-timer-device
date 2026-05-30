// HybridRefreshPolicy.h — concrete IRefreshPolicy (Strategy).
// Pure C++; no Arduino — implemented in src/domain/ and unit-tested.
//
// Policy: skip when unchanged; force a full refresh on request or after a
// budget of partials (anti-ghosting); otherwise do a byte-aligned partial over
// just the fields that changed.
#pragma once

#include "domain/IRefreshPolicy.h"

namespace app {

class HybridRefreshPolicy : public domain::IRefreshPolicy {
public:
    // maxPartials: number of partial refreshes tolerated before a cleansing full.
    explicit HybridRefreshPolicy(int maxPartials = 12) : maxPartials_(maxPartials) {}

    domain::RefreshDecision decide(const domain::RenderModel& next,
                                   const domain::RenderModel& last) override;
    void requestFull() override { forceFull_ = true; }

    // Exposed for tests/inspection.
    int partialsSinceFull() const { return partialCount_; }

private:
    static domain::DirtyRegion changedRegion(const domain::RenderModel& next,
                                             const domain::RenderModel& last);

    int maxPartials_;
    int partialCount_ = 0;
    bool forceFull_ = true; // first frame is always a full refresh
};

} // namespace app
