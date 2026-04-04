#pragma once

#include <cstdint>

namespace RTC {
struct RendererStatistics {
    uint32_t rays = 0;
    uint32_t shadowRays = 0;
};
}