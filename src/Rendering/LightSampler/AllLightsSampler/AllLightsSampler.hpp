#pragma once

#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/DirectLightEstimator/IDirectLightEstimator.hpp"
#include "Rendering/LightSampler/ILightSampler.hpp"

#include <memory>

namespace RTC {
class AllLightsSampler : public ILightSampler {
private:
    std::unique_ptr<IDirectLightEstimator> directLightEstimator_;

public:
    AllLightsSampler(
        std::unique_ptr<IDirectLightEstimator> directLightEstimator
    );

    void setup(const Scene& scene) override;

    [[nodiscard]] LinearColor getRadiance(
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection,
        RendererStatistics& statistics,
        uint32_t samplesToTake
    ) override;
};
}