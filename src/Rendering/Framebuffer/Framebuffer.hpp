#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Point.hpp"
#include "Core/Math/Vector.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace RTC {
class Framebuffer {
private:
    Vector2<uint32_t> resolution_;
    std::vector<LinearColor> buffer_;

public:
    Framebuffer(const Vector2<uint32_t>& resolution);

    void setColorAt(
        const Point2<uint32_t>& point,
        const LinearColor& color
    ) noexcept;

    [[nodiscard]] const Vector2<uint32_t>& getResolution() const noexcept;

    [[nodiscard]] std::span<const LinearColor>
    getConstBuffer() const noexcept;

    [[nodiscard]] const LinearColor& getColorAt(
        const Point2<uint32_t>& point
    ) const noexcept;
};
}