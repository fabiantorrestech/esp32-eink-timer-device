#include "app/HybridRefreshPolicy.h"

#include "domain/Layout.h"

namespace app {

using domain::DirtyRegion;
using domain::RefreshDecision;
using domain::RenderModel;

DirtyRegion HybridRefreshPolicy::changedRegion(const RenderModel& next,
                                               const RenderModel& last) {
    DirtyRegion region{};
    if (next.display != last.display) {
        region = DirtyRegion::unionOf(region, domain::layout::kDigits);
    }
    // RenderModel::operator== already collapses sub-degree fraction jitter, so a
    // raw compare here would over-dirty; reuse the model's notion of "changed".
    RenderModel onlyFraction = last;
    onlyFraction.fractionRemaining = next.fractionRemaining;
    if (onlyFraction != last) {
        region = DirtyRegion::unionOf(region, domain::layout::kArc);
    }
    if (next.ledOn != last.ledOn || next.bleConnected != last.bleConnected) {
        region = DirtyRegion::unionOf(region, domain::layout::kStatus);
    }
    return region.alignedToByte();
}

RefreshDecision HybridRefreshPolicy::decide(const RenderModel& next,
                                            const RenderModel& last) {
    // Nothing changed and no full pending -> don't touch the panel.
    if (!forceFull_ && next == last) {
        return RefreshDecision{RefreshDecision::Kind::None, {}};
    }

    // Forced, or we've spent our partial budget -> cleansing full refresh.
    if (forceFull_ || partialCount_ >= maxPartials_) {
        forceFull_ = false;
        partialCount_ = 0;
        return RefreshDecision{RefreshDecision::Kind::Full, {}};
    }

    ++partialCount_;
    return RefreshDecision{RefreshDecision::Kind::Partial, changedRegion(next, last)};
}

} // namespace app
