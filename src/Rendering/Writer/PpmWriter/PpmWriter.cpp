#include "PpmWriter.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

#include <cmath>
#include <fstream>
#include <utility>

namespace RTC {
void PpmWriter::writePpmHeader(
    std::ofstream& fileStream,
    const Vector2<uint32_t>& resolution
) const {
    fileStream << "P3\n";
    fileStream << resolution.getX() << " " << resolution.getY() << "\n";
    fileStream << "255\n";
}

Color8Bit PpmWriter::castTo8Bit(LinearColor color) const {
    color.red = color.red / (color.red + 1.0F);
    color.green = color.green / (color.green + 1.0F);
    color.blue = color.blue / (color.blue + 1.0F);

    color.red = std::pow(color.red, 1.0F / 2.2F);
    color.green = std::pow(color.green, 1.0F / 2.2F);
    color.blue = std::pow(color.blue, 1.0F / 2.2F);

    color.red = std::clamp(color.red, 0.0F, 1.0F);
    color.green = std::clamp(color.green, 0.0F, 1.0F);
    color.blue = std::clamp(color.blue, 0.0F, 1.0F);

    color *= 255.0F;

    return {
        .red = uint8_t(color.red),
        .green = uint8_t(color.green),
        .blue = uint8_t(color.blue)
    };
}

PpmWriter::PpmWriter(std::filesystem::path imagePath) :
    imagePath_(std::move(imagePath)) {}

void PpmWriter::write(const Framebuffer& framebuffer) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();
    std::ofstream fileStream {imagePath_};

    writePpmHeader(fileStream, resolution);

    for (uint32_t yIndex = 0; yIndex < resolution.getY(); yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.getX(); xIndex++) {
            const LinearColor color =
                framebuffer.getColorAt({xIndex, yIndex});

            const Color8Bit castedColor = castTo8Bit(color);

            fileStream << int(castedColor.red) << " "
                       << int(castedColor.green) << " "
                       << int(castedColor.blue) << "\n";
        }
    }
}
}