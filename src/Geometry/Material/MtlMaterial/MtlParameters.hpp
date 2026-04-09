#pragma once

#include "Core/Color/Color.hpp"

namespace RTC {
struct MtlParameters {
    LinearColor ambient {.red = 0, .green = 0, .blue = 0};
    LinearColor diffuse {.red = 0, .green = 0, .blue = 0};
    LinearColor specular {.red = 0, .green = 0, .blue = 0};
    float shininess = 0.0F;
    float transparency = 0.0F;
};
}