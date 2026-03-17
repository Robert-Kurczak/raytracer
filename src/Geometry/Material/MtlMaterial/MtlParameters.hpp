#pragma once

#include "Core/Color/Color.hpp"

namespace RTC {
struct MtlParameters {
    Color<float> ambient {.red = 0, .green = 0, .blue = 0};
    Color<float> diffuse {.red = 0, .green = 0, .blue = 0};
    Color<float> specular {.red = 0, .green = 0, .blue = 0};
    float shininess = 0.0F;
    float transparency = 0.0F;
};
}