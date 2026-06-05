#include "AxisAlignedBoundingBox.hpp"

#include "Core/Math/Numeric.hpp"

#include <cassert>
#include <cstdint>

namespace RTC {
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
    const Point3f& centerPoint,
    float sideLength
) {
    const float halfLength = sideLength / 2.0F;

    for (uint8_t axis = 0; axis < 3; axis++) {
        axisIntervals_.at(axis) = {
            centerPoint[axis] - halfLength, centerPoint[axis] + halfLength
        };
    }

    padIntervals();
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const Point3f& centerPoint
) :
    AxisAlignedBoundingBox(centerPoint, 0.0F) {}

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

Point3f AxisAlignedBoundingBox::getBoxCenter() const {
    return Point3f {
        getIntervalCenter(0), getIntervalCenter(1), getIntervalCenter(2)
    };
}

float AxisAlignedBoundingBox::getDiagonal() const {
    const Point3f startPoint {
        axisIntervals_[0].start,
        axisIntervals_[1].start,
        axisIntervals_[2].start
    };

    const Point3f endPoint {
        axisIntervals_[0].end,
        axisIntervals_[1].end,
        axisIntervals_[2].end
    };

    return (endPoint - startPoint).getLength();
}

float AxisAlignedBoundingBox::getDistanceFromAxis(
    const Point3f& point,
    uint8_t axisIndex
) const {
    const float axisStart = axisIntervals_[axisIndex].start;
    const float axisEnd = axisIntervals_[axisIndex].end;

    if (point[axisIndex] < axisStart) {
        return axisStart - point[axisIndex];
    }

    if (point[axisIndex] > axisEnd) {
        return point[axisIndex] - axisEnd;
    }

    return 0.0F;
}

float AxisAlignedBoundingBox::getDistanceSquared(
    const Point3f& point
) const {
    const float xAxisDistance = getDistanceFromAxis(point, X_AXIS_INDEX);
    const float yAxisDistance = getDistanceFromAxis(point, Y_AXIS_INDEX);
    const float zAxisDistance = getDistanceFromAxis(point, Z_AXIS_INDEX);

    return (xAxisDistance * xAxisDistance) +
           (yAxisDistance * yAxisDistance) +
           (zAxisDistance * zAxisDistance);
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