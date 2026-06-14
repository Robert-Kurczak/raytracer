#include "StbImageTextureBuilder.hpp"

#include "Rendering/Texture/ImageTexture/ImageTexture.hpp"

#include <stdexcept>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace RTC {
[[nodiscard]] std::shared_ptr<ITexture> StbImageTextureBuilder::
    buildFromFile(const std::filesystem::path& path) const {
    int width = -1;
    int height = -1;
    int channels = -1;

    float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

    if (data == nullptr) {
        throw std::runtime_error(
            "Could not load texture from: " + path.string()
        );
    }

    const uint32_t bufferSize = width * height * channels;

    std::vector<float> buffer(data, data + bufferSize);

    stbi_image_free(data);

    return std::make_shared<ImageTexture>(
        uint32_t(width),
        uint32_t(height),
        uint32_t(channels),
        std::move(buffer)
    );
}
}