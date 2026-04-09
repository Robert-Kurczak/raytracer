#include "PpmWriter.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

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
    const LinearColor mappedColor {
        .red = color.red / (color.red + 1.0F),
        .green = color.green / (color.green + 1.0F),
        .blue = color.blue / (color.blue + 1.0F)
    };

    const LinearColor scaledColor = mappedColor * 255.0F;

    return {
        .red = uint8_t(scaledColor.red),
        .green = uint8_t(scaledColor.green),
        .blue = uint8_t(scaledColor.blue)
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