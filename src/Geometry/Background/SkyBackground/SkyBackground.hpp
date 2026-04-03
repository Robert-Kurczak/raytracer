#pragma once

#include "Geometry/Background/IBackground.hpp"

namespace RTC {
class SkyBackground : public IBackground {
public:
    [[nodiscard]] Color<float> sample(const Ray& ray) const override;
};
}