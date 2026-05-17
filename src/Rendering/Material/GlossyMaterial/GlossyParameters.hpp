#pragma once

#include "Core/Color/Color.hpp"
namespace RTC {
struct GlossyParameters {
    float roughness = 0.1F;
    LinearColor fresnelBaseTerm = LinearColor::black();
};
}