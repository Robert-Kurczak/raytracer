#include "BvhNode.hpp"

#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"

namespace RTC {
BvhNode::BvhNode(
    std::unique_ptr<IHittable> left,
    std::unique_ptr<IHittable> right,
    AxisAlignedBoundingBox boundingBox
) :
    left_(std::move(left)),
    right_(std::move(right)),
    boundingBox_(boundingBox) {}

const AxisAlignedBoundingBox& BvhNode::getBoundingBox() const {
    return boundingBox_;
}

bool BvhNode::isHit(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    if (!boundingBox_.isHit(ray, interval)) {
        return false;
    }

    return left_->isHit(ray, interval, hitData) ||
           right_->isHit(ray, interval, hitData);
}
}