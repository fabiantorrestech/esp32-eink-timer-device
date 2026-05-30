// GxEpdDisplay.h — IDisplay facade over GxEPD2 + Adafruit_GFX.
// Hides the paged firstPage()/nextPage() loop, partial-window setup, and BUSY
// handling. Only compiled in the esp32dev (Arduino) build.
#pragma once

#include <GxEPD2_BW.h>

#include "Pins.h"
#include "hal/IDisplay.h"

namespace drivers {

class GxEpdDisplay : public hal::IDisplay {
public:
    GxEpdDisplay();

    void begin() override;
    void drawFull(const domain::RenderModel& model) override;
    void drawPartial(const domain::RenderModel& model,
                     const domain::DirtyRegion& region) override;
    void hibernate() override;

private:
    // Render one complete frame into the (already-selected) window.
    void drawFrame(const domain::RenderModel& model);

    // Waveshare 4.2" 400x300 B/W, SSD1683 (V2). Full-height page buffer.
    GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display_;
};

} // namespace drivers
