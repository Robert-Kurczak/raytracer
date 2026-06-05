#pragma once

#include "Geometry/Light/LightSample.hpp"
#include "Rendering/DirectLightEstimator/IDirectLightEstimator.hpp"

namespace RTC {
class DirectLightEstimator : public IDirectLightEstimator {
private:
    [[nodiscard]] bool isInShadow(
        const Point3f& origin,
        const Vector3f& toLight,
        const Scene& scene
    ) const;

public:
    [[nodiscard]] LinearColor getScatteringWeight(
        const LightSample& lightSample,
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection
    ) const override;

    [[nodiscard]] LinearColor estimateRadiance(
        const ILight& light,
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection
    ) const override;
};
}