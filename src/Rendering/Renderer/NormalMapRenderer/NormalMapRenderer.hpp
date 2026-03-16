#pragma once

#include "../IRenderer.hpp"
#include "Core/Color/Color.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/ProgressIndicator/IProgressIndicator.hpp"

namespace RTC {
class NormalMapRenderer : public IRenderer {
private:
    IProgressIndicator& progressIndicator_;
    const uint32_t samplesPerPixel_;

    Color<float> getSkyColor(const Vector3<float>& rayDirectionVersor);
    Color<float> calculateColor(const HitData& hitData);

public:
    NormalMapRenderer(
        IProgressIndicator& progressIndicator,
        uint32_t samplesPerPixel
    );

    void render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}