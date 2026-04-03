#pragma once

#include "Core/Math/Interval.hpp"
#include "Core/Math/Point.hpp"
#include "Core/Ray/Ray.hpp"

#include <cstdint>

namespace RTC {
class AxisAlignedBoundingBox {
private:
    std::array<Interval<float>, 3> axisIntervals_ {
        Interval<float> {0, 0},
        Interval<float> {0, 0},
        Interval<float> {0, 0}
    };

    void padIntervals();

public:
    AxisAlignedBoundingBox(
        const Interval<float>& xInterval,
        const Interval<float>& yInterval,
        const Interval<float>& zInterval
    );

    AxisAlignedBoundingBox(
        const Point3<float>& startPoint,
        const Point3<float>& endPoint
    );

    AxisAlignedBoundingBox(
        const AxisAlignedBoundingBox& box1,
        const AxisAlignedBoundingBox& box2
    );

    [[nodiscard]] const Interval<float>& getXAxisInterval() const;
    [[nodiscard]] const Interval<float>& getYAxisInterval() const;
    [[nodiscard]] const Interval<float>& getZAxisInterval() const;

    [[nodiscard]] float getIntervalCenter(uint8_t axisIndex) const;
    [[nodiscard]] uint8_t getLongestAxisIndex() const;

    [[nodiscard]] bool isHit(
        const Ray& ray,
        const Interval<float>& rayInterval
    ) const;
};
}