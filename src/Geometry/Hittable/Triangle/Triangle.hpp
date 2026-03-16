#pragma once

#include "Geometry/Hittable/IHittable.hpp"

namespace RTC {
class Triangle : public IHittable {
private:
    const Point3<float> vertexA_;
    const Point3<float> vertexB_;
    const Point3<float> vertexC_;

    const Vector3<float> edge1_;
    const Vector3<float> edge2_;
    const Vector3<float> outwardNormal_;

    void updateHitData(
        float rayT,
        const Ray& ray,
        HitData& hitData
    ) const;

public:
    Triangle(
        const Point3<float>& vertexA,
        const Point3<float>& vertexB,
        const Point3<float>& vertexC
    );

    bool isHit(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const override;
};
}