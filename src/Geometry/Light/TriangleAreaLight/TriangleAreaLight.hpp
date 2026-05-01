#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"

namespace RTC {
class TriangleAreaLight : public ILight {
private:
    const Point3<float> vertexA_;
    const Point3<float> vertexB_;
    const Point3<float> vertexC_;
    const LinearColor emission_;

    const Vector3<float> perpendicular_;
    const Vector3<float> normal_;
    const float pdf_;

    [[nodiscard]] Vector3<float> getPerpendicular(
        const Point3<float>& vertexA,
        const Point3<float>& vertexB,
        const Point3<float>& vertexC
    ) const;

    [[nodiscard]] Point3f getRandomSample() const;

public:
    TriangleAreaLight(
        const Point3<float>& vertexA,
        const Point3<float>& vertexB,
        const Point3<float>& vertexC,
        const LinearColor& emission
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