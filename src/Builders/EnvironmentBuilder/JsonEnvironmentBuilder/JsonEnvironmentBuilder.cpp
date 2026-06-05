#include "JsonEnvironmentBuilder.hpp"

#include "../RenderEnvironment.hpp"
#include "Builders/BvhBuilder/BvhBuilder.hpp"
#include "Builders/BvhBuilder/IBvhBuilder.hpp"
#include "Builders/LightCutsTreeBuilder/ILightCutsTreeBuilder.hpp"
#include "Builders/LightCutsTreeBuilder/LightCutsTreeBuilder.hpp"
#include "Builders/MeshBuilder/IMeshBuilder.hpp"
#include "Builders/MeshBuilder/MeshBuilderResult.hpp"
#include "Builders/MeshBuilder/ObjMeshBuilder/ObjMeshBuilder.hpp"
#include "Builders/PhotonMapBuilder/IPhotonMapBuilder.hpp"
#include "Builders/PhotonMapBuilder/PhotonMapBuilder.hpp"
#include "Core/Color/Color.hpp"
#include "Geometry/Background/IBackground.hpp"
#include "Geometry/Background/SkyBackground/SkyBackground.hpp"
#include "Geometry/Background/SolidBackground/SolidBackground.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Hittable/Sphere/Sphere.hpp"
#include "Geometry/Light/DirectionalLight/DirectionalLight.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/PointLight/PointLight.hpp"
#include "Rendering/DirectLightEstimator/DirectLightEstimator.hpp"
#include "Rendering/DirectLightEstimator/IDirectLightEstimator.hpp"
#include "Rendering/LightSampler/AllLightsSampler/AllLightsSampler.hpp"
#include "Rendering/LightSampler/ILightSampler.hpp"
#include "Rendering/LightSampler/LightCutsSampler/LightCutsSampler.hpp"
#include "Rendering/LightSampler/LightCutsSampler/LightCutsSamplerParameters.hpp"
#include "Rendering/LightSampler/RandomLightSampler/RandomLightSampler.hpp"
#include "Rendering/ProgressIndicator/CoutProgressIndicator/CoutProgressIndicator.hpp"
#include "Rendering/ProgressIndicator/IProgressIndicator.hpp"
#include "Rendering/Renderer/IRenderer.hpp"
#include "Rendering/Renderer/PathRenderer/PathRenderer.hpp"
#include "Rendering/Renderer/PathRenderer/PathRendererParameters.hpp"
#include "Rendering/Renderer/PhotonMapRenderer/PhotonMapRenderer.hpp"
#include "Rendering/Renderer/PhotonMapRenderer/PhotonMapRendererParameters.hpp"
#include "Rendering/Writer/ExrWriter/ExrWriter.hpp"
#include "Rendering/Writer/PpmWriter/PpmWriter.hpp"
#include "Utils/Logger/CoutLogger/CoutLogger.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "Utils/Logger/NullLogger/NullLogger.hpp"
#include "World/Camera/CameraParameters.hpp"
#include "nlohmann/json_fwd.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <stdexcept>

using json = nlohmann::json;

