#include "MaterialRenderer.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/LightData.hpp"
#include "Geometry/Material/IMaterial.hpp"
#include "World/Scene/Scene.hpp"

#include <iostream>
#include <memory>
#include <utility>

namespace RTC {
static constexpr float epsilon = 0.001F;

static constexpr Color<float> WHITE_ATTENUATION {
    .red = 1.0F,
    .green = 1.0F,
    .blue = 1.0F
};

static constexpr Color<float> BLUEISH_ATTENUATION {
    .red = 0.5F,
    .green = 0.7F,
    .blue = 1.0F
};

Color<float> MaterialRenderer::getSkyAttenuation(
    const Vector3<float>& rayDirectionVersor
) const {
    const float a = (rayDirectionVersor.getY() + 1.0F) / 2.0F;
    return (1.0F - a) * WHITE_ATTENUATION + a * BLUEISH_ATTENUATION;
}

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

    return traceRay(scatteredRay, scene, interval, depth - 1);
}

Color<float> MaterialRenderer::traceRay(
    const Ray& ray,
    const Scene& scene,
    const Interval<float>& interval,
    uint32_t depth
) const {
    if (depth == 0) {
        return Color<float>::black();
    }

    HitData hitData {};

    const bool objectHit = scene.hitRay(ray, interval, hitData);

    if (!objectHit) {
        return getSkyAttenuation(ray.getDirection().getNormalized());
    }

    const Color<float> directLight = getDirectLight(hitData, scene);
    Color<float> indirectLightAttenuation = Color<float>::black();
    const Color<float> indirectLight = getIndirectLight(
        indirectLightAttenuation, ray, interval, hitData, scene, depth
    );

    const Color<float> resultColor =
        directLight + (indirectLight * indirectLightAttenuation);

    return Color<float> {
        .red = std::min(resultColor.red, 1.0F),
        .green = std::min(resultColor.green, 1.0F),
        .blue = std::min(resultColor.blue, 1.0F)
    };
}

void MaterialRenderer::renderSection(
    const Camera& camera,
    const Scene& scene,
    const Interval<float>& renderInterval,
    const Interval<uint32_t>& xIndices,
    const Interval<uint32_t>& yIndices,
    Framebuffer& framebuffer
) const {
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
}

MaterialRenderer::MaterialRenderer(
    MaterialRendererParameters parameters,
    IProgressIndicator& progressIndicator
) :
    parameters_(std::move(parameters)),
    progressIndicator_(progressIndicator) {}

void MaterialRenderer::render(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) noexcept {
    constexpr Interval<float> renderInterval {
        epsilon, Interval<float>::infinity()
    };
    const Vector2<uint32_t> resolution = framebuffer.getResolution();

    const Interval<uint32_t> xIndices {0, resolution.getX()};
    const Interval<uint32_t> yIndices {0, resolution.getY()};

    renderSection(
        camera, scene, renderInterval, xIndices, yIndices, framebuffer
    );

    std::cout << "\n";
}
}