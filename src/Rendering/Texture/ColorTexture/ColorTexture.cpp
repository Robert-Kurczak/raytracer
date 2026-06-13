#include "ColorTexture.hpp"

namespace RTC {
ColorTexture::ColorTexture(LinearColor color) : color_(color) {}

LinearColor ColorTexture::sample(float u, float v) const {
    return color_;
}
}