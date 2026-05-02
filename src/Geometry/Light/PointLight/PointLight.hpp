#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/LightSample.hpp"

namespace RTC {
class PointLight : public ILight {
private:
    Point3f position_;
    LinearColor emission_;
    float decay_;

public:
    PointLight(
        const Point3f& position,
        const LinearColor& emission,
        float decay
    );

    PointLight(
        const Point3f& position,
        const Color8Bit& emission,
        float decay
    );

    [[nodiscard]] LightSample getSample(
        const Point3f& origin
    ) const override;
};
}