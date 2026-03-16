#pragma once

#include "Builders/EnvironmentBuilder/IEnvironmentBuilder.hpp"
#include "Builders/MeshBuilder/IMeshBuilder.hpp"
#include "Core/Math/Point.hpp"
#include "Rendering/ProgressIndicator/IProgressIndicator.hpp"
#include "Rendering/Renderer/IRenderer.hpp"
#include "Rendering/Writer/IWriter.hpp"
#include "World/Camera/Camera.hpp"
#include "World/Scene/Scene.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace RTC {
class JsonEnvironmentBuilder : public IEnvironmentBuilder {
private:
    IProgressIndicator& progressIndicator_;
    IMeshBuilder& objMeshBuilder_;

    [[nodiscard]] Point3<float> parsePosition(
        const nlohmann::json& jsonArray
    ) const;

    [[nodiscard]] std::unique_ptr<IWriter> parseWriter(
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::unique_ptr<IRenderer> parseRenderer(
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::unique_ptr<Camera> parseCamera(
        const nlohmann::json& jsonContent
    ) const;

    [[nodiscard]] std::unique_ptr<Scene> parseScene(
        const nlohmann::json& jsonContent
    ) const;

public:
    JsonEnvironmentBuilder(
        IProgressIndicator& progressIndicator,
        IMeshBuilder& objMeshBuilder
    );

    [[nodiscard]] RenderEnvironment build(
        const std::filesystem::path& path
    ) const override;
};
}