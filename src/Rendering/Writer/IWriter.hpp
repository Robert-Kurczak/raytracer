#pragma once

#include "Rendering/Framebuffer/Framebuffer.hpp"

namespace RTC {
class IWriter {
public:
    IWriter() = default;
    IWriter(const IWriter&) = delete;
    IWriter(IWriter&&) = delete;
    void operator=(const IWriter&) = delete;
    void operator=(IWriter&&) = delete;
    virtual ~IWriter() = default;

    virtual void write(const Framebuffer& framebuffer) noexcept = 0;
};
}