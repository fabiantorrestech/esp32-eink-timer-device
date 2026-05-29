// IBleService.h — companion-app link contract.
// Today: NimBLE GATT. Tomorrow a WiFi/cloud sync service can implement the same
// publish/command contract for Focusmate / Google Calendar without touching the
// domain or UI.
#pragma once

#include "domain/InputCommand.h"
#include "domain/TimerSnapshot.h"

namespace hal {

class IBleService {
public:
    virtual ~IBleService() = default;

    virtual void begin() = 0;

    // Publish current state to subscribers (notify characteristic).
    virtual void notifyState(const domain::TimerSnapshot& snapshot) = 0;

    // Remote writes are translated to InputCommands and pushed into this sink,
    // so app control is identical whether it came from a knob or a phone.
    virtual void setCommandSink(domain::ICommandSink* sink) = 0;

    virtual bool isConnected() const = 0;
};

} // namespace hal
