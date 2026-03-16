#include "PpmWriter.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

#include <fstream>
#include <utility>

namespace RTC {
void PpmWriter::writePpmHeader(
    std::ofstream& fileStream,
    const Vector2<uint32_t>& resolution
) {
    fileStream << "P3\n";
    fileStream << resolution.getX() << " " << resolution.getY() << "\n";
    fileStream << "255\n";
}

PpmWriter::PpmWriter(std::filesystem::path imagePath) :
    imagePath_(std::move(imagePath)) {}

void PpmWriter::write(const Framebuffer& framebuffer) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();
    std::ofstream fileStream {imagePath_};

    writePpmHeader(fileStream, resolution);

    for (uint32_t yIndex = 0; yIndex < resolution.getY(); yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.getX(); xIndex++) {
            const Color8Bit color =
                framebuffer.getColorAt({xIndex, yIndex});

            fileStream << int(color.red) << " " << int(color.green) << " "
                       << int(color.blue) << "\n";
        }
    }
}
}