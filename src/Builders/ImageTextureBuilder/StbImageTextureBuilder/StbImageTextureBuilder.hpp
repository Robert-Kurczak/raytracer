#pragma once

#include "Builders/ImageTextureBuilder/IImageTextureBuilder.hpp"

namespace RTC {
class StbImageTextureBuilder : public IImageTextureBuilder {
public:
    [[nodiscard]] std::shared_ptr<ITexture> buildFromFile(
        const std::filesystem::path& path
    ) const override;
};
}