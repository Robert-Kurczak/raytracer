#pragma once

#include "Builders/BvhBuilder/IBvhBuilder.hpp"
#include "Builders/MeshBuilder/IMeshBuilder.hpp"
#include "Rendering/Renderer/IRenderer.hpp"
#include "Rendering/Writer/IWriter.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "World/Camera/Camera.hpp"
#include "World/Scene/Scene.hpp"

#include <memory>

namespace RTC {
struct RenderEnvironment {
    std::shared_ptr<ILogger> logger;
    std::unique_ptr<IMeshBuilder> meshBuilder;
    std::unique_ptr<IBvhBuilder> bvhBuilder;
    std::unique_ptr<IWriter> writer;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Scene> scene;
};
}