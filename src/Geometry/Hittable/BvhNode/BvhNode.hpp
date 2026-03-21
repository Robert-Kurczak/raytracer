#pragma once

#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Hittable/IHittable.hpp"

#include <memory>

namespace RTC {
class BvhNode : public IHittable {
private:
    std::unique_ptr<IHittable> left_;
    std::unique_ptr<IHittable> right_;
    AxisAlignedBoundingBox boundingBox_;

public:
    BvhNode(
        std::unique_ptr<IHittable> left,
        std::unique_ptr<IHittable> right,
        AxisAlignedBoundingBox boundingBox
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