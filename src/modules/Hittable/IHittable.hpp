#pragma once

#include "Hittable/HitData.hpp"
#include "Math/Interval.hpp"
#include "Ray/Ray.hpp"

namespace RTC {
class IHittable {
public:
    IHittable() = default;
    IHittable(const IHittable&) = delete;
    IHittable(IHittable&&) = delete;
    IHittable operator=(const IHittable&) = delete;
    IHittable operator=(IHittable&&) = delete;
    virtual ~IHittable() = default;

    virtual bool isHit(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const = 0;
};
}