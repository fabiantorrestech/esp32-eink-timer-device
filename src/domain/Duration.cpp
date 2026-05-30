#include "domain/Duration.h"

#include <cstdio>

namespace domain {

const char* Duration::format(char* buf, int bufLen) const {
    if (buf == nullptr || bufLen <= 0) return buf;
    const int h = hoursPart();
    const int m = minutesPart();
    const int s = secondsPart();
    if (h > 0) {
        std::snprintf(buf, static_cast<size_t>(bufLen), "%d:%02d:%02d", h, m, s);
    } else {
        // No hours: show total minutes so 90 min reads "90:00" only if < 60; here
        // anything >= 60 min has an hours part, so MM stays 0..59.
        std::snprintf(buf, static_cast<size_t>(bufLen), "%02d:%02d", m, s);
    }
    return buf;
}

} // namespace domain
