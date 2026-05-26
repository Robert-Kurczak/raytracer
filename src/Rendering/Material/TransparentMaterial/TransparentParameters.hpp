#pragma once

#include "Core/Color/Color.hpp"

namespace RTC {
struct TransparentParameters {
    float refractionIndex = 0.0F;
    LinearColor emission = LinearColor::black();
    LinearColor transmissionFilter = LinearColor::black();
};
}