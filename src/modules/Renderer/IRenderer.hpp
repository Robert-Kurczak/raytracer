#pragma once

#include "Camera/Camera.hpp"
#include "Framebuffer/Framebuffer.hpp"

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
        Framebuffer& framebuffer
    ) noexcept = 0;
};
}