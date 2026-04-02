#pragma once

#include "Core/Color/Color.hpp"

namespace RTC {
struct DiffuseParameters {
    Color<float> baseColor {.red = 0.0F, .green = 0.0F, .blue = 0.0F};
    float roughness = 0.0F;
};
}