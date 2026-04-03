#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Ray/Ray.hpp"

namespace RTC {
class IBackground {
public:
    IBackground() = default;
    IBackground(const IBackground&) = delete;
    IBackground(IBackground&&) = delete;
    IBackground operator=(const IBackground&) = delete;
    IBackground operator=(IBackground&&) = delete;
    virtual ~IBackground() = default;

    [[nodiscard]] virtual Color<float> sample(const Ray& ray) const = 0;
};
}