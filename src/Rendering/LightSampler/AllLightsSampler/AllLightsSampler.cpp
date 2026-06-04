#include "Rendering/LightSampler/AllLightsSampler/AllLightsSampler.hpp"

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"

namespace RTC {
AllLightsSampler::AllLightsSampler(
    std::unique_ptr<IDirectLightEstimator> directLightEstimator
) :
    directLightEstimator_(std::move(directLightEstimator)) {}

LinearColor AllLightsSampler::getRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics,
    uint32_t samplesToTake
) {
    LinearColor radiance = LinearColor::black();

    for (const std::shared_ptr<ILight>& light : scene.getLights()) {
        for (uint32_t i = 0; i < samplesToTake; i++) {
            radiance += directLightEstimator_->estimate(
                *light, scene, hitData, offsetHitPoint, outDirection
            );
        }
    }

    statistics.shadowRays += scene.getLights().size() * samplesToTake;

    return radiance / float(samplesToTake);
}
}