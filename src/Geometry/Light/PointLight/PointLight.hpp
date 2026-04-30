#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/LightSample.hpp"

namespace RTC {
class PointLight : public ILight {
private:
    Point3<float> position_;
    LinearColor color_;
    float decay_;

public:
    PointLight(
        const Point3<float>& position,
        const LinearColor& color,
        float decay
    );

    PointLight(
        const Point3<float>& position,
        const Color8Bit& color,
        float decay
    );

    [[nodiscard]] LightSample getSample(
        const Point3<float>& worldPosition,
        const Vector3<float>& worldNormal
    ) const override;

    [[nodiscard]] LightSample getSample(
        const Point3f& origin
    ) const override;
};
}