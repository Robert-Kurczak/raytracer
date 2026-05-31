#pragma once

#include <cstdint>

namespace RTC {
struct PathRendererParameters {
    uint32_t pathsPerPixel = 0;
    uint32_t lightSamplesPerHit = 0;
    uint32_t scatterRecursionDepth = 0;
};
}