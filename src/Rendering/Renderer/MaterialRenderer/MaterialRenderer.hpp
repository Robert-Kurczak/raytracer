#pragma once

#include "../IRenderer.hpp"
#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/LightData.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/Renderer/MaterialRenderer/MaterialRendererParameters.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <memory>

namespace RTC {
class MaterialRenderer : public IRenderer {
private:
    std::shared_ptr<ILogger> logger_;
    MaterialRendererParameters parameters_;

    [[nodiscard]] Color<float> getSkyAttenuation(
        const Vector3<float>& rayDirectionVersor
    ) const;

    [[nodiscard]] bool isInShadow(
        const HitData& hitData,
        const LightData& lightData,
        const Scene& scene
    ) const;

    [[nodiscard]] Color<float> getDirectLight(
        const HitData& hitData,
        const Scene& scene
    ) const;

    [[nodiscard]] Color<float> getIndirectLight(
        Color<float>& attenuation,
        const Ray& ray,
        const Interval<float>& interval,
        const HitData& hitData,
        const Scene& scene,
        uint32_t& tracedRaysCount,
        uint32_t depth
    ) const;

    [[nodiscard]] Color<float> traceRay(
        const Ray& ray,
        const Scene& scene,
        const Interval<float>& interval,
        uint32_t& tracedRaysCount,
        uint32_t depth
    ) const;

    uint32_t renderSection(
        const Camera& camera,
        const Scene& scene,
        const Interval<float>& renderInterval,
        const Interval<uint32_t>& xIndices,
        const Interval<uint32_t>& yIndices,
        Framebuffer& framebuffer
    ) const;

public:
    MaterialRenderer(
        std::shared_ptr<ILogger> logger,
        MaterialRendererParameters parameters
    );

    void render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}