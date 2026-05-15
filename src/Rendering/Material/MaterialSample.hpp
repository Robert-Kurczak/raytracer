#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
struct MaterialSample {
    Vector3f inDirection {0.0F, 0.0F, 0.0F};
    LinearColor brdf {.red = 0.0F, .green = 0.0F, .blue = 0.0F};
    float pdf = 0.0F;
};
}