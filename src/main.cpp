#include "Builders/EnvironmentBuilder/JsonEnvironmentBuilder/JsonEnvironmentBuilder.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <span>

void logStatistics(
    RTC::ILogger& logger,
    const RTC::RendererStatistics& statistics,
    uint32_t renderTimeMilliseconds,
    uint32_t totalTimeMilliseconds
) {
    logger.log(
        RTC::LogLevel::Info,
        std::format("Traced rays: {}", statistics.rays)
    );

    logger.log(
        RTC::LogLevel::Info,
        std::format("Traced shadow rays: {}", statistics.shadowRays)
    );

    logger.log(
        RTC::LogLevel::Info,
        std::format("Tracing time: {}ms", renderTimeMilliseconds)
    );

    logger.log(
        RTC::LogLevel::Info,
        std::format("Total execution time: {}ms", totalTimeMilliseconds)
    );

    const float tracingRaysPerSecond =
        (float(statistics.rays) / float(renderTimeMilliseconds)) *
        1000.0F;

    logger.log(
        RTC::LogLevel::Info,
        std::format(
            "Rays per second (tracing time): {} [rays/s]",
            tracingRaysPerSecond
        )
    );

    const float totalRaysPerSecond =
        (float(statistics.rays) / float(totalTimeMilliseconds)) * 1000.0F;

    logger.log(
        RTC::LogLevel::Info,
        std::format(
            "Rays per second (total time): {} [rays/s]",
            totalRaysPerSecond
        )
    );
}

int main(int argc, char* argv[]) {
    assert(argc > 1);

    const auto totalStartTime = std::chrono::high_resolution_clock::now();

    const auto args = std::span(argv, size_t(argc));

    const std::filesystem::path configFilePath {args[1]};

    RTC::JsonEnvironmentBuilder environmentBuilder {};

    RTC::RenderEnvironment environment =
        environmentBuilder.build(configFilePath);

    RTC::Framebuffer framebuffer {environment.camera->getSceenSize()};

    const auto renderStartTime =
        std::chrono::high_resolution_clock::now();

    const RTC::RendererStatistics statistics =
        environment.renderer->render(
            *environment.camera, *environment.scene, framebuffer
        );

    const auto renderEndTime = std::chrono::high_resolution_clock::now();

    const auto renderTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            renderEndTime - renderStartTime
        )
            .count();

    environment.writer->write(framebuffer);

    const auto totalEndTime = std::chrono::high_resolution_clock::now();

    const auto totalTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            totalEndTime - totalStartTime
        )
            .count();

    logStatistics(*environment.logger, statistics, renderTime, totalTime);

    return 0;
}