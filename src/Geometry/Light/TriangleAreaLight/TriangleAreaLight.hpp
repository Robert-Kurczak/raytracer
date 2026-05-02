#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"

namespace RTC {
class TriangleAreaLight : public ILight {
private:
    const Point3f vertexA_;
    const Point3f vertexB_;
    const Point3f vertexC_;
    const LinearColor emission_;

    const Vector3f perpendicular_;
    const Vector3f normal_;
    const float area_;

    [[nodiscard]] Vector3f getPerpendicular(
        const Point3f& vertexA,
        const Point3f& vertexB,
        const Point3f& vertexC
    ) const;

    [[nodiscard]] Point3f getRandomSample() const;

public:
    TriangleAreaLight(
        const Point3f& vertexA,
        const Point3f& vertexB,
        const Point3f& vertexC,
        const LinearColor& emission
    );

    [[nodiscard]] LightSample getSample(
        const Point3f& origin
    ) const override;
};
}