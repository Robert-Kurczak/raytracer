#pragma once

#include "Geometry/Light/ILight.hpp"
#include "Rendering/DirectLightEstimator/IDirectLightEstimator.hpp"
#include "Rendering/LightSampler/ILightSampler.hpp"

#include <memory>

namespace RTC {
class RandomLightSampler : public ILightSampler {
private:
    uint32_t samples_;
    std::unique_ptr<IDirectLightEstimator> directLightEstimator_;

public:
    RandomLightSampler(
        uint32_t samples,
        std::unique_ptr<IDirectLightEstimator> directLightEstimator
    );

    void setup(const Scene& scene) override;

    LinearColor getRadiance(
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection,
        RendererStatistics& statistics
    ) override;
};
}