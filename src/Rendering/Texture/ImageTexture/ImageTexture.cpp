#include "ImageTexture.hpp"

#include "Core/Color/Color.hpp"

#include <cmath>
#include <format>
#include <iostream>

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
    u -= std::floor(u);
    v -= std::floor(v);

    v = 1.0F - v;

    const auto xIndex = uint32_t(u * float(width_));
    const auto yIndex = uint32_t(v * float(height_));

    const auto baseIndex = channels_ * ((yIndex * width_) + xIndex);

    return LinearColor {
        .red = imageData_[baseIndex],
        .green = imageData_[baseIndex + 1],
        .blue = imageData_[baseIndex + 2]
    };
}
}