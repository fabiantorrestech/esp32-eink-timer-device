// Layout.h — on-screen geometry for the 400x300 panel.
// Pure data, shared by HybridRefreshPolicy (to size dirty regions) and the
// display facade (to actually draw). Keeping it here makes the dirty-region
// math unit-testable without a real panel.
#pragma once

#include "domain/DirtyRegion.h"

namespace domain {
namespace layout {

inline constexpr int16_t kPanelW = 400;
inline constexpr int16_t kPanelH = 300;

// Big numeric readout, centered.
inline constexpr DirtyRegion kDigits{96, 110, 208, 64};

// Progress arc / disc bounding box (the visual-timer wheel).
inline constexpr DirtyRegion kArc{120, 20, 160, 160};

// Top status strip (BLE link + LED hint icons).
inline constexpr DirtyRegion kStatus{0, 0, kPanelW, 18};

} // namespace layout
} // namespace domain
