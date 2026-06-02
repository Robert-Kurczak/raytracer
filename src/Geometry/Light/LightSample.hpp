#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
struct LightSample {
    LinearColor outLight {.red = 0.0F, .green = 0.0F, .blue = 0.0F};
    Vector3f toLight {0.0F, 0.0F, 0.0F};
    Vector3f inDirection {0.0F, 0.0F, 0.0F};
    float pdf = 0.0F;
};
}