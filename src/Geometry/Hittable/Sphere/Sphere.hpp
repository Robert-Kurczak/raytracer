#pragma once

#include "Core/Math/Point.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Hittable/IHittable.hpp"

namespace RTC {
class Sphere : public IHittable {
private:
    const Point3<float> center_ {0.0F, 0.0F, 0.0F};
    const float radius_ = 0.0F;
    const float radiusSquared_ = 0.0F;

    void updateHitData(
        float rayT,
        const Ray& ray,
        HitData& hitData
    ) const;

public:
    Sphere(const Point3<float>& center, float radius);

    bool isHit(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const override;
};
}