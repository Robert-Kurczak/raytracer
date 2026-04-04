#include "MaterialRenderer.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/LightData.hpp"
#include "Geometry/Material/IMaterial.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "World/Scene/Scene.hpp"

#include <format>
#include <memory>
#include <thread>
#include <utility>

namespace RTC {
static constexpr float epsilon = 0.001F;

bool MaterialRenderer::isInShadow(
    const HitData& hitData,
    const LightData& lightData,
    const Scene& scene
) const {
    const Point3<float> origin =
        hitData.hitPoint + hitData.hitNormal * epsilon;

    const Ray shadowRay {origin, lightData.toLight};

    const Interval<float> interval {
        epsilon, // ray origin is at hit point
        1.0F // ray end (light) is at the end of unnormalized direction
    };

    return scene.hitAny(shadowRay, interval);
}

Color<float> MaterialRenderer::getDirectLight(
    const HitData& hitData,
    const Scene& scene
) const {
    Color<float> illuminationColor = Color<float>::black();

    for (const auto& light : scene.getLights()) {
        const LightData lightData = light->getSample(hitData.hitPoint);

        if (isInShadow(hitData, lightData, scene)) {
            continue;
        }

        const float cosinus = getDotProduct(
            hitData.hitNormal, lightData.toLight.getNormalized()
        );

        const float intensity = std::max(cosinus, 0.0F);
        illuminationColor += lightData.illumination * intensity;
    }

    return illuminationColor;
}

Color<float> MaterialRenderer::getIndirectLight(
    Color<float>& attenuation,
    const Ray& ray,
    const Interval<float>& interval,
    const HitData& hitData,
    const Scene& scene,
    RendererStatistics& statistics,
    uint32_t depth
) const {
    Ray scatteredRay {};

    const std::shared_ptr<IMaterial> material =
        hitData.material ? hitData.material
                         : parameters_.defaultMaterial_;

    const bool wasScattered =
        material->scatter(ray, hitData, attenuation, scatteredRay);

    if (!wasScattered) {
        return Color<float> {.red = 0, .green = 0, .blue = 0};
    }

    return traceRay(scatteredRay, scene, interval, statistics, depth - 1);
}

Color<float> MaterialRenderer::traceRay(
    const Ray& ray,
    const Scene& scene,
    const Interval<float>& interval,
    RendererStatistics& statistics,
    uint32_t depth
) const {
    if (depth == 0) {
        return Color<float>::black();
    }

    statistics.rays++;

    HitData hitData {};

    const bool objectHit = scene.hitRay(ray, interval, hitData);

    if (!objectHit) {
        return background_->sample(ray);
    }

    const Color<float> directLight = getDirectLight(hitData, scene);
    statistics.shadowRays += scene.getLights().size();

    Color<float> indirectLightAttenuation = Color<float>::black();
    const Color<float> indirectLight = getIndirectLight(
        indirectLightAttenuation,
        ray,
        interval,
        hitData,
        scene,
        statistics,
        depth
    );

    const Color<float> resultColor =
        directLight + (indirectLight * indirectLightAttenuation);

    return Color<float> {
        .red = std::min(resultColor.red, 1.0F),
        .green = std::min(resultColor.green, 1.0F),
        .blue = std::min(resultColor.blue, 1.0F)
    };
}

RendererStatistics MaterialRenderer::renderSection(
    const Camera& camera,
    const Scene& scene,
    const Interval<float>& renderInterval,
    const Interval<uint32_t>& xIndices,
    const Interval<uint32_t>& yIndices,
    Framebuffer& framebuffer
) const {
    RendererStatistics statistics {};

    for (uint32_t y = yIndices.start; y < yIndices.end; y++) {
        for (uint32_t x = xIndices.start; x < xIndices.end; x++) {
            const Point2<uint32_t> pixel {x, y};

            Color<float> resultColor = Color<float>::black();

            for (uint32_t i = 0; i < parameters_.samplesPerPixel; i++) {
                Ray ray = camera.getRandomizedRay(pixel);

                const Color<float> color = traceRay(
                    ray,
                    scene,
                    renderInterval,
                    statistics,
                    parameters_.scatterRecursionDepth
                );

                resultColor += color;
            }

            const Color8Bit color8Bit = castColorTo8Bit(
                resultColor * 255.0F / float(parameters_.samplesPerPixel)
            );

            framebuffer.setColorAt(pixel, color8Bit);
        }
    }

    return statistics;
}

std::vector<RendererStatistics> MaterialRenderer::renderAll(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) const {
    const uint32_t threadCount = std::thread::hardware_concurrency();
    logger_->log(
        LogLevel::Info,
        std::format("Rendering on {} threads", threadCount)
    );

    const Vector2<uint32_t> resolution = framebuffer.getResolution();
    const uint32_t sectionWidth = resolution.getX() / threadCount;

    const Interval<float> renderInterval {
        epsilon, Interval<float>::infinity()
    };
    const Interval<uint32_t> yIndices {0, resolution.getY()};

    std::vector<RendererStatistics> statistics(threadCount);

    {
        std::vector<std::jthread> threads(threadCount);

        for (uint32_t i = 0; i < threadCount - 1; i++) {
            threads[i] = std::jthread {[&, i]() {
                const uint32_t sectionStart = i * sectionWidth;
                const Interval<uint32_t> xIndices {
                    sectionStart, sectionStart + sectionWidth
                };

                statistics[i] = renderSection(
                    camera,
                    scene,
                    renderInterval,
                    xIndices,
                    yIndices,
                    framebuffer
                );
            }};
        }

        const Interval<uint32_t> xIndices {
            (threadCount - 1) * sectionWidth, resolution.getX()
        };

        threads[threadCount - 1] = std::jthread {[&]() {
            statistics[threadCount - 1] = renderSection(
                camera,
                scene,
                renderInterval,
                xIndices,
                yIndices,
                framebuffer
            );
        }};
    }

    return statistics;
}

MaterialRenderer::MaterialRenderer(
    std::shared_ptr<ILogger> logger,
    std::unique_ptr<IBackground> background,
    MaterialRendererParameters parameters
) :
    logger_(std::move(logger)),
    background_(std::move(background)),
    parameters_(std::move(parameters)) {}

RendererStatistics MaterialRenderer::render(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) noexcept {
    logger_->log(LogLevel::Info, "Rendering");

    RendererStatistics totalStatistics;

    const std::vector<RendererStatistics> threadStatistics =
        renderAll(camera, scene, framebuffer);

    for (const auto& stats : threadStatistics) {
        totalStatistics.rays += stats.rays;
        totalStatistics.shadowRays += stats.shadowRays;
    }

    return totalStatistics;
}
}