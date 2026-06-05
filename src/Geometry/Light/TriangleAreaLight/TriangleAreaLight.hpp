#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
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
    const LinearColor power_;
    const AxisAlignedBoundingBox boundingBox_;

    [[nodiscard]] AxisAlignedBoundingBox createBoundingBox(
        const Point3f& vertexA,
        const Point3f& vertexB,
        const Point3f& vertexC
    ) const;

    [[nodiscard]] Vector3f getPerpendicular(
        const Point3f& vertexA,
        const Point3f& vertexB,
        const Point3f& vertexC
    ) const;

    [[nodiscard]] Point3f getRandomPoint() const;

public:
    TriangleAreaLight(
        const Point3f& vertexA,
        const Point3f& vertexB,
        const Point3f& vertexC,
        const LinearColor& emission
    );

    void setup(const AxisAlignedBoundingBox& sceneBoundingBox) override;

    void discretize(
        std::vector<std::shared_ptr<ILight>>& discreteLights,
        uint32_t samples
    ) const override;

    [[nodiscard]] bool isInfinite() const override;

    [[nodiscard]] AxisAlignedBoundingBox getBoundingBox() const override;

    [[nodiscard]] LightSample getSample(
        const Point3f& origin
    ) const override;

    [[nodiscard]] LinearColor getPower() const override;

    [[nodiscard]] Photon emitPhoton() const override;
};
}