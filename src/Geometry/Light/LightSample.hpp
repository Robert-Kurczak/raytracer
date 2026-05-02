#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
struct LightSample {
    LinearColor outLight {0.0F, 0.0F, 0.0F};
    Vector3f toLight {0.0F, 0.0F, 0.0F};
    Vector3f inDirection {0.0F, 0.0F, 0.0F};
    float pdf = 0.0F;
};
}