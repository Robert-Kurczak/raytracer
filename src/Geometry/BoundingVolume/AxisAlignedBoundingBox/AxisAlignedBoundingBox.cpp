#include "AxisAlignedBoundingBox.hpp"

#include <cstdint>

namespace RTC {
static constexpr float EPSILON = 0.0001F;

static constexpr uint8_t X_AXIS_INDEX = 0;
static constexpr uint8_t Y_AXIS_INDEX = 1;
static constexpr uint8_t Z_AXIS_INDEX = 2;

void AxisAlignedBoundingBox::padIntervals() {
    for (uint8_t axis = 0; axis < 3; axis++) {
        auto& interval = axisIntervals_[axis];

        if (interval.getSize() < EPSILON) {
            interval.expand(EPSILON);
        }
    }
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const Interval<float>& xInterval,
    const Interval<float>& yInterval,
    const Interval<float>& zInterval
) :
    axisIntervals_({xInterval, yInterval, zInterval}) {
    padIntervals();
}

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

    padIntervals();
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

    padIntervals();
}

const Interval<float>& AxisAlignedBoundingBox::getXAxisInterval() const {
    return axisIntervals_[X_AXIS_INDEX];
}

const Interval<float>& AxisAlignedBoundingBox::getYAxisInterval() const {
    return axisIntervals_[Y_AXIS_INDEX];
}

const Interval<float>& AxisAlignedBoundingBox::getZAxisInterval() const {
    return axisIntervals_[Z_AXIS_INDEX];
}

float AxisAlignedBoundingBox::getIntervalCenter(uint8_t axisIndex) const {
    const Interval<float>& interval = axisIntervals_[axisIndex];
    return (interval.start + interval.end) / 2.0F;
}

uint8_t AxisAlignedBoundingBox::getLongestAxisIndex() const {
    const float xSize = getXAxisInterval().getSize();
    const float ySize = getYAxisInterval().getSize();
    const float zSize = getZAxisInterval().getSize();

    if (xSize > ySize && xSize > zSize) {
        return X_AXIS_INDEX;
    }
    if (ySize > zSize) {
        return Y_AXIS_INDEX;
    }
    return Z_AXIS_INDEX;
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