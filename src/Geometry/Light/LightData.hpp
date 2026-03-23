#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
struct LightData {
    Vector3<float> toLight {0.0F, 0.0F, 0.0F};
    float distance = 0.0F;
    Color<float> illumination =
        {.red = 0.0F, .green = 0.0F, .blue = 0.0F};
};
}