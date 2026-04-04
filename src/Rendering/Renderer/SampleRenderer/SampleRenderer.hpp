#pragma once

#include "../IRenderer.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"

namespace RTC {
class SampleRenderer : public IRenderer {
public:
    RendererStatistics render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}