#pragma once

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

    virtual void render(Framebuffer& framebuffer) noexcept = 0;
};
}