// IRefreshPolicy.h — Strategy interface for the e-paper refresh decision.
// Pure C++; no Arduino. Concrete policy (HybridRefreshPolicy) is unit-tested.
#pragma once

#include "domain/DirtyRegion.h"
#include "domain/RenderModel.h"

namespace domain {

struct RefreshDecision {
    enum class Kind : uint8_t {
        None = 0, // nothing changed; skip the draw entirely
        Partial,  // fast partial-window refresh over `region`
        Full,     // slow full refresh (clears ghosting); region ignored
    };
    Kind kind = Kind::None;
    DirtyRegion region{};
};

class IRefreshPolicy {
public:
    virtual ~IRefreshPolicy() = default;

    // Decide how to render `next` given the previously rendered `last`.
    virtual RefreshDecision decide(const RenderModel& next, const RenderModel& last) = 0;

    // Force the next decide() to return Full (e.g. on state entry / wake).
    virtual void requestFull() = 0;
};

} // namespace domain
