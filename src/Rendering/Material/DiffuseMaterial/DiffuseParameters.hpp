#pragma once

#include "Core/Color/Color.hpp"
#include "Rendering/Texture/ITexture.hpp"

#include <memory>

namespace RTC {
struct DiffuseParameters {
    std::shared_ptr<ITexture> colorTexture;
    LinearColor emission {.red = 0.0F, .green = 0.0F, .blue = 0.0F};
};
}