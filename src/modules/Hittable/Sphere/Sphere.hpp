#pragma once

#include "Hittable/IHittable.hpp"
#include "Math/Point.hpp"

namespace RTC {
class Sphere : IHittable {
private:
    const Point3<float> center_ {0.0F, 0.0F, 0.0F};
    const float radius_ = 0.0F;
    const float radiusSquared_ = 0.0F;

public:
    Sphere(const Point3<float>& center, float radius);

    bool isHit(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const override;
};
}