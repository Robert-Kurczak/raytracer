#pragma once

#include "Camera/Camera.hpp"
#include "Renderer/IRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Writer/IWriter.hpp"

#include <memory>

namespace RTC {
struct RenderEnvironment {
    std::unique_ptr<IWriter> writer;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Scene> scene;
};
}