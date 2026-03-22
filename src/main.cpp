#include "Builders/BvhBuilder/BvhBuilder.hpp"
#include "Builders/EnvironmentBuilder/JsonEnvironmentBuilder/JsonEnvironmentBuilder.hpp"
#include "Builders/MeshBuilder/ObjMeshBuilder/ObjMeshBuilder.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/ProgressIndicator/CoutProgressIndicator/CoutProgressIndicator.hpp"

#include <cassert>
#include <filesystem>
#include <span>

int main(int argc, char* argv[]) {
    assert(argc > 1);

    const auto args = std::span(argv, size_t(argc));

    const std::filesystem::path configFilePath {args[1]};

    RTC::CoutProgressIndicator progressIndicator {};
    RTC::ObjMeshBuilder objMeshBuilder {};
    RTC::BvhBuilder bvhBuilder {};
    RTC::JsonEnvironmentBuilder environmentBuilder {
        progressIndicator, objMeshBuilder, bvhBuilder
    };
    RTC::RenderEnvironment environment =
        environmentBuilder.build(configFilePath);
    RTC::Framebuffer framebuffer {environment.camera->getSceenSize()};

    environment.renderer->render(
        *environment.camera, *environment.scene, framebuffer
    );
    environment.writer->write(framebuffer);

    return 0;
}