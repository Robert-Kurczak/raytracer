#pragma once

#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Material/IMaterial.hpp"

#include <memory>

namespace RTC {
class Triangle : public IHittable {
private:
    const Point3<float> vertexA_;
    const Point3<float> vertexB_;
    const Point3<float> vertexC_;
    const AxisAlignedBoundingBox boundingBox_;
    std::shared_ptr<IMaterial> material_;

    const Vector3<float> edge1_;
    const Vector3<float> edge2_;
    const Vector3<float> outwardNormal_;

    [[nodiscard]] AxisAlignedBoundingBox createBoundingBox(
        const Point3<float>& vertexA,
        const Point3<float>& vertexB,
        const Point3<float>& vertexC
    ) const;

    void updateHitData(
        float rayT,
        const Ray& ray,
        HitData& hitData
    ) const;

public:
    Triangle(
        const Point3<float>& vertexA,
        const Point3<float>& vertexB,
        const Point3<float>& vertexC,
        std::shared_ptr<IMaterial> material
    );

    [[nodiscard]] const AxisAlignedBoundingBox&
    getBoundingBox() const override;

    bool isHit(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const override;
};
}