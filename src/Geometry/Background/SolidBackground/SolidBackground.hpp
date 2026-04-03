#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Background/IBackground.hpp"

namespace RTC {
class SolidBackground : public IBackground {
private:
    Color<float> color_;

public:
    SolidBackground(Color<float> color);

    [[nodiscard]] Color<float> sample(const Ray& ray) const override;
};
}