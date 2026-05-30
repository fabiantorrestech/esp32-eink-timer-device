// OneButtonInput.h — IButtonInput over the OneButton library.
// Decodes click / double-click / long-press; latches the latest for takeEvent().
// esp32dev build only.
#pragma once

#include <OneButton.h>

#include "hal/IButtonInput.h"

namespace drivers {

class OneButtonInput : public hal::IButtonInput {
public:
    // activeLow + internal pull-up are the norm for a button to GND.
    explicit OneButtonInput(uint8_t pin) : button_(pin, true, true) {}

    void begin() override;
    void poll() override { button_.tick(); }
    hal::ButtonEvent takeEvent() override;

private:
    static void onClick(void* ctx);
    static void onDoubleClick(void* ctx);
    static void onLongPress(void* ctx);

    OneButton button_;
    hal::ButtonEvent pending_ = hal::ButtonEvent::None;
};

} // namespace drivers
