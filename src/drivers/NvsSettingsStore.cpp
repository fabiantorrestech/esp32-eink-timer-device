#include "drivers/NvsSettingsStore.h"

namespace drivers {

bool NvsSettingsStore::load(domain::SettingsModel& out) {
    prefs_.begin(kNamespace, /*readOnly=*/true);
    const uint16_t version = prefs_.getUShort("ver", 0);
    if (version != domain::SettingsModel::kSchemaVersion) {
        prefs_.end();
        return false; // nothing stored, or an old/incompatible layout
    }
    out.schemaVersion = version;
    out.defaultDuration =
        domain::Duration(static_cast<int32_t>(prefs_.getInt("dur", 1500)));
    out.mode = static_cast<domain::TimerMode>(prefs_.getUChar("mode", 0));
    out.ledPolicy = static_cast<domain::LedPolicy>(prefs_.getUChar("led", 2));
    out.soundEnabled = prefs_.getBool("snd", false);
    prefs_.end();
    return true;
}

void NvsSettingsStore::save(const domain::SettingsModel& in) {
    prefs_.begin(kNamespace, /*readOnly=*/false);
    prefs_.putUShort("ver", domain::SettingsModel::kSchemaVersion);
    prefs_.putInt("dur", in.defaultDuration.seconds());
    prefs_.putUChar("mode", static_cast<uint8_t>(in.mode));
    prefs_.putUChar("led", static_cast<uint8_t>(in.ledPolicy));
    prefs_.putBool("snd", in.soundEnabled);
    prefs_.end();
}

} // namespace drivers
