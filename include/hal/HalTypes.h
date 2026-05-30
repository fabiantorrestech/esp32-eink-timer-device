// HalTypes.h — small enums/structs shared across HAL interfaces.
// Pure C++ (no Arduino) so the domain and tests can use them freely.
#pragma once

#include <cstdint>

namespace hal {

// Semantic button events produced by IButtonInput (debounced/decoded by the driver).
enum class ButtonEvent : uint8_t {
    None = 0,
    Click,
    DoubleClick,
    LongPress,
};

// On/off indicator state for IIndicator.
enum class IndicatorState : uint8_t {
    Off = 0,
    On,
    Blink,
};

// Why the device woke from deep sleep (mapped from esp_sleep_get_wakeup_cause()).
enum class WakeReason : uint8_t {
    PowerOn = 0, // cold boot / reset, not a sleep wake
    RtcAlarm,    // ext0: DS3231 alarm fired
    UserInput,   // ext1: a button / encoder press
    Timer,       // internal timer wake
    Other,
};

// Which wake sources to arm before entering deep sleep.
struct WakeSources {
    bool rtcAlarm = false; // ext0 on the DS3231 INT pin
    bool userInput = false; // ext1 on the button/encoder mask
};

} // namespace hal
