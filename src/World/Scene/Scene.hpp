#pragma once

#include "Core/Math/Interval.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Hittable/IHittable.hpp"

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