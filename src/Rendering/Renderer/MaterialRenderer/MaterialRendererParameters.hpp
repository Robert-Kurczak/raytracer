#pragma once

#include "Geometry/Material/IMaterial.hpp"

#include <cstdint>
#include <memory>

namespace RTC {
struct MaterialRendererParameters {
    uint32_t samplesPerPixel;
    uint32_t scatterRecursionDepth;
    std::shared_ptr<IMaterial> defaultMaterial_;
};
}