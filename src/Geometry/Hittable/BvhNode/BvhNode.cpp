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

bool BvhNode::hitClosest(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    if (not boundingBox_.isHit(ray, interval)) {
        return false;
    }

    const bool hitLeft = left_->hitClosest(ray, interval, hitData);

    Interval<float> rightRayInterval = interval;
    if (hitLeft) {
        rightRayInterval.end = hitData.rayT;
    }

    const bool hitRight =
        right_->hitClosest(ray, rightRayInterval, hitData);

    return hitLeft || hitRight;
}

bool BvhNode::hitAny(
    const Ray& ray,
    const Interval<float>& interval
) const {
    if (not boundingBox_.isHit(ray, interval)) {
        return false;
    }

    const bool hitLeft = left_->hitAny(ray, interval);

    if (hitLeft) {
        return true;
    }

    const bool hitRight = right_->hitAny(ray, interval);

    return hitRight;
}
}