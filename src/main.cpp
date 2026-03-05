#include "Camera/Camera.hpp"
#include "Camera/CameraParameters.hpp"
#include "Framebuffer/Framebuffer.hpp"
#include "Renderer/DefaultRenderer/DefaultRenderer.hpp"
#include "Writer/PpmWriter/PpmWriter.hpp"

#include <filesystem>

inline constexpr std::string OUTPUT_IMAGE_PATH {"raytracing.ppm"};
inline constexpr RTC::Vector2<uint32_t> IMAGE_RESOLUTION {1920, 1080};

inline constexpr RTC::CameraParameters CAMERA_PARAMETERS {
    .screenWidth = IMAGE_RESOLUTION.getX(),
    .aspectRatio = 16.0F / 9.0F,
    .center = {0.0F, 0.0F, 0.0F},
};

int main() {
    RTC::PpmWriter writer {OUTPUT_IMAGE_PATH};
    RTC::DefaultRenderer renderer {};
    RTC::Framebuffer framebuffer {IMAGE_RESOLUTION};
    RTC::Camera camera {CAMERA_PARAMETERS};

    renderer.render(camera, framebuffer);
    writer.write(framebuffer);

    return 0;
}