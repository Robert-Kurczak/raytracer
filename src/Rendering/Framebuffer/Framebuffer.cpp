#include "Framebuffer.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
Framebuffer::Framebuffer(const Vector2<uint32_t>& resolution) :
    resolution_(resolution),
    buffer_(size_t(resolution_.getX() * resolution_.getY())) {}

void Framebuffer::setColorAt(
    const Point2<uint32_t>& point,
    const LinearColor& color
) noexcept {
    const size_t index =
        point.getX() + (resolution_.getX() * point.getY());
    buffer_.at(index) = color;
}

const Vector2<uint32_t>& Framebuffer::getResolution() const noexcept {
    return resolution_;
}

std::span<const LinearColor> Framebuffer::
    getConstBuffer() const noexcept {
    return buffer_;
}

const LinearColor& Framebuffer::getColorAt(
    const Point2<uint32_t>& point
) const noexcept {
    const size_t index =
        point.getX() + (resolution_.getX() * point.getY());

    return buffer_[index];
}
}
