#pragma once

#include "Framebuffer/Framebuffer.hpp"
#include "Renderer/IRenderer.hpp"

namespace RTC {
class SampleRenderer : public IRenderer {
public:
    void render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}