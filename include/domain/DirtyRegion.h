// DirtyRegion.h — a rectangular region of the panel that changed and must be
// redrawn. Pure C++; no Arduino.
#pragma once

#include <cstdint>

namespace domain {

struct DirtyRegion {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;

    bool isEmpty() const { return w <= 0 || h <= 0; }

    // SSD1683 partial windows snap to 8px on the X axis: round x down and w up
    // to multiples of 8 so the controller accepts the window.
    DirtyRegion alignedToByte() const {
        DirtyRegion r = *this;
        const int16_t x0 = static_cast<int16_t>(x & ~0x7);
        const int16_t x1 = static_cast<int16_t>((x + w + 7) & ~0x7);
        r.x = x0;
        r.w = static_cast<int16_t>(x1 - x0);
        return r;
    }

    // Bounding box union of two regions (used to merge changed areas).
    static DirtyRegion unionOf(const DirtyRegion& a, const DirtyRegion& b) {
        if (a.isEmpty()) return b;
        if (b.isEmpty()) return a;
        const int16_t x0 = a.x < b.x ? a.x : b.x;
        const int16_t y0 = a.y < b.y ? a.y : b.y;
        const int16_t x1 = (a.x + a.w) > (b.x + b.w) ? (a.x + a.w) : (b.x + b.w);
        const int16_t y1 = (a.y + a.h) > (b.y + b.h) ? (a.y + a.h) : (b.y + b.h);
        return DirtyRegion{x0, y0, static_cast<int16_t>(x1 - x0),
                           static_cast<int16_t>(y1 - y0)};
    }
};

} // namespace domain
