#pragma once

#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "World/Camera/Camera.hpp"
#include "World/Scene/Scene.hpp"

namespace RTC {
class IRenderer {
public:
    IRenderer() = default;
    IRenderer(const IRenderer&) = delete;
    IRenderer(IRenderer&&) = delete;
    void operator=(const IRenderer&) = delete;
    void operator=(IRenderer&&) = delete;
    virtual ~IRenderer() = default;

    virtual void render(
        const Camera& camera,
        const Scene& scene,
        Framebuffer& framebuffer
    ) noexcept = 0;
};
}