namespace RTC {
static constexpr LinearColor DEFAULT_BACKGROUND_COLOR {
    .red = 0.3F,
    .green = 0.3F,
    .blue = 0.3F
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

LinearColor JsonEnvironmentBuilder::parse8BitColor(
    const nlohmann::json& jsonArray
) const {
    return LinearColor {
               .red = jsonArray[0].get<float>(),
               .green = jsonArray[1].get<float>(),
               .blue = jsonArray[2].get<float>(),
           } /
           255.0F;
}

LinearColor JsonEnvironmentBuilder::parseNormalizedColor(
    const nlohmann::json& jsonArray
) const {
    return LinearColor {
        .red = float(jsonArray[0].get<uint8_t>()) / 255.0F,
        .green = float(jsonArray[1].get<uint8_t>()) / 255.0F,
        .blue = float(jsonArray[2].get<uint8_t>()) / 255.0F
    };
}

std::unique_ptr<IBackground> JsonEnvironmentBuilder::parseBackground(
    const nlohmann::json& jsonContent
) const {
    const std::string backgroundType =
        jsonContent["background"]["type"].get<std::string>();

    if (backgroundType == "sky") {
        return std::make_unique<SkyBackground>();
    }

    if (backgroundType == "solid") {
        const LinearColor color =
            parseNormalizedColor(jsonContent["background"]["color"]);

        return std::make_unique<SolidBackground>(color);
    }

    return std::make_unique<SolidBackground>(DEFAULT_BACKGROUND_COLOR);
}

std::unique_ptr<IWriter> JsonEnvironmentBuilder::parseWriter(
    const nlohmann::json& jsonContent
) const {
    const std::filesystem::path outputPath =
        jsonContent["outputPath"].get<std::filesystem::path>();

    const std::filesystem::path fileFormat = outputPath.extension();

    if (fileFormat == ".exr") {
        return std::make_unique<ExrWriter>(outputPath);
    }

    return std::make_unique<PpmWriter>(outputPath);
}

std::unique_ptr<ILightSampler> JsonEnvironmentBuilder::parseLightSampler(
    const std::shared_ptr<ILogger>& logger,
    const nlohmann::json& jsonContent
) const {
    std::unique_ptr<IDirectLightEstimator> directLightEstimator =
        std::make_unique<DirectLightEstimator>();

    const std::string type =
        jsonContent["lightSampler"]["type"].get<std::string>();

    if (type == "lightCuts") {
        LightCutsSamplerParameters parameters {
            .discreteSamplerPerLight =
                jsonContent["lightSampler"]["discreteSamplerPerLight"]
                    .get<uint32_t>(),
            .maxError =
                jsonContent["lightSampler"]["maxError"].get<float>(),
            .maxCutSize =
                jsonContent["lightSampler"]["maxCutSize"].get<uint32_t>()
        };

        std::unique_ptr<ILightCutsTreeBuilder> treeBuilder =
            std::make_unique<LightCutsTreeBuilder>(logger);

        return std::make_unique<LightCutsSampler>(
            parameters,
            std::move(treeBuilder),
            std::move(directLightEstimator)
        );
    }

    const uint32_t samples =
        jsonContent["lightSampler"]["samples"].get<uint32_t>();

    if (type == "random") {
        return std::make_unique<RandomLightSampler>(
            samples, std::move(directLightEstimator)
        );
    }

    return std::make_unique<AllLightsSampler>(
        samples, std::move(directLightEstimator)
    );
}

std::unique_ptr<IRenderer> JsonEnvironmentBuilder::parseRenderer(
    const std::shared_ptr<ILogger>& logger,
    const nlohmann::json& jsonContent
) const {
    std::unique_ptr<ILightSampler> lightSampler =
        parseLightSampler(logger, jsonContent);

    std::unique_ptr<IProgressIndicator> progressIndicator =
        std::make_unique<CoutProgressIndicator>();

    std::unique_ptr<IBackground> background =
        parseBackground(jsonContent);

    const bool photonMappingEnabled =
        jsonContent["renderer"]["photonMappingEnabled"].get<bool>();

    if (photonMappingEnabled) {
        const PhotonMapRendererParameters parameters {
            .pathsPerPixel =
                jsonContent["renderer"]["pathsPerPixel"].get<uint32_t>(),
            .scatterRecursionDepth =
                jsonContent["renderer"]["scatterRecursionDepth"]
                    .get<uint32_t>(),
            .emittedPhotons =
                jsonContent["renderer"]["emittedPhotons"].get<uint32_t>(),
            .nearestPhotons =
                jsonContent["renderer"]["nearestPhotons"].get<uint32_t>(),
        };

        std::unique_ptr<IPhotonMapBuilder> photonMapBuilder =
            std::make_unique<PhotonMapBuilder>(logger);

        return std::make_unique<PhotonMapRenderer>(
            logger,
            std::move(lightSampler),
            std::move(progressIndicator),
            std::move(photonMapBuilder),
            std::move(background),
            parameters
        );
    }

    const PathRendererParameters parameters {
        .pathsPerPixel =
            jsonContent["renderer"]["pathsPerPixel"].get<uint32_t>(),
        .scatterRecursionDepth =
            jsonContent["renderer"]["scatterRecursionDepth"]
                .get<uint32_t>(),
    };

    return std::make_unique<PathRenderer>(
        logger,
        std::move(lightSampler),
        std::move(progressIndicator),
        std::move(background),
        parameters
    );
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

std::vector<std::unique_ptr<IHittable>> JsonEnvironmentBuilder::
    parseSceneObjects(
        IMeshBuilder& meshBuilder,
        IBvhBuilder& bvhBuilder,
        std::vector<std::shared_ptr<ILight>>& sceneLights,
        const nlohmann::json& jsonContent
    ) const {
    const auto& jsonSceneObjects = jsonContent["objects"];
    std::vector<std::unique_ptr<IHittable>> sceneObjects;

    for (const auto& jsonObject : jsonSceneObjects) {
        const std::string objectType =
            jsonObject["type"].get<std::string>();

        const Point3<float> objectPosition =
            parsePosition(jsonObject["position"]);

        if (objectType == "obj") {
            const std::string filePath =
                jsonObject["filePath"].get<std::string>();

            MeshBuilderResult meshParsingResult =
                meshBuilder.buildFromFile(filePath, objectPosition);

            std::unique_ptr<IHittable> bvhMesh =
                bvhBuilder.build(std::move(meshParsingResult.triangles));

            sceneObjects.emplace_back(std::move(bvhMesh));

            std::ranges::move(
                meshParsingResult.areaLights,
                std::back_inserter(sceneLights)
            );

        } else if (objectType == "sphere") {
            const float radius = jsonObject["radius"].get<float>();

            std::unique_ptr<Sphere> sphere =
                std::make_unique<Sphere>(objectPosition, radius);

            sceneObjects.emplace_back(std::move(sphere));
        }
    }

    return sceneObjects;
}

void JsonEnvironmentBuilder::parseSceneLights(
    std::vector<std::shared_ptr<ILight>>& sceneLights,
    const nlohmann::json& jsonContent
) const {
    const auto& jsonSceneLights = jsonContent["lights"];

    for (const auto& jsonLight : jsonSceneLights) {
        const std::string objectType =
            jsonLight["type"].get<std::string>();

        if (objectType == "point") {
            const Point3f position = parsePosition(jsonLight["position"]);

            const LinearColor color = parse8BitColor(jsonLight["color"]);

            const float decay = jsonLight["decay"].get<float>();

            std::shared_ptr<ILight> pointLight =
                std::make_shared<PointLight>(position, color, decay);

            sceneLights.emplace_back(std::move(pointLight));
        } else if (objectType == "directional") {
            const LinearColor color = parse8BitColor(jsonLight["color"]);

            const Vector3f direction =
                parsePosition(jsonLight["direction"]).getNormalized();

            std::shared_ptr<ILight> directionalLight =
                std::make_shared<DirectionalLight>(color, direction);

            sceneLights.emplace_back(std::move(directionalLight));
        }
    }
}

std::unique_ptr<Scene> JsonEnvironmentBuilder::parseScene(
    IMeshBuilder& meshBuilder,
    IBvhBuilder& bvhBuilder,
    const nlohmann::json& jsonContent
) const {
    std::vector<std::shared_ptr<ILight>> sceneLights {};
    auto objects = parseSceneObjects(
        meshBuilder, bvhBuilder, sceneLights, jsonContent
    );

    parseSceneLights(sceneLights, jsonContent);

    std::unique_ptr<IHittable> sceneRoot =
        bvhBuilder.build(std::move(objects));

    auto scene = std::make_unique<Scene>(
        std::move(sceneRoot), std::move(sceneLights)
    );

    scene->setup();

    return std::move(scene);
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

    std::unique_ptr<IRenderer> renderer =
        parseRenderer(logger, jsonContent);

    std::unique_ptr<Scene> scene =
        parseScene(*meshBuilder, *bvhBuilder, jsonContent);

    return RenderEnvironment {
        .logger = std::move(logger),
        .meshBuilder = std::move(meshBuilder),
        .bvhBuilder = std::move(bvhBuilder),
        .writer = parseWriter(jsonContent),
        .renderer = std::move(renderer),
        .camera = parseCamera(jsonContent),
        .scene = std::move(scene)
    };
}
}