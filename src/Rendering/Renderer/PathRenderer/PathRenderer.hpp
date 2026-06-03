#pragma once

#include "../IRenderer.hpp"
#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Geometry/Background/IBackground.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/LightSampler/ILightSampler.hpp"
#include "Rendering/Material/MaterialSample.hpp"
#include "Rendering/ProgressIndicator/IProgressIndicator.hpp"
#include "Rendering/Renderer/PathRenderer/PathRendererParameters.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "World/Scene/Scene.hpp"

#include <memory>

namespace RTC {
class PathRenderer : public IRenderer {
private:
    std::shared_ptr<ILogger> logger_;
    std::unique_ptr<ILightSampler> lightSampler_;
    std::unique_ptr<IProgressIndicator> progressIndicator_;
    std::unique_ptr<IBackground> background_;
    PathRendererParameters parameters_;

    [[nodiscard]] LinearColor getEmission(
        const HitData& hitData,
        const Vector3f& outDirection,
        uint32_t recursionDepth
    ) const;

    [[nodiscard]] LinearColor getIndirectLight(
        const HitData& hitData,
        const Point3f& offsetHitPoint,
        const Vector3f& outDirection,
        const Scene& scene,
        RendererStatistics& statistics,
        uint32_t recursionDepth
    ) const;

    [[nodiscard]] LinearColor traceRay(
        const Ray& ray,
        const Scene& scene,
        RendererStatistics& statistics,
        uint32_t recursionDepth = 0
    ) const;

    RendererStatistics renderSection(
        const Camera& camera,
        const Scene& scene,
        const Interval<float>& renderInterval,
        const Interval<uint32_t>& xIndices,
        const Interval<uint32_t>& yIndices,
        Framebuffer& framebuffer
    ) const;

    std::vector<RendererStatistics> renderAll(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) const;

public:
    PathRenderer(
        std::shared_ptr<ILogger> logger,
        std::unique_ptr<ILightSampler> lightSampler,
        std::unique_ptr<IProgressIndicator> progressIndicator,
        std::unique_ptr<IBackground> background,
        PathRendererParameters parameters
    );

    RendererStatistics render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}