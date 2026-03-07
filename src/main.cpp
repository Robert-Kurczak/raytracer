#include "Camera/Camera.hpp"
#include "Camera/CameraParameters.hpp"
#include "Framebuffer/Framebuffer.hpp"
#include "Hittable/IHittable.hpp"
#include "Hittable/Sphere/Sphere.hpp"
#include "Renderer/DefaultRenderer/DefaultRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Writer/PpmWriter/PpmWriter.hpp"

#include <filesystem>
#include <memory>

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
    RTC::Scene scene {};

    std::unique_ptr<RTC::IHittable> sphere =
        std::make_unique<RTC::Sphere>(
            RTC::Point3<float> {0.0F, 0.0F, -1.0F}, 0.5F
        );

    scene.addObject(std::move(sphere));

    renderer.render(camera, scene, framebuffer);
    writer.write(framebuffer);

    return 0;
}