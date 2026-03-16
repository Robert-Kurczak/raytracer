#include "JsonEnvironmentBuilder.hpp"

#include "../RenderEnvironment.hpp"
#include "Geometry/Hittable/Sphere/Sphere.hpp"
#include "Rendering/Renderer/NormalMapRenderer/NormalMapRenderer.hpp"
#include "Rendering/Writer/PpmWriter/PpmWriter.hpp"
#include "World/Camera/CameraParameters.hpp"

#include <format>
#include <fstream>
#include <memory>
#include <stdexcept>

using json = nlohmann::json;

namespace RTC {
[[nodiscard]] Point3<float> JsonEnvironmentBuilder::parsePosition(
    const nlohmann::json& jsonArray
) const {
    return Point3<float> {
        jsonArray[0].get<float>(),
        jsonArray[1].get<float>(),
        jsonArray[2].get<float>()
    };
}

std::unique_ptr<IWriter> JsonEnvironmentBuilder::parseWriter(
    const nlohmann::json& jsonContent
) const {
    const std::string outputPath =
        jsonContent["outputPath"].get<std::string>();

    return std::make_unique<PpmWriter>(outputPath);
}

[[nodiscard]] std::unique_ptr<IRenderer> JsonEnvironmentBuilder::
    parseRenderer(const nlohmann::json& jsonContent) const {
    const uint32_t samplesPerPixel =
        jsonContent["renderer"]["samplesPerPixel"].get<uint32_t>();

    return std::make_unique<NormalMapRenderer>(
        progressIndicator_, samplesPerPixel
    );
}

[[nodiscard]] std::unique_ptr<Camera> JsonEnvironmentBuilder::parseCamera(
    const nlohmann::json& jsonContent
) const {
    const CameraParameters parameters {
        .screenWidth =
            jsonContent["camera"]["screenWidth"].get<uint32_t>(),
        .aspectRatio = jsonContent["camera"]["aspectRatio"].get<double>(),
        .center = parsePosition(jsonContent["camera"]["position"])
    };

    return std::make_unique<Camera>(parameters);
}

[[nodiscard]] std::unique_ptr<Scene> JsonEnvironmentBuilder::parseScene(
    const nlohmann::json& jsonContent
) const {
    std::unique_ptr<Scene> scene = std::make_unique<Scene>();

    const auto& objectsInScene = jsonContent["objects"];

    for (const auto& object : objectsInScene) {
        const std::string objectType = object["type"].get<std::string>();
        const Point3<float> objectPosition =
            parsePosition(object["position"]);

        if (objectType == "obj") {
            const std::string filePath =
                object["filePath"].get<std::string>();

            std::unique_ptr<Mesh> mesh =
                objMeshBuilder_.buildFromFile(filePath, objectPosition);

            scene->addObject(std::move(mesh));
        } else if (objectType == "sphere") {
            const float radius = object["radius"].get<float>();

            std::unique_ptr<Sphere> sphere =
                std::make_unique<Sphere>(objectPosition, radius);

            scene->addObject(std::move(sphere));
        }
    }

    return scene;
}

JsonEnvironmentBuilder::JsonEnvironmentBuilder(
    IProgressIndicator& progressIndicator,
    IMeshBuilder& objMeshBuilder
) :
    progressIndicator_(progressIndicator),
    objMeshBuilder_(objMeshBuilder) {}

RenderEnvironment JsonEnvironmentBuilder::build(
    const std::filesystem::path& path
) const {
    std::ifstream file {path};

    if (!file) {
        throw std::runtime_error(
            std::format("Could not open {}", path.c_str())
        );
    }

    const json jsonContent = json::parse(file);

    return RenderEnvironment {
        .writer = parseWriter(jsonContent),
        .renderer = parseRenderer(jsonContent),
        .camera = parseCamera(jsonContent),
        .scene = parseScene(jsonContent)
    };
}
}