#include "LightCutsSampler.hpp"

#include "Core/Color/Color.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Rendering/LightCutsTree/LightClusterMaxHeap.hpp"

#include <memory>

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

void LightCutsSampler::setup(const Scene& scene) {
    infiniteLights_.clear();

    std::vector<std::shared_ptr<ILight>> discreteLights;

    for (const std::shared_ptr<ILight>& light : scene.getLights()) {
        if (light->isInfinite()) {
            infiniteLights_.push_back(light);
        } else {
            light->discretize(
                discreteLights, parameters_.discreteSamplerPerLight
            );
        }
    }

    lightCutsTree_ =
        lightCutsTreeBuilder_->build(std::move(discreteLights));
}

LinearColor LightCutsSampler::getLightCutsRadiance(
    const Scene& scene,
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    RendererStatistics& statistics
) const {
    LightCutMaxHeap lightCut = lightCutsTree_->getCut(
        hitData.hitPoint, parameters_.maxError, parameters_.maxCutSize
    );

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
    LightCutsSamplerParameters parameters,
    std::unique_ptr<ILightCutsTreeBuilder> lightCutsTreeBuilder,
    std::unique_ptr<IDirectLightEstimator> directLightEstimator
) :
    parameters_(parameters),
    lightCutsTreeBuilder_(std::move(lightCutsTreeBuilder)),
    directLightEstimator_(std::move(directLightEstimator)) {}

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