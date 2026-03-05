#pragma once

#include "Color/Color.hpp"
#include "Framebuffer/Framebuffer.hpp"
#include "Renderer/IRenderer.hpp"

namespace RTC {
class DefaultRenderer : public IRenderer {
private:
    Color8Bit getSkyColor(const Vector3<float>& rayDirectionVersor);
    Color8Bit calculateColor(Ray& ray);

public:
    void render(
        const Camera& camera,
        Framebuffer& framebuffer
    ) noexcept override;
};
}