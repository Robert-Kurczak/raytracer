#pragma once

#include "Rendering/Texture/ITexture.hpp"

namespace RTC {
class ImageTexture : public ITexture {
public:
    [[nodiscard]] LinearColor sample(float u, float v) const override;
};
}