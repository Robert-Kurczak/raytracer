#include "AxisAlignedBoundingBox.hpp"

#include <cstdint>

namespace RTC {
AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const Interval<float>& xInterval,
    const Interval<float>& yInterval,
    const Interval<float>& zInterval
) :
    axisIntervals_({xInterval, yInterval, zInterval}) {}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const Point3<float>& startPoint,
    const Point3<float>& endPoint
) {
    for (uint8_t axis = 0; axis < 3; axis++) {
        if (startPoint[axis] < endPoint[axis]) {
            axisIntervals_.at(axis) = {startPoint[axis], endPoint[axis]};
        } else {
            axisIntervals_.at(axis) = {endPoint[axis], startPoint[axis]};
        }
    }
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const AxisAlignedBoundingBox& box1,
    const AxisAlignedBoundingBox& box2
) {
    for (uint8_t axis = 0; axis < 3; axis++) {
        axisIntervals_[axis].start = std::min(
            box1.axisIntervals_[axis].start,
            box2.axisIntervals_[axis].start
        );

        axisIntervals_[axis].end = std::max(
            box1.axisIntervals_[axis].end, box2.axisIntervals_[axis].end
        );
    }
}

float AxisAlignedBoundingBox::getIntervalCenter(uint8_t axisIndex) const {
    const Interval<float>& interval = axisIntervals_[axisIndex];
    return (interval.end - interval.start) / 2.0F;
}

bool AxisAlignedBoundingBox::isHit(
    const Ray& ray,
    const Interval<float>& rayInterval
) const {
    const Point3<float> rayOrigin = ray.getOrigin();
    const Vector3<float> rayDirection = ray.getDirection();

    float startT = rayInterval.start;
    float endT = rayInterval.end;

    for (uint8_t axis = 0; axis < 3; axis++) {
        const float axisStart = axisIntervals_[axis].start;
        const float axisEnd = axisIntervals_[axis].end;
        const float origin = rayOrigin[axis];
        const float direction = rayDirection[axis];
        const float directionInverse = 1.0F / direction;

        float axisStartT = (axisStart - origin) * directionInverse;
        float axisEndT = (axisEnd - origin) * directionInverse;

        if (direction < 0) {
            std::swap(axisStartT, axisEndT);
        }

        startT = std::max(startT, axisStartT);
        endT = std::min(endT, axisEndT);

        if (startT >= endT) {
            return false;
        }
    }

    return startT < endT;
}
}