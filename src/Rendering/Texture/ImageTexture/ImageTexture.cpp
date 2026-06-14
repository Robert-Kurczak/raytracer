#include "ImageTexture.hpp"

#include "Core/Color/Color.hpp"

namespace RTC {
ImageTexture::ImageTexture(
    uint32_t width,
    uint32_t height,
    uint32_t channels,
    std::vector<float>&& imageData
) :
    width_(width),
    height_(height),
    channels_(channels),
    imageData_(std::move(imageData)) {}

LinearColor ImageTexture::sample(float u, float v) const {
    u *= float(width_ - 1);
    v *= float(height_ - 1);

    const auto baseIndex =
        uint32_t(float(channels_) * (float(width_) * v + u));

    return LinearColor {
        imageData_[baseIndex],
        imageData_[baseIndex + 1],
        imageData_[baseIndex + 2]
    };
}
}