#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"
#include "World/Scene/Scene.hpp"

namespace RTC {
class ILightSampler {
public:
    ILightSampler() = default;
    ILightSampler(const ILightSampler&) = delete;
    ILightSampler(ILightSampler&&) = delete;
    ILightSampler operator=(const ILightSampler&) = delete;
    ILightSampler operator=(ILightSampler&&) = delete;
    virtual ~ILightSampler() = default;

    virtual void setup(const Scene& scene) = 0;

    virtual LinearColor getRadiance(
        const Scene& scene,
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection,
        RendererStatistics& statistics,
        uint32_t samplesToTake
    ) = 0;
};
}