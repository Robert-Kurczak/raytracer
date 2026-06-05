#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/DirectLightEstimator/IDirectLightEstimator.hpp"
#include "Rendering/LightCutsTree/ILightCutsTree.hpp"
#include "Rendering/LightSampler/ILightSampler.hpp"

#include <vector>

namespace RTC {
class LightCutsSampler : public ILightSampler {
private:
    std::unique_ptr<IDirectLightEstimator> directLightEstimator_;
    std::unique_ptr<ILightCutsTree> lightCutsTree_;
    std::vector<std::shared_ptr<ILight>> infiniteLights_;

    [[nodiscard]] LinearColor getInfiniteLightsRadiance(
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection,
        RendererStatistics& statistics
    ) const;

    [[nodiscard]] LinearColor getLightCutsRadiance(
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection,
        RendererStatistics& statistics
    ) const;

public:
    LightCutsSampler(
        std::unique_ptr<IDirectLightEstimator> directLightEstimator,
        std::unique_ptr<ILightCutsTree> lightCutsTree,
        std::vector<std::shared_ptr<ILight>> infiniteLights
    );

    LinearColor getRadiance(
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection,
        RendererStatistics& statistics,
        uint32_t samplesToTake
    ) override;
};
}