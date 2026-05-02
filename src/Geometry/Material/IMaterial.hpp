#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Material/MaterialSample.hpp"

namespace RTC {
class IMaterial {
public:
    IMaterial() = default;
    IMaterial(const IMaterial&) = delete;
    IMaterial(IMaterial&&) = delete;
    IMaterial operator=(const IMaterial&) = delete;
    IMaterial operator=(IMaterial&&) = delete;
    virtual ~IMaterial() = default;

    [[nodiscard]] virtual const LinearColor& getBaseColor() const = 0;

    [[nodiscard]] virtual const LinearColor& getEmission() const = 0;

    [[nodiscard]] virtual LinearColor getEmission(
        const Point3f& origin,
        const Vector3f& direction
    ) const = 0;

    [[nodiscard]] virtual LinearColor calculateBrdf(
        const Point3f& origin,
        const Vector3f& outDirection,
        const Vector3f& inDirection
    ) const = 0;

    [[nodiscard]] virtual MaterialSample getSample(
        const Point3f& origin,
        const Vector3f& normal,
        const Vector3f& outDirection
    ) const = 0;
};
}