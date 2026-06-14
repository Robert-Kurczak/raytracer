#pragma once

#include "Builders/ImageTextureBuilder/IImageTextureBuilder.hpp"
#include "Utils/Logger/ILogger.hpp"

namespace RTC {
class StbImageTextureBuilder : public IImageTextureBuilder {
private:
    std::shared_ptr<ILogger> logger_;

public:
    StbImageTextureBuilder(std::shared_ptr<ILogger> logger);

    [[nodiscard]] std::shared_ptr<ITexture> buildFromFile(
        const std::filesystem::path& path
    ) const override;
};
}