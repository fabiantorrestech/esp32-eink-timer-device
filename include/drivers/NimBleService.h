// NimBleService.h — IBleService GATT stub over NimBLE-Arduino 2.x.
// One service with a notify "state" characteristic (publishes TimerSnapshot as
// JSON) and a writable "command" characteristic (remote control -> ICommandSink).
//
// NOTE: this targets the NimBLE-Arduino 2.x API. The callback signatures below
// (NimBLEConnInfo& parameters) are NOT compatible with 1.x examples.
// esp32dev build only.
#pragma once

#include <NimBLEDevice.h>

#include "domain/InputCommand.h"
#include "hal/IBleService.h"

namespace drivers {

class NimBleService : public hal::IBleService,
                      public NimBLEServerCallbacks,
                      public NimBLECharacteristicCallbacks {
public:
    // 128-bit custom UUIDs (Nordic-style base). Mirror these in the Android app.
    static constexpr const char* kServiceUuid = "6b9a0001-1f4c-4d5e-9b3a-1c2d3e4f5061";
    static constexpr const char* kStateUuid = "6b9a0002-1f4c-4d5e-9b3a-1c2d3e4f5061";
    static constexpr const char* kCommandUuid = "6b9a0003-1f4c-4d5e-9b3a-1c2d3e4f5061";

    void begin() override;
    void notifyState(const domain::TimerSnapshot& snapshot) override;
    void setCommandSink(domain::ICommandSink* sink) override { sink_ = sink; }
    bool isConnected() const override { return connected_; }

    // NimBLEServerCallbacks (2.x signatures)
    void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) override;
    void onDisconnect(NimBLEServer* server, NimBLEConnInfo& connInfo, int reason) override;

    // NimBLECharacteristicCallbacks (2.x signature)
    void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo& connInfo) override;

private:
    domain::ICommandSink* sink_ = nullptr;
    NimBLECharacteristic* stateChar_ = nullptr;
    bool connected_ = false;
};

} // namespace drivers
