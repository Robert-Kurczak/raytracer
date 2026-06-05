#pragma once

#include <cstdint>

namespace RTC {
struct LightCutsSamplerParameters {
    uint32_t discreteSamplerPerLight;
    float maxError;
    uint32_t maxCutSize;
};
}