#pragma once

#include "Core/Color/Color.hpp"

namespace RTC {
class ITexture {
public:
    ITexture() = default;
    ITexture(const ITexture&) = delete;
    ITexture(ITexture&&) = delete;
    ITexture& operator=(const ITexture&) = delete;
    ITexture& operator=(ITexture&&) = delete;
    virtual ~ITexture() = default;

    [[nodiscard]] virtual LinearColor sample(float u, float v) const = 0;
};
}