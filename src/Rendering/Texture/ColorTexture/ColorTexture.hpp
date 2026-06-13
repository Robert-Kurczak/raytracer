#pragma once

#include "Core/Color/Color.hpp"
#include "Rendering/Texture/ITexture.hpp"

namespace RTC {
class ColorTexture : public ITexture {
private:
    const LinearColor color_;

public:
    ColorTexture(LinearColor color);

    [[nodiscard]] LinearColor sample(float u, float v) const override;
};
}