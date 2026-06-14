#pragma once

#include "Rendering/Texture/ITexture.hpp"

#include <vector>

namespace RTC {
class ImageTexture : public ITexture {
private:
    const uint32_t width_;
    const uint32_t height_;
    const uint32_t channels_;
    const std::vector<float> imageData_;

public:
    ImageTexture(
        uint32_t width,
        uint32_t height,
        uint32_t channels,
        std::vector<float>&& imageData
    );

    [[nodiscard]] LinearColor sample(float u, float v) const override;
};
}