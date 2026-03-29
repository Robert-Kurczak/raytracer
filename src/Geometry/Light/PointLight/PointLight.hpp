#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"

namespace RTC {
class PointLight : public ILight {
private:
    Point3<float> position_;
    Color<float> color_;
    float decay_;

public:
    PointLight(
        const Point3<float>& position,
        const Color<float>& color,
        float decay
    );

    PointLight(
        const Point3<float>& position,
        const Color8Bit& color,
        float decay
    );

    [[nodiscard]] LightData getSample(
        const Point3<float>& worldPosition
    ) const override;
};
}