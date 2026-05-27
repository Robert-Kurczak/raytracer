#include "MaterialRenderer.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Rendering/Material/IMaterial.hpp"
#include "Rendering/Material/MaterialSample.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "World/Scene/Scene.hpp"

#include <format>
#include <memory>
#include <thread>
#include <utility>

namespace RTC {
static constexpr float epsilon = 1e-4F;

LinearColor MaterialRenderer::getEmission(
    const HitData& hitData,
    const Vector3f& outDirection,
    uint32_t recursionDepth
) const {
    const bool isRayPrimary = recursionDepth == 0;
    const LinearColor materialEmission =
        hitData.material->getEmission(hitData.hitPoint, outDirection);

    return isRayPrimary ? materialEmission : LinearColor::black();
}

LinearColor MaterialRenderer::getIndirectLight(
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    const Scene& scene,
    RendererStatistics& statistics,
    uint32_t recursionDepth
) const {
    const MaterialSample materialSample = hitData.material->getSample(
        hitData.hitPoint, hitData.hitNormal, outDirection
    );

    if (materialSample.scatterType == ScatterType::Specular) {
        const Point3f specularOffsetPoint =
            hitData.hitPoint + epsilon * materialSample.inDirection;

        const Ray scatterRay {
            specularOffsetPoint, materialSample.inDirection
        };

        const LinearColor scatterLight =
            traceRay(scatterRay, scene, statistics, recursionDepth + 1);

        return (materialSample.brdf * scatterLight) / materialSample.pdf;
    }

    const Ray scatterRay {offsetHitPoint, materialSample.inDirection};

    const LinearColor scatterLight =
        traceRay(scatterRay, scene, statistics, recursionDepth + 1);

    const float cosinus = std::max(
        0.0F, getDotProduct(hitData.hitNormal, materialSample.inDirection)
    );

    return (materialSample.brdf * scatterLight * cosinus) /
           materialSample.pdf;
}

LinearColor MaterialRenderer::getDirectLight(
    const HitData& hitData,
    const Point3f& offsetHitPoint,
    const Vector3f& outDirection,
    const Scene& scene,
    RendererStatistics& statistics
) const {
    statistics.shadowRays += scene.getLights().size();

    LinearColor directLight = LinearColor::black();

    for (const std::unique_ptr<ILight>& light : scene.getLights()) {
        for (uint32_t i = 0; i < parameters_.lightSamplesPerHit; i++) {
            const LightSample lightSample =
                light->getSample(hitData.hitPoint);

            if (isInShadow(offsetHitPoint, lightSample.toLight, scene)) {
                continue;
            }

            const LinearColor brdf = hitData.material->calculateBrdf(
                hitData.hitPoint,
                hitData.hitNormal,
                outDirection,
                lightSample.inDirection
            );

            const float cosinus = std::max(
                0.0F,
                getDotProduct(hitData.hitNormal, lightSample.inDirection)
            );

            const LinearColor sample =
                brdf * lightSample.outLight * cosinus / lightSample.pdf;

            directLight += LinearColor {
                .red = sample.red / float(parameters_.lightSamplesPerHit),
                .green =
                    sample.green / float(parameters_.lightSamplesPerHit),
                .blue =
                    sample.blue / float(parameters_.lightSamplesPerHit)
            };
        }
    }

    return directLight;
}

bool MaterialRenderer::isInShadow(
    const Point3f& origin,
    const Vector3f& toLight,
    const Scene& scene
) const {
    const Ray shadowRay {origin, toLight};

    const Interval<float> interval {
        epsilon,       // ray origin is at hit point
        1.0F - epsilon // ray end (light) is at the end of unnormalized
                       // direction
    };

    return scene.hitAny(shadowRay, interval);
}

LinearColor MaterialRenderer::traceRay(
    const Ray& ray,
    const Scene& scene,
    RendererStatistics& statistics,
    uint32_t recursionDepth
) const {
    constexpr Interval<float> renderInterval {
        epsilon, Interval<float>::infinity()
    };

    if (recursionDepth > parameters_.scatterRecursionDepth) {
        return LinearColor::black();
    }

    statistics.rays++;

    HitData hitData;

    const bool hitAnything =
        scene.hitClosest(ray, renderInterval, hitData);

    if (not hitAnything) {
        return background_->sample(ray);
    }

    const Vector3f outDirection = -ray.getDirection().getNormalized();
    const Point3f offsetHitPoint =
        hitData.hitPoint + epsilon * hitData.hitNormal;

    const LinearColor emittedLight =
        getEmission(hitData, outDirection, recursionDepth);

    const LinearColor indirectLight = getIndirectLight(
        hitData,
        offsetHitPoint,
        outDirection,
        scene,
        statistics,
        recursionDepth
    );

    const LinearColor directLight = getDirectLight(
        hitData, offsetHitPoint, outDirection, scene, statistics
    );

    return emittedLight + indirectLight + directLight;
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

            LinearColor resultColor = LinearColor::black();

            for (uint32_t i = 0; i < parameters_.pathsPerPixel; i++) {
                Ray ray = camera.getRandomizedRay(pixel);

                const LinearColor color =
                    traceRay(ray, scene, statistics);

                resultColor += color;
            }

            resultColor /= float(parameters_.pathsPerPixel);

            framebuffer.setColorAt(pixel, resultColor);
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

    logger_->log(
        LogLevel::Info,
        std::format("Total light sources: {}", scene.getLights().size())
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
    parameters_(parameters) {}

RendererStatistics MaterialRenderer::render(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) noexcept {
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