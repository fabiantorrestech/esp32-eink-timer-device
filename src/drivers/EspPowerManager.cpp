#include "drivers/EspPowerManager.h"

#include <esp_sleep.h>

#include "Pins.h"

namespace drivers {

hal::WakeReason EspPowerManager::wakeReason() const {
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_EXT0: return hal::WakeReason::RtcAlarm;
        case ESP_SLEEP_WAKEUP_EXT1: return hal::WakeReason::UserInput;
        case ESP_SLEEP_WAKEUP_TIMER: return hal::WakeReason::Timer;
        case ESP_SLEEP_WAKEUP_UNDEFINED: return hal::WakeReason::PowerOn;
        default: return hal::WakeReason::Other;
    }
}

void EspPowerManager::enterDeepSleep(const hal::WakeSources& sources) {
    if (sources.rtcAlarm) {
        // DS3231 INT is active-low/open-drain: wake when it pulls the line low.
        esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(pins::kRtcInt), 0);
    }
    if (sources.userInput) {
        // Buttons/encoder push are active-low (INPUT_PULLUP) -> wake on ALL_LOW.
        esp_sleep_enable_ext1_wakeup(pins::kExt1WakeMask, ESP_EXT1_WAKEUP_ALL_LOW);
    }
    esp_deep_sleep_start(); // does not return
}

} // namespace drivers
