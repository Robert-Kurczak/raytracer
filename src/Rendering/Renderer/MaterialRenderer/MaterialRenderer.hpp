#pragma once

#include "../IRenderer.hpp"
#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Material/IMaterial.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/ProgressIndicator/IProgressIndicator.hpp"

#include <memory>

namespace RTC {
class MaterialRenderer : public IRenderer {
private:
    IProgressIndicator& progressIndicator_;
    const uint32_t samplesPerPixel_;
    std::shared_ptr<IMaterial> defaultMaterial_;

    [[nodiscard]] Color<float> getSkyAttenuation(
        const Vector3<float>& rayDirectionVersor
    ) const;

    [[nodiscard]] Color<float> getAttenuationRecursively(
        const Ray& ray,
        const Scene& scene,
        const Interval<float>& interval,
        int32_t depth
    ) const;

    [[nodiscard]] Color<float> calculateColor(
        const Ray& ray,
        const HitData& hitData
    ) const;

public:
    MaterialRenderer(
        IProgressIndicator& progressIndicator,
        uint32_t samplesPerPixel,
        std::shared_ptr<IMaterial> defaultMaterial
    );

    void render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}