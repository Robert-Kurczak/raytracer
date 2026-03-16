#pragma once

#include "Rendering/Renderer/IRenderer.hpp"
#include "Rendering/Writer/IWriter.hpp"
#include "World/Camera/Camera.hpp"
#include "World/Scene/Scene.hpp"

#include <memory>

namespace RTC {
struct RenderEnvironment {
    std::unique_ptr<IWriter> writer;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Scene> scene;
};
}