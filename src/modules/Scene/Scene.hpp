#pragma once

#include "Hittable/HitData.hpp"
#include "Hittable/IHittable.hpp"
#include "Math/Interval.hpp"
#include "Ray/Ray.hpp"

#include <memory>
#include <vector>

namespace RTC {
class Scene {
private:
    std::vector<std::unique_ptr<IHittable>> objects_;

public:
    void addObject(std::unique_ptr<IHittable> object);

    bool hitRay(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const;
};
}