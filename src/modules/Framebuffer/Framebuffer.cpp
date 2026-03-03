#include "Framebuffer.hpp"

#include "Color/Color.hpp"
#include "Math/Vector.hpp"

namespace RTC {
Framebuffer::Framebuffer(const Vector2<uint32_t>& resolution) :
    resolution_(resolution),
    buffer_(size_t(resolution_.getX() * resolution_.getY())) {}

void Framebuffer::setColorAt(
    const Point2<uint32_t>& point,
    const Color8Bit& color
) noexcept {
    const size_t index =
        point.getX() + (resolution_.getX() * point.getY());
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
    const size_t index =
        point.getX() + (resolution_.getX() * point.getY());

    return buffer_.at(index);
}
}
