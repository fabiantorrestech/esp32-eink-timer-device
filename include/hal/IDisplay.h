// IDisplay.h — display facade contract.
// Hides the e-paper paging loop, init quirks, and BUSY handling behind four
// methods. The render strategy (full vs partial) is decided elsewhere.
#pragma once

#include "domain/DirtyRegion.h"
#include "domain/RenderModel.h"

namespace hal {

class IDisplay {
public:
    virtual ~IDisplay() = default;

    virtual void begin() = 0;

    // Full-screen refresh (slow, flashes, clears ghosting).
    virtual void drawFull(const domain::RenderModel& model) = 0;

    // Partial-window refresh over `region` only (fast, no flash).
    virtual void drawPartial(const domain::RenderModel& model,
                             const domain::DirtyRegion& region) = 0;

    // Drop the panel into deep sleep to protect it and save power.
    virtual void hibernate() = 0;
};

} // namespace hal
