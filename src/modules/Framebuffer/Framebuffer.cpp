#include "Framebuffer.hpp"

#include "Color/Color.hpp"
#include "Math/Vector.hpp"

namespace RTC {
Framebuffer::Framebuffer(const Vector2<uint32_t>& resolution) :
    resolution_(resolution),
    buffer_(size_t(resolution_.x * resolution_.y)) {}

void Framebuffer::setColorAt(
    const Point2<uint32_t>& point,
    const Color8Bit& color
) noexcept {
    const size_t index = point.x + (resolution_.x * point.y);
    buffer_.at(index) = color;
}

[[nodiscard]] const Vector2<uint32_t>& Framebuffer::
    getResolution() const noexcept {
    return resolution_;
}

std::span<const Color8Bit> Framebuffer::getConstBuffer() const noexcept {
    return buffer_;
}

const Color8Bit& Framebuffer::getColorAt(
    const Point2<uint32_t>& point
) const noexcept {
    const size_t index = point.x + (resolution_.x * point.y);

    return buffer_.at(index);
}
}
