#include "drivers/OneButtonInput.h"

namespace drivers {

void OneButtonInput::begin() {
    button_.attachClick(&OneButtonInput::onClick, this);
    button_.attachDoubleClick(&OneButtonInput::onDoubleClick, this);
    button_.attachLongPressStart(&OneButtonInput::onLongPress, this);
}

hal::ButtonEvent OneButtonInput::takeEvent() {
    const hal::ButtonEvent e = pending_;
    pending_ = hal::ButtonEvent::None;
    return e;
}

void OneButtonInput::onClick(void* ctx) {
    static_cast<OneButtonInput*>(ctx)->pending_ = hal::ButtonEvent::Click;
}

void OneButtonInput::onDoubleClick(void* ctx) {
    static_cast<OneButtonInput*>(ctx)->pending_ = hal::ButtonEvent::DoubleClick;
}

void OneButtonInput::onLongPress(void* ctx) {
    static_cast<OneButtonInput*>(ctx)->pending_ = hal::ButtonEvent::LongPress;
}

} // namespace drivers
