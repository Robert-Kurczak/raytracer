#pragma once

#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Hittable/Triangle/Triangle.hpp"

#include <memory>
#include <vector>

namespace RTC {
class Mesh : public IHittable {
private:
    std::vector<std::unique_ptr<Triangle>> triangles_;

public:
    void addTriangle(std::unique_ptr<Triangle> triangle);

    bool isHit(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const override;
};
}