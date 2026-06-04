#include "PhotonMapRenderer.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Core/Math/Numeric.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Rendering/Material/IMaterial.hpp"
#include "Rendering/Material/MaterialSample.hpp"
#include "Rendering/PhotonMap/PhotonMap.hpp"
#include "Rendering/Renderer/PhotonMapRenderer/PhotonMapRendererParameters.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "World/Scene/Scene.hpp"

#include <format>
#include <memory>
#include <thread>
#include <utility>

namespace RTC {

LinearColor PhotonMapRenderer::getEmission(
    const HitData& hitData,
    const Vector3f& outDirection,
    uint32_t recursionDepth
) const {
    const bool isRayPrimary = recursionDepth == 0;
    const LinearColor materialEmission =
        hitData.material->getEmission(hitData.hitPoint, outDirection);

    return isRayPrimary ? materialEmission : LinearColor::black();
}

LinearColor PhotonMapRenderer::getIndirectLight(
    const HitData& hitData,
    const Vector3f& outDirection,
    const Scene& scene,
    const PhotonMaps& photonMaps,
    RendererStatistics& statistics,
    uint32_t recursionDepth
) const {
    const MaterialSample materialSample = hitData.material->getSample(
        hitData.hitPoint, hitData.hitNormal, outDirection
    );

    if (materialSample.scatterType == ScatterType::Specular) {
        const Point3f specularOffsetPoint =
            hitData.hitPoint + EPSILON * materialSample.inDirection;

        const Ray scatterRay {
            specularOffsetPoint, materialSample.inDirection
        };

        const LinearColor scatterLight = traceRay(
            scatterRay, scene, photonMaps, statistics, recursionDepth + 1
        );

        return (materialSample.brdf * scatterLight) / materialSample.pdf;
    }

    const LinearColor globalRadiance = photonMaps.globalMap.getRadiance(
        hitData, outDirection, parameters_.nearestPhotons
    );

    const LinearColor causticRadiance = photonMaps.causticMap.getRadiance(
        hitData, outDirection, parameters_.nearestPhotons
    );

    return globalRadiance + causticRadiance;
}

void PhotonMapRenderer::tracePhoton(
    Photon& photon,
    bool specularBounce,
    std::vector<Photon>& globalPhotons,
    std::vector<Photon>& causticPhotons,
    const Scene& scene,
    RendererStatistics& statistics,
    uint32_t recursionDepth
) {
    constexpr Interval<float> renderInterval {
        EPSILON, Interval<float>::infinity()
    };

    if (recursionDepth > parameters_.scatterRecursionDepth) {
        return;
    }

    HitData hitData;
    Ray photonRay {photon.position, photon.direction};

    statistics.rays++;

    const bool hitAnything =
        scene.hitClosest(photonRay, renderInterval, hitData);

    if (not hitAnything) {
        return;
    }

    const Vector3f outDirection =
        -photonRay.getDirection().getNormalized();

    const MaterialSample materialSample = hitData.material->getSample(
        hitData.hitPoint, hitData.hitNormal, outDirection
    );

    if (materialSample.scatterType == ScatterType::Specular) {
        specularBounce = true;
        photon.position =
            hitData.hitPoint + EPSILON * materialSample.inDirection;
        photon.direction = materialSample.inDirection;

        const LinearColor materialColor =
            materialSample.brdf / materialSample.pdf;

        photon.power.red *= materialColor.red;
        photon.power.green *= materialColor.green;
        photon.power.blue *= materialColor.blue;

        tracePhoton(
            photon,
            specularBounce,
            globalPhotons,
            causticPhotons,
            scene,
            statistics,
            recursionDepth + 1
        );

        return;
    }

    const Point3f offsetHitPoint =
        hitData.hitPoint + EPSILON * hitData.hitNormal;

    const float incidenceCosine = std::max(
        0.0F, getDotProduct(hitData.hitNormal, materialSample.inDirection)
    );

    const LinearColor materialColor =
        (materialSample.brdf * incidenceCosine) / materialSample.pdf;

    const float survivalProbability =
        std::clamp(materialColor.getLargestComponent(), 0.05F, 1.0F);

    if (getRandomNumber<float>() > survivalProbability) {
        return;
    }

    photon.position = offsetHitPoint;
    photon.power.red *= materialColor.red;
    photon.power.green *= materialColor.green;
    photon.power.blue *= materialColor.blue;
    photon.power /= survivalProbability;

    const bool hitDiffuse =
        materialSample.scatterType == ScatterType::Diffuse;

    if (specularBounce and hitDiffuse) {
        causticPhotons.push_back(photon);
    } else if (hitDiffuse) {
        globalPhotons.push_back(photon);
    }

    photon.direction = materialSample.inDirection;

    tracePhoton(
        photon,
        specularBounce,
        globalPhotons,
        causticPhotons,
        scene,
        statistics,
        recursionDepth + 1
    );
}

void PhotonMapRenderer::scatterPhotons(
    const Scene& scene,
    RendererStatistics& statistics,
    std::vector<Photon>& globalPhotons,
    std::vector<Photon>& causticPhotons
) {
    const uint32_t photonsPerLight =
        parameters_.emittedPhotons / scene.getLights().size();

    for (const std::shared_ptr<ILight>& light : scene.getLights()) {
        for (uint32_t i = 0; i < photonsPerLight; i++) {
            Photon photon = light->emitPhoton();
            photon.power /= float(photonsPerLight);

            tracePhoton(
                photon,
                false,
                globalPhotons,
                causticPhotons,
                scene,
                statistics
            );
        }
    }
}

LinearColor PhotonMapRenderer::traceRay(
    const Ray& ray,
    const Scene& scene,
    const PhotonMaps& photonMaps,
    RendererStatistics& statistics,
    uint32_t recursionDepth
) const {
    constexpr Interval<float> renderInterval {
        EPSILON, Interval<float>::infinity()
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
        hitData.hitPoint + EPSILON * hitData.hitNormal;

    const LinearColor emittedLight =
        getEmission(hitData, outDirection, recursionDepth);

    const LinearColor indirectLight = getIndirectLight(
        hitData,
        outDirection,
        scene,
        photonMaps,
        statistics,
        recursionDepth
    );

    const LinearColor directLight = lightSampler_->getRadiance(
        scene,
        hitData,
        offsetHitPoint,
        outDirection,
        statistics,
        parameters_.lightSamplesPerHit
    );

    return emittedLight + directLight + indirectLight;
}

RendererStatistics PhotonMapRenderer::renderSection(
    const Camera& camera,
    const Scene& scene,
    const PhotonMaps& photonMaps,
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
                    traceRay(ray, scene, photonMaps, statistics);

                resultColor += color;
            }

            resultColor /= float(parameters_.pathsPerPixel);
            framebuffer.setColorAt(pixel, resultColor);

            progressIndicator_->tick();
        }
    }

    return statistics;
}

std::vector<RendererStatistics> PhotonMapRenderer::renderAll(
    const Camera& camera,
    const Scene& scene,
    const PhotonMaps& photonMaps,
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
        EPSILON, Interval<float>::infinity()
    };
    const Interval<uint32_t> yIndices {0, resolution.getY()};

    progressIndicator_->setGoal(resolution.getX() * resolution.getY());

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
                    photonMaps,
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
                photonMaps,
                renderInterval,
                xIndices,
                yIndices,
                framebuffer
            );
        }};
    }

    return statistics;
}

PhotonMapRenderer::PhotonMapRenderer(
    std::shared_ptr<ILogger> logger,
    std::unique_ptr<ILightSampler> lightSampler,
    std::unique_ptr<IProgressIndicator> progressIndicator,
    std::unique_ptr<IPhotonMapBuilder> photonMapBuilder,
    std::unique_ptr<IBackground> background,
    PhotonMapRendererParameters parameters
) :
    logger_(std::move(logger)),
    lightSampler_(std::move(lightSampler)),
    progressIndicator_(std::move(progressIndicator)),
    photonMapBuilder_(std::move(photonMapBuilder)),
    background_(std::move(background)),
    parameters_(parameters) {}

RendererStatistics PhotonMapRenderer::render(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) noexcept {
    RendererStatistics totalStatistics;

    std::vector<Photon> globalPhotons;
    std::vector<Photon> causticPhotons;

    scatterPhotons(scene, totalStatistics, globalPhotons, causticPhotons);

    const PhotonMaps photonMaps {
        .globalMap = photonMapBuilder_->build(std::move(globalPhotons)),
        .causticMap = photonMapBuilder_->build(std::move(causticPhotons))
    };

    const std::vector<RendererStatistics> threadStatistics =
        renderAll(camera, scene, photonMaps, framebuffer);

    for (const auto& stats : threadStatistics) {
        totalStatistics.rays += stats.rays;
        totalStatistics.shadowRays += stats.shadowRays;
    }

    return totalStatistics;
}
}