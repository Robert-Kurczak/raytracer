#include "Builders/EnvironmentBuilder/JsonEnvironmentBuilder/JsonEnvironmentBuilder.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"

#include <cassert>
#include <filesystem>
#include <span>

int main(int argc, char* argv[]) {
    assert(argc > 1);

    const auto args = std::span(argv, size_t(argc));

    const std::filesystem::path configFilePath {args[1]};

    RTC::JsonEnvironmentBuilder environmentBuilder {};

    RTC::RenderEnvironment environment =
        environmentBuilder.build(configFilePath);

    RTC::Framebuffer framebuffer {environment.camera->getSceenSize()};

    environment.renderer->render(
        *environment.camera, *environment.scene, framebuffer
    );
    environment.writer->write(framebuffer);

    return 0;
}