#include "ExrWriter.hpp"

#include "ImfPixelType.h"

#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfRgbaFile.h>
#include <cstddef>
#include <filesystem>
#include <half.h>
#include <stdexcept>
#include <utility>
#include <vector>

namespace RTC {
using PixelType = float;
static constexpr Imf::PixelType EXR_PIXEL_TYPE = Imf::FLOAT;

ExrWriter::ExrWriter(std::filesystem::path imagePath) :
    imagePath_(std::move(imagePath)) {
    const std::filesystem::path parentDirectory =
        imagePath_.parent_path();

    if (not std::filesystem::exists(parentDirectory)) {
        throw std::runtime_error(
            parentDirectory.string() + "directory does not exist."
        );
    }
}

void ExrWriter::write(const Framebuffer& framebuffer) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();

    const int width = int(resolution.getX());
    const int height = int(resolution.getY());
    const size_t pixelCount = size_t(width) * height;

    std::vector<PixelType> redChannel(pixelCount);
    std::vector<PixelType> greenChannel(pixelCount);
    std::vector<PixelType> blueChannel(pixelCount);

    for (uint32_t i = 0; i < pixelCount; i++) {
        redChannel[i] = framebuffer.getConstBuffer()[i].red;
        greenChannel[i] = framebuffer.getConstBuffer()[i].green;
        blueChannel[i] = framebuffer.getConstBuffer()[i].blue;
    }

    Imf::Header header(width, height);
    header.channels().insert("R", Imf::Channel(EXR_PIXEL_TYPE));
    header.channels().insert("G", Imf::Channel(EXR_PIXEL_TYPE));
    header.channels().insert("B", Imf::Channel(EXR_PIXEL_TYPE));

    Imf::OutputFile file {imagePath_.c_str(), header};

    Imf::FrameBuffer exrFramebuffer;
    exrFramebuffer.insert(
        "R",
        Imf::Slice(
            EXR_PIXEL_TYPE,
            reinterpret_cast<char*>(redChannel.data()), // NOLINT
            sizeof(PixelType),
            sizeof(PixelType) * width
        )
    );

    exrFramebuffer.insert(
        "G",
        Imf::Slice(
            EXR_PIXEL_TYPE,
            reinterpret_cast<char*>(greenChannel.data()), // NOLINT
            sizeof(float),
            sizeof(float) * width
        )
    );

    exrFramebuffer.insert(
        "B",
        Imf::Slice(
            EXR_PIXEL_TYPE,
            reinterpret_cast<char*>(blueChannel.data()), // NOLINT
            sizeof(float),
            sizeof(float) * width
        )
    );

    file.setFrameBuffer(exrFramebuffer);
    file.writePixels(height);
}
}