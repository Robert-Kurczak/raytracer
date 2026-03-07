#pragma once

#include "Color/Color.hpp"
#include "Framebuffer/Framebuffer.hpp"
#include "Hittable/HitData.hpp"
#include "Renderer/IRenderer.hpp"

namespace RTC {
class NormalMapRenderer : public IRenderer {
private:
    Color8Bit getSkyColor(const Vector3<float>& rayDirectionVersor);
    Color8Bit calculateColor(const HitData& hitData);

public:
    void render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}