#pragma once

#include <cstdint>

namespace RTC {
struct PhotonMapRendererParameters {
    uint32_t pathsPerPixel = 0;
    uint32_t scatterRecursionDepth = 0;
    uint32_t emittedPhotons = 0;
    uint32_t nearestPhotons = 0;
};
}