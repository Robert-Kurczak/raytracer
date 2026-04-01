#include "JsonEnvironmentBuilder.hpp"

#include "../RenderEnvironment.hpp"
#include "Builders/BvhBuilder/BvhBuilder.hpp"
#include "Builders/BvhBuilder/IBvhBuilder.hpp"
#include "Builders/MeshBuilder/IMeshBuilder.hpp"
#include "Builders/MeshBuilder/ObjMeshBuilder/ObjMeshBuilder.hpp"
#include "Core/Color/Color.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Hittable/Sphere/Sphere.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/PointLight/PointLight.hpp"
#include "Geometry/Material/MtlMaterial/MtlMaterial.hpp"
#include "Rendering/Renderer/MaterialRenderer/MaterialRenderer.hpp"
#include "Rendering/Renderer/MaterialRenderer/MaterialRendererParameters.hpp"
#include "Rendering/Writer/PpmWriter/PpmWriter.hpp"
#include "Utils/Logger/CoutLogger/CoutLogger.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "Utils/Logger/NullLogger/NullLogger.hpp"
#include "World/Camera/CameraParameters.hpp"

#include <format>
#include <fstream>
#include <memory>
#include <stdexcept>

using json = nlohmann::json;

namespace RTC {
static constexpr MtlParameters DEFAULT_MATERIAL_PARAMETERS {
    .ambient = {.red = 0.0F, .green = 0.0F, .blue = 0.0F},
    .diffuse = {.red = 0.8F, .green = 0.6F, .blue = 0.8F},
    .specular = {.red = 0.0F, .green = 0.0F, .blue = 0.0F},
    .shininess = 0.0F,
    .transparency = 0.0F
};

std::shared_ptr<ILogger> JsonEnvironmentBuilder::parseLogger(
    const nlohmann::json& jsonContent
) const {
    const bool verbose = jsonContent["verbose"].get<bool>();

    if (verbose) {
        return std::make_shared<CoutLogger>();
    }

    return std::make_shared<NullLogger>();
}

Point3<float> JsonEnvironmentBuilder::parsePosition(
    const nlohmann::json& jsonArray
) const {
    return Point3<float> {
        jsonArray[0].get<float>(),
        jsonArray[1].get<float>(),
        jsonArray[2].get<float>()
    };
}

Color8Bit JsonEnvironmentBuilder::parse8BitColor(
    const nlohmann::json& jsonArray
) const {
    return Color8Bit {
        jsonArray[0].get<uint8_t>(),
        jsonArray[1].get<uint8_t>(),
        jsonArray[2].get<uint8_t>()
    };
}

std::unique_ptr<IWriter> JsonEnvironmentBuilder::parseWriter(
    const nlohmann::json& jsonContent
) const {
    const std::string outputPath =
        jsonContent["outputPath"].get<std::string>();

    return std::make_unique<PpmWriter>(outputPath);
}

std::unique_ptr<IRenderer> JsonEnvironmentBuilder::parseRenderer(
    const nlohmann::json& jsonContent
) const {
    const MaterialRendererParameters parameters {
        .samplesPerPixel =
            jsonContent["renderer"]["samplesPerPixel"].get<uint32_t>(),
        .scatterRecursionDepth =
            jsonContent["renderer"]["scatterRecursionDepth"]
                .get<uint32_t>(),
        .defaultMaterial_ =
            std::make_shared<MtlMaterial>(DEFAULT_MATERIAL_PARAMETERS)
    };

    return std::make_unique<MaterialRenderer>(parameters);
}

std::unique_ptr<Camera> JsonEnvironmentBuilder::parseCamera(
    const nlohmann::json& jsonContent
) const {
    const CameraParameters parameters {
        .screenWidth =
            jsonContent["camera"]["screenWidth"].get<uint32_t>(),
        .screenHeight =
            jsonContent["camera"]["screenHeight"].get<uint32_t>(),
        .position = parsePosition(jsonContent["camera"]["position"]),
        .direction = parsePosition(jsonContent["camera"]["direction"]),
        .upDirection =
            parsePosition(jsonContent["camera"]["upDirection"]),
        .fieldOfViewDegrees =
            jsonContent["camera"]["fieldOfViewDegrees"].get<float>()
    };

    return std::make_unique<Camera>(parameters);
}

void JsonEnvironmentBuilder::parseObjects(
    Scene& scene,
    IMeshBuilder& meshBuilder,
    IBvhBuilder& bvhBuilder,
    const nlohmann::json& jsonContent
) const {
    const auto& objectsInScene = jsonContent["objects"];

    for (const auto& object : objectsInScene) {
        const std::string objectType = object["type"].get<std::string>();
        const Point3<float> objectPosition =
            parsePosition(object["position"]);

        if (objectType == "obj") {
            const std::string filePath =
                object["filePath"].get<std::string>();

            IMeshBuilder::TriangleBuffer mesh =
                meshBuilder.buildFromFile(filePath, objectPosition);

            std::unique_ptr<IHittable> bvhMesh =
                bvhBuilder.build(std::move(mesh));

            scene.addObject(std::move(bvhMesh));
        } else if (objectType == "sphere") {
            const float radius = object["radius"].get<float>();

            std::unique_ptr<Sphere> sphere =
                std::make_unique<Sphere>(objectPosition, radius);

            scene.addObject(std::move(sphere));
        }
    }
}

void JsonEnvironmentBuilder::parseLights(
    Scene& scene,
    const nlohmann::json& jsonContent
) const {
    const auto& lightsInScene = jsonContent["lights"];

    for (const auto& light : lightsInScene) {
        const std::string objectType = light["type"].get<std::string>();

        if (objectType == "point") {
            const Point3<float> position =
                parsePosition(light["position"]);

            const Color8Bit color = parse8BitColor(light["color"]);

            const float decay = light["decay"].get<float>();

            std::unique_ptr<ILight> pointLight =
                std::make_unique<PointLight>(position, color, decay);

            scene.addLight(std::move(pointLight));
        }
    }
}

std::unique_ptr<Scene> JsonEnvironmentBuilder::parseScene(
    IMeshBuilder& meshBuilder,
    IBvhBuilder& bvhBuilder,
    const nlohmann::json& jsonContent
) const {
    std::unique_ptr<Scene> scene = std::make_unique<Scene>();

    parseObjects(*scene, meshBuilder, bvhBuilder, jsonContent);
    parseLights(*scene, jsonContent);

    return scene;
}

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

    std::shared_ptr<ILogger> logger = parseLogger(jsonContent);

    std::unique_ptr<IMeshBuilder> meshBuilder =
        std::make_unique<ObjMeshBuilder>(logger);

    std::unique_ptr<IBvhBuilder> bvhBuilder =
        std::make_unique<BvhBuilder>(logger);

    std::unique_ptr<Scene> scene =
        parseScene(*meshBuilder, *bvhBuilder, jsonContent);

    return RenderEnvironment {
        .logger = std::move(logger),
        .meshBuilder = std::move(meshBuilder),
        .bvhBuilder = std::move(bvhBuilder),
        .writer = parseWriter(jsonContent),
        .renderer = parseRenderer(jsonContent),
        .camera = parseCamera(jsonContent),
        .scene = std::move(scene)
    };
}
}