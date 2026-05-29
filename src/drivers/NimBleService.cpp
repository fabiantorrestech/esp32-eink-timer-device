#include "drivers/NimBleService.h"

#include <ArduinoJson.h>

#include <cstring>

namespace drivers {

void NimBleService::begin() {
    NimBLEDevice::init("VisualTimer");

    NimBLEServer* server = NimBLEDevice::createServer();
    server->setCallbacks(this, /*deleteOnDisconnect=*/false);

    NimBLEService* service = server->createService(kServiceUuid);

    stateChar_ = service->createCharacteristic(
        kStateUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

    NimBLECharacteristic* cmdChar = service->createCharacteristic(
        kCommandUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    cmdChar->setCallbacks(this);

    service->start();

    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    adv->addServiceUUID(service->getUUID());
    adv->enableScanResponse(true);
    NimBLEDevice::startAdvertising();
}

void NimBleService::notifyState(const domain::TimerSnapshot& snapshot) {
    if (stateChar_ == nullptr || !connected_) return;

    JsonDocument doc;
    doc["phase"] = static_cast<int>(snapshot.phase);
    doc["remaining"] = snapshot.display.seconds();
    doc["total"] = snapshot.total.seconds();
    doc["frac"] = snapshot.fractionRemaining;

    char buf[96];
    const size_t n = serializeJson(doc, buf, sizeof(buf));
    stateChar_->setValue(reinterpret_cast<const uint8_t*>(buf), n);
    stateChar_->notify();
}

void NimBleService::onConnect(NimBLEServer*, NimBLEConnInfo&) {
    connected_ = true;
}

void NimBleService::onDisconnect(NimBLEServer*, NimBLEConnInfo&, int) {
    connected_ = false;
    NimBLEDevice::startAdvertising(); // re-advertise so the app can reconnect
}

void NimBleService::onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo&) {
    if (sink_ == nullptr) return;
    const std::string value(characteristic->getValue().c_str());

    domain::InputCommand cmd = domain::InputCommand::None;
    if (value == "start" || value == "pause") cmd = domain::InputCommand::StartPause;
    else if (value == "reset") cmd = domain::InputCommand::Reset;
    else if (value == "+") cmd = domain::InputCommand::Increment;
    else if (value == "-") cmd = domain::InputCommand::Decrement;
    else if (value == "menu") cmd = domain::InputCommand::EnterMenu;
    else if (value == "back") cmd = domain::InputCommand::Back;

    if (cmd != domain::InputCommand::None) sink_->submit(cmd);
}

} // namespace drivers
