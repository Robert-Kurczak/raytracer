#pragma once

#include "Core/Color/Color.hpp"

#include <cstdint>
#include <filesystem>

namespace RTC {
struct MtlParameters {
    LinearColor diffuse = LinearColor::black();
    LinearColor ambient = LinearColor::black();
    LinearColor specular = LinearColor::black();
    LinearColor emission = LinearColor::black();
    std::filesystem::path diffuseTexturePath;
    float shininess = 0.0F;
    float transparency = 0.0F;
    float refractionIndex = 0.0F;
    LinearColor transmisionFilter = BLACK_LINEAR_COLOR;
    uint8_t illuminationModel = UINT8_MAX;
};
}