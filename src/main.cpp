#include "Camera/Camera.hpp"
#include "Camera/CameraParameters.hpp"
#include "Framebuffer/Framebuffer.hpp"
#include "Hittable/IHittable.hpp"
#include "Hittable/Sphere/Sphere.hpp"
#include "Hittable/Triangle/Triangle.hpp"
#include "Renderer/NormalMapRenderer/NormalMapRenderer.hpp"
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

inline constexpr uint32_t SAMPLES_PER_PIXEL = 100;

int main() {
    RTC::PpmWriter writer {OUTPUT_IMAGE_PATH};
    RTC::NormalMapRenderer renderer {SAMPLES_PER_PIXEL};
    RTC::Framebuffer framebuffer {IMAGE_RESOLUTION};
    RTC::Camera camera {CAMERA_PARAMETERS};
    RTC::Scene scene {};

    std::unique_ptr<RTC::IHittable> sphere1 =
        std::make_unique<RTC::Sphere>(
            RTC::Point3<float> {0.0F, 0.0F, -1.0F}, 0.5F
        );

    std::unique_ptr<RTC::IHittable> sphere2 =
        std::make_unique<RTC::Sphere>(
            RTC::Point3<float> {1.5F, 0.5F, -3.0F}, 0.85F
        );

    std::unique_ptr<RTC::IHittable> sphere3 =
        std::make_unique<RTC::Sphere>(
            RTC::Point3<float> {0.0F, -100.5F, -1.0F}, 100.0F
        );

    std::unique_ptr<RTC::IHittable> triangle =
        std::make_unique<RTC::Triangle>(
            RTC::Point3<float> {-2.0F, -0.5F, -1.0F},
            RTC::Point3<float> {0.0F, -0.5F, -1.0F},
            RTC::Point3<float> {-1.0F, 0.7F, -1.5F}
        );

    scene.addObject(std::move(sphere1));
    scene.addObject(std::move(sphere2));
    scene.addObject(std::move(sphere3));
    scene.addObject(std::move(triangle));

    renderer.render(camera, scene, framebuffer);
    writer.write(framebuffer);

    return 0;
}