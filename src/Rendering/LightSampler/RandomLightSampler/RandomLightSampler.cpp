#include "RandomLightSampler.hpp"

#include "Core/Math/Random.hpp"
#include "Geometry/Light/ILight.hpp"

#include <memory>

namespace RTC {
RandomLightSampler::RandomLightSampler(
    uint32_t samples,
    std::unique_ptr<IDirectLightEstimator> directLightEstimator
) :
    samples_(samples),
    directLightEstimator_(std::move(directLightEstimator)) {}

void RandomLightSampler::setup(const Scene& scene) {}

LinearColor RandomLightSampler::getRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics
) {
    LinearColor radiance = LinearColor::black();

    const auto lightsAmount = float(scene.getLights().size());

    for (uint32_t i = 0; i < samples_; i++) {
        const auto randomIndex =
            uint32_t(getRandomNumber<float>(0, lightsAmount));

        const std::shared_ptr<ILight>& light =
            scene.getLights().at(randomIndex);

        radiance +=
            directLightEstimator_->estimateRadiance(
                *light, scene, hitData, offsetHitPoint, outDirection
            ) *
            lightsAmount;
    }

    statistics.shadowRays += samples_;

    return radiance / float(samples_);
}
}