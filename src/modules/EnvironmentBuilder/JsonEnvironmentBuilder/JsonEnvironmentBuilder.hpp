#pragma once

#include "../IEnvironmentBuilder.hpp"
#include "MeshBuilder/IMeshBuilder.hpp"
#include "ProgressIndicator/IProgressIndicator.hpp"
#include "Renderer/IRenderer.hpp"
#include "Writer/IWriter.hpp"

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