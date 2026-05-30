// InputCommand.h — the uniform command vocabulary.
// Every input source (encoder, buttons, BLE) is translated into one of these,
// so remote control and physical control travel the same path. Pure C++.
#pragma once

#include <cstdint>

namespace domain {

enum class InputCommand : uint8_t {
    None = 0,
    StartPause,    // toggle running/paused; from idle, starts
    Reset,         // back to idle / reload default
    Increment,     // bump duration up (encoder CW / app +)
    Decrement,     // bump duration down (encoder CCW / app -)
    EnterMenu,     // open settings
    Back,          // leave settings / cancel
    Tick,          // 1 Hz time advance (from TimerTask)
};

// Sink for commands — implemented by the UI controller. Encoders/buttons/BLE
// hold a pointer to one of these (Command pattern).
class ICommandSink {
public:
    virtual ~ICommandSink() = default;
    virtual void submit(InputCommand cmd) = 0;
};

} // namespace domain
