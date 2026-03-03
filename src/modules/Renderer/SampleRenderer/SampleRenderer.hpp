#pragma once

#include "Framebuffer/Framebuffer.hpp"
#include "Renderer/IRenderer.hpp"

namespace RTC {
class SampleRenderer : public IRenderer {
public:
    void render(Framebuffer& framebuffer) noexcept override;
};
}