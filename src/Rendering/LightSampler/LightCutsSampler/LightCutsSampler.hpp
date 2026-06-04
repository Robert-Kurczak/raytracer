#pragma once

#include "Rendering/LightSampler/ILightSampler.hpp"

namespace RTC {
class LightCutsSampler : public ILightSampler {
private:

public:
    LightCutsSampler();
    

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