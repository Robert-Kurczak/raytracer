#include "LightCutsSampler.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Numeric.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Rendering/LightCutsTree/LightClusterMaxHeap.hpp"

namespace RTC {
LinearColor LightCutsSampler::getInfiniteLightsRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics
) const {
    LinearColor radiance = LinearColor::black();

    for (const std::shared_ptr<ILight>& light : infiniteLights_) {
        radiance += directLightEstimator_->estimateRadiance(
            *light, scene, hitData, offsetHitPoint, outDirection
        );
    }

    statistics.shadowRays += infiniteLights_.size();

    return radiance;
}

LinearColor LightCutsSampler::getLightCutsRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics
) const {
    constexpr float maxError = 0.02;
    constexpr uint32_t maxCutSize = 1024;

    LightCutMaxHeap lightCut =
        lightCutsTree_->getCut(hitData.hitPoint, maxError, maxCutSize);

    statistics.shadowRays += lightCut.size();

    LinearColor cumulativeRadiance = LinearColor::black();

    while (not lightCut.empty()) {
        const std::shared_ptr<LightNode> lightNode =
            lightCut.top().lightNode;

        const LightSample lightSample =
            lightNode->representative->getSample(hitData.hitPoint);

        const LinearColor scatteringWeight =
            directLightEstimator_->getScatteringWeight(
                lightSample, scene, hitData, offsetHitPoint, outDirection
            );

        const LinearColor radiance = scatteringWeight * lightNode->power;

        cumulativeRadiance += radiance;

        lightCut.pop();
    }

    return cumulativeRadiance;
}

LightCutsSampler::LightCutsSampler(
    std::unique_ptr<IDirectLightEstimator> directLightEstimator,
    std::unique_ptr<ILightCutsTree> lightCutsTree,
    std::vector<std::shared_ptr<ILight>> infiniteLights
) :
    directLightEstimator_(std::move(directLightEstimator)),
    lightCutsTree_(std::move(lightCutsTree)),
    infiniteLights_(std::move(infiniteLights)) {}

LinearColor LightCutsSampler::getRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics,
    uint32_t samplesToTake
) {
    const LinearColor infiniteLightsRadiance = getInfiniteLightsRadiance(
        scene, hitData, offsetHitPoint, outDirection, statistics
    );

    const LinearColor lightCutsRadiance = getLightCutsRadiance(
        scene, hitData, offsetHitPoint, outDirection, statistics
    );

    return infiniteLightsRadiance + lightCutsRadiance;
}
}