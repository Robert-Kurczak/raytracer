#pragma once

#include "Builders/BvhBuilder/IBvhBuilder.hpp"
#include "Builders/EnvironmentBuilder/IEnvironmentBuilder.hpp"
#include "Builders/MeshBuilder/IMeshBuilder.hpp"
#include "Core/Color/Color.hpp"
#include "Core/Math/Point.hpp"
#include "Geometry/Background/IBackground.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/Renderer/IRenderer.hpp"
#include "Rendering/Writer/IWriter.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "World/Camera/Camera.hpp"
#include "World/Scene/Scene.hpp"
#include "nlohmann/json_fwd.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace RTC {
class JsonEnvironmentBuilder : public IEnvironmentBuilder {
private:
    [[nodiscard]] std::shared_ptr<ILogger> parseLogger(
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] Point3<float> parsePosition(
        const nlohmann::json& jsonArray
    ) const;

    [[nodiscard]] LinearColor parse8BitColor(
        const nlohmann::json& jsonArray
    ) const;

    [[nodiscard]] LinearColor parseNormalizedColor(
        const nlohmann::json& jsonArray
    ) const;

    [[nodiscard]] std::unique_ptr<IBackground> parseBackground(
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::unique_ptr<IWriter> parseWriter(
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::unique_ptr<IRenderer> parseRenderer(
        const std::shared_ptr<ILogger>& logger,
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::unique_ptr<Camera> parseCamera(
        const nlohmann::json& jsonContent
    ) const;

    void parseObjects(
        Scene& scene,
        IMeshBuilder& meshBuilder,
        IBvhBuilder& bvhBuilder,
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::vector<std::unique_ptr<IHittable>>
    parseSceneObjects(
        IMeshBuilder& meshBuilder,
        IBvhBuilder& bvhBuilder,
        std::vector<std::unique_ptr<ILight>>& sceneLights,
        const nlohmann::json& jsonContent
    ) const;

    void parseSceneLights(
        std::vector<std::unique_ptr<ILight>>& sceneLights,
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::unique_ptr<Scene> parseScene(
        IMeshBuilder& meshBuilder,
        IBvhBuilder& bvhBuilder,
        const nlohmann::json& jsonContent
    ) const;

public:
    [[nodiscard]] RenderEnvironment build(
        const std::filesystem::path& path
    ) const override;
};
}