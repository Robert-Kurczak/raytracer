#pragma once

#include "Core/Color/Color.hpp"

namespace RTC {
struct DiffuseParameters {
    LinearColor baseColor {.red = 0.0F, .green = 0.0F, .blue = 0.0F};
    LinearColor emission {.red = 0.0F, .green = 0.0F, .blue = 0.0F};
};
}