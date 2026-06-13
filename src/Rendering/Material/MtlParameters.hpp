#pragma once

#include "Core/Color/Color.hpp"
#include "Rendering/Texture/ITexture.hpp"

#include <cstdint>
#include <memory>

namespace RTC {
struct MtlParameters {
    std::shared_ptr<ITexture> diffuse;
    LinearColor ambient = LinearColor::black();
    LinearColor specular = LinearColor::black();
    LinearColor emission = LinearColor::black();
    float shininess = 0.0F;
    float transparency = 0.0F;
    float refractionIndex = 0.0F;
    LinearColor transmisionFilter = BLACK_LINEAR_COLOR;
    uint8_t illuminationModel = UINT8_MAX;
};
}