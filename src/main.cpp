#include "Camera/Camera.hpp"
#include "Camera/CameraParameters.hpp"
#include "Framebuffer/Framebuffer.hpp"
#include "Hittable/IHittable.hpp"
#include "Hittable/Mesh/Mesh.hpp"
#include "Hittable/Sphere/Sphere.hpp"
#include "MeshBuilder/ObjMeshBuilder.hpp"
#include "Renderer/NormalMapRenderer/NormalMapRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Writer/PpmWriter/PpmWriter.hpp"

#include <filesystem>
#include <memory>

inline const std::string OUTPUT_IMAGE_PATH {"build/raytracing.ppm"};
inline const std::string OBJ_FILE_PATH {"resources/spot-rotated.obj"};

inline constexpr RTC::CameraParameters CAMERA_PARAMETERS {
    .screenWidth = 300,
    .aspectRatio = 16.0F / 9.0F,
    .center = {0.0F, 0.0F, 0.0F},
};

inline constexpr uint32_t SAMPLES_PER_PIXEL = 50;

int main() {
    RTC::PpmWriter writer {OUTPUT_IMAGE_PATH};
    RTC::Camera camera {CAMERA_PARAMETERS};
    RTC::Framebuffer framebuffer {camera.getSceenSize()};
    RTC::NormalMapRenderer renderer {SAMPLES_PER_PIXEL};
    RTC::Scene scene {};
    RTC::ObjMeshBuilder meshBuilder {};

    std::unique_ptr<RTC::IHittable> groundSphere =
        std::make_unique<RTC::Sphere>(
            RTC::Point3<float> {0.0F, -100.5F, -1.0F}, 100.0F
        );

    std::unique_ptr<RTC::Mesh> mesh =
        meshBuilder.buildFromFile(OBJ_FILE_PATH, {-0.5F, 0.0F, -2.0F});

    scene.addObject(std::move(groundSphere));
    scene.addObject(std::move(mesh));

    renderer.render(camera, scene, framebuffer);
    writer.write(framebuffer);

    return 0;
}