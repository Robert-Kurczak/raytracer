#pragma once

#include "Color/Color.hpp"
#include "Framebuffer/Framebuffer.hpp"
#include "Hittable/HitData.hpp"
#include "Renderer/IRenderer.hpp"

namespace RTC {
class NormalMapRenderer : public IRenderer {
private:
    const uint32_t samplesPerPixel_;

    Color<float> getSkyColor(const Vector3<float>& rayDirectionVersor);
    Color<float> calculateColor(const HitData& hitData);

public:
    NormalMapRenderer(uint32_t samplesPerPixel);

    void render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}