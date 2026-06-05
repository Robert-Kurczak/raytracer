#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/ILight.hpp"

namespace RTC {
class IDirectLightEstimator {
public:
    IDirectLightEstimator() = default;
    IDirectLightEstimator(const IDirectLightEstimator&) = delete;
    IDirectLightEstimator(IDirectLightEstimator&&) = delete;
    IDirectLightEstimator operator=(const IDirectLightEstimator&) =
        delete;
    IDirectLightEstimator operator=(IDirectLightEstimator&&) = delete;
    virtual ~IDirectLightEstimator() = default;

    [[nodiscard]] virtual LinearColor getScatteringWeight(
        const LightSample& lightSample,
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection
    ) const = 0;

    [[nodiscard]] virtual LinearColor estimateRadiance(
        const ILight& light,
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection
    ) const = 0;
};
}