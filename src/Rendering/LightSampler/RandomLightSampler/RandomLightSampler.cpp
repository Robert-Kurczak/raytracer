#include "RandomLightSampler.hpp"

#include "Core/Math/Random.hpp"
#include "Geometry/Light/ILight.hpp"

#include <memory>

namespace RTC {
RandomLightSampler::RandomLightSampler(
    std::unique_ptr<IDirectLightEstimator> directLightEstimator
) :
    directLightEstimator_(std::move(directLightEstimator)) {}

LinearColor RandomLightSampler::getRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics,
    uint32_t samplesToTake
) {
    LinearColor radiance = LinearColor::black();

    const auto lightsAmount = float(scene.getLights().size());

    for (uint32_t i = 0; i < samplesToTake; i++) {
        const auto randomIndex =
            uint32_t(getRandomNumber<float>(0, lightsAmount));

        const std::unique_ptr<ILight>& light =
            scene.getLights().at(randomIndex);

        radiance +=
            directLightEstimator_->estimate(
                *light, scene, hitData, offsetHitPoint, outDirection
            ) *
            lightsAmount;
    }

    statistics.shadowRays += samplesToTake;

    return radiance / float(samplesToTake);
}
}