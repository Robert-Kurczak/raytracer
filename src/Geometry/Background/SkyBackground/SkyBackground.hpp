#pragma once

#include "Geometry/Background/IBackground.hpp"

namespace RTC {
class SkyBackground : public IBackground {
public:
    [[nodiscard]] LinearColor sample(const Ray& ray) const override;
};
}