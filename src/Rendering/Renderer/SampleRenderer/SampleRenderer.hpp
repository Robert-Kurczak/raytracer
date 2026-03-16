#pragma once

#include "../IRenderer.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"

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