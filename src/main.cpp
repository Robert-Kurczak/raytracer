#include "EnvironmentBuilder/JsonEnvironmentBuilder/JsonEnvironmentBuilder.hpp"
#include "MeshBuilder/ObjMeshBuilder.hpp"
#include "ProgressIndicator/CoutProgressIndicator/CoutProgressIndicator.hpp"

#include <filesystem>

int main(int argc, char* argv[]) {
    assert(argc > 1);

    const auto args = std::span(argv, size_t(argc));

    const std::filesystem::path configFilePath {args[1]};

    RTC::CoutProgressIndicator progressIndicator {};
    RTC::ObjMeshBuilder objMeshBuilder {};
    RTC::JsonEnvironmentBuilder environmentBuilder {
        progressIndicator, objMeshBuilder
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