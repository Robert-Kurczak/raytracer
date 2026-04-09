#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Background/IBackground.hpp"

namespace RTC {
class SolidBackground : public IBackground {
private:
    LinearColor color_;

public:
    SolidBackground(LinearColor color);

    [[nodiscard]] LinearColor sample(const Ray& ray) const override;
};
}