#include "Framebuffer/Framebuffer.hpp"
#include "Renderer/SampleRenderer/SampleRenderer.hpp"
#include "Writer/PpmWriter/PpmWriter.hpp"

#include <filesystem>

inline constexpr std::string OUTPUT_IMAGE_PATH {"raytracing.ppm"};
inline constexpr RTC::Vector2<uint32_t> IMAGE_RESOLUTION {
    .x = 1920,
    .y = 1080
};

int main() {
    RTC::PpmWriter writer {OUTPUT_IMAGE_PATH};
    RTC::SampleRenderer renderer {};
    RTC::Framebuffer framebuffer {IMAGE_RESOLUTION};

    renderer.render(framebuffer);
    writer.write(framebuffer);

    return 0;
}