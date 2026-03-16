#pragma once

#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/Writer/IWriter.hpp"

#include <filesystem>
#include <fstream>

namespace RTC {
class PpmWriter : public IWriter {
private:
    const std::filesystem::path imagePath_;

    void writePpmHeader(
        std::ofstream& fileStream,
        const Vector2<uint32_t>& resolution
    );

public:
    PpmWriter(std::filesystem::path imagePath);

    void write(const Framebuffer& framebuffer) noexcept override;
};
}