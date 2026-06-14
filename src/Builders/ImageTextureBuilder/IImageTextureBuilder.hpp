#pragma once

#include "Rendering/Texture/ITexture.hpp"

#include <filesystem>

namespace RTC {
class IImageTextureBuilder {
public:
    IImageTextureBuilder() = default;
    IImageTextureBuilder(const IImageTextureBuilder&) = delete;
    IImageTextureBuilder(IImageTextureBuilder&&) = delete;
    IImageTextureBuilder& operator=(const IImageTextureBuilder&) = delete;
    IImageTextureBuilder& operator=(IImageTextureBuilder&&) = delete;
    virtual ~IImageTextureBuilder() = default;

    [[nodiscard]] virtual std::shared_ptr<ITexture> buildFromFile(
        const std::filesystem::path& path
    ) const = 0;
};
}