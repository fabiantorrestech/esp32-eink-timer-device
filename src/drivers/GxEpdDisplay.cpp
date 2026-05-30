#include "drivers/GxEpdDisplay.h"

#include <Fonts/FreeMonoBold24pt7b.h>

#include "domain/Layout.h"

namespace drivers {

namespace {
constexpr int16_t kArcCx = domain::layout::kArc.x + domain::layout::kArc.w / 2;
constexpr int16_t kArcCy = domain::layout::kArc.y + domain::layout::kArc.h / 2;
constexpr int16_t kArcR = domain::layout::kArc.w / 2;
} // namespace

GxEpdDisplay::GxEpdDisplay()
    : display_(GxEPD2_420_GDEY042T81(pins::kEpdCs, pins::kEpdDc, pins::kEpdRst,
                                     pins::kEpdBusy)) {}

void GxEpdDisplay::begin() {
    display_.init(115200);
    display_.setRotation(0);
    display_.setTextColor(GxEPD_BLACK);
}

void GxEpdDisplay::drawFrame(const domain::RenderModel& model) {
    display_.fillScreen(GxEPD_WHITE);

    // Visual-timer wheel: outline + a filled wedge proportional to time left.
    // (Skeleton uses a simple ring; a true swept pie comes with the UI work.)
    display_.drawCircle(kArcCx, kArcCy, kArcR, GxEPD_BLACK);
    const int filled = static_cast<int>(model.fractionRemaining * kArcR);
    if (filled > 0) {
        display_.fillCircle(kArcCx, kArcCy, static_cast<int16_t>(filled), GxEPD_BLACK);
    }

    // Big numeric readout.
    char buf[12];
    model.display.format(buf, sizeof(buf));
    display_.setFont(&FreeMonoBold24pt7b);
    display_.setCursor(domain::layout::kDigits.x, domain::layout::kDigits.y + 48);
    display_.print(buf);

    // Status strip: BLE link hint.
    display_.setFont(nullptr);
    display_.setCursor(domain::layout::kStatus.x + 2, domain::layout::kStatus.y + 2);
    display_.print(model.bleConnected ? "BLE" : "   ");
}

void GxEpdDisplay::drawFull(const domain::RenderModel& model) {
    display_.setFullWindow();
    display_.firstPage();
    do {
        drawFrame(model);
    } while (display_.nextPage());
}

void GxEpdDisplay::drawPartial(const domain::RenderModel& model,
                               const domain::DirtyRegion& region) {
    const domain::DirtyRegion r = region.alignedToByte();
    display_.setPartialWindow(r.x, r.y, r.w, r.h);
    display_.firstPage();
    do {
        drawFrame(model);
    } while (display_.nextPage());
}

void GxEpdDisplay::hibernate() {
    display_.hibernate();
}

} // namespace drivers
