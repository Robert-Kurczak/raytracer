#pragma once

#include "Color/Color.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace RTC {
class Framebuffer {
private:
    Vector2<uint32_t> resolution_;
    std::vector<Color8Bit> buffer_;

public:
    Framebuffer(const Vector2<uint32_t>& resolution);

    void setColorAt(
        const Point2<uint32_t>& point,
        const Color8Bit& color
    ) noexcept;

    [[nodiscard]] const Vector2<uint32_t>& getResolution() const noexcept;

    [[nodiscard]] std::span<const Color8Bit>
    getConstBuffer() const noexcept;

    [[nodiscard]] const Color8Bit& getColorAt(
        const Point2<uint32_t>& point
    ) const noexcept;
};
}