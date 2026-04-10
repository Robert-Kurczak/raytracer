#pragma once

#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/Writer/IWriter.hpp"

#include <filesystem>

namespace RTC {
class ExrWriter : public IWriter {
private:
    const std::filesystem::path imagePath_;

public:
    ExrWriter(std::filesystem::path imagePath);

    void write(const Framebuffer& framebuffer) noexcept override;
};
}