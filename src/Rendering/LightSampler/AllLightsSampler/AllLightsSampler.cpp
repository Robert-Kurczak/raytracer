#include "Rendering/LightSampler/AllLightsSampler/AllLightsSampler.hpp"

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"

namespace RTC {
AllLightsSampler::AllLightsSampler(
    uint32_t samples,
    std::unique_ptr<IDirectLightEstimator> directLightEstimator
) :
    samples_(samples),
    directLightEstimator_(std::move(directLightEstimator)) {}

void AllLightsSampler::setup(const Scene& scene) {}

LinearColor AllLightsSampler::getRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics
) {
    LinearColor radiance = LinearColor::black();

    for (const std::shared_ptr<ILight>& light : scene.getLights()) {
        for (uint32_t i = 0; i < samples_; i++) {
            radiance += directLightEstimator_->estimateRadiance(
                *light, scene, hitData, offsetHitPoint, outDirection
            );
        }
    }

    statistics.shadowRays += scene.getLights().size() * samples_;

    return radiance / float(samples_);
}
}