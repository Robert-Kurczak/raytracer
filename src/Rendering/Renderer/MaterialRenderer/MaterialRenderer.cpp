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
static constexpr uint32_t MAX_DEPTH = 10;

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

Ray MaterialRenderer::createShadowRay(
    const HitData& hitData,
    const LightData& lightData
) const {
    const Point3<float> origin =
        hitData.hitPoint + hitData.hitNormal * epsilon;

    return Ray {origin, lightData.toLight};
}

Color<float> MaterialRenderer::getSkyAttenuation(
    const Vector3<float>& rayDirectionVersor
) const {
    const float a = (rayDirectionVersor.getY() + 1.0F) / 2.0F;
    return (1.0F - a) * WHITE_ATTENUATION + a * BLUEISH_ATTENUATION;
}

Color<float> MaterialRenderer::getAttenuationRecursively(
    const Ray& ray,
    const Scene& scene,
    const Interval<float>& interval,
    int32_t depth
) const {
    if (depth <= 0) {
        return Color<float>::black();
    }

    HitData hitData {};

    const bool objectHit = scene.hitRay(ray, interval, hitData);

    if (!objectHit) {
        return getSkyAttenuation(ray.getDirection().getNormalized());
    }

    Color<float> illuminationColor = Color<float>::black();

    for (const auto& light : scene.getLights()) {
        const LightData lightData = light->getSample(hitData.hitPoint);
        const Ray shadowRay = createShadowRay(hitData, lightData);

        const bool shadowRayHit =
            scene.hitAny(shadowRay, {epsilon, 1.0F});

        if (shadowRayHit) {
            continue;
        }

        const float cosinus = getDotProduct(
            hitData.hitNormal, lightData.toLight.getNormalized()
        );

        const float intensity = std::max(cosinus, 0.0F);
        illuminationColor += lightData.illumination * intensity;
    }

    Ray scatteredRay {};
    Color<float> attenuation {.red = 0, .green = 0, .blue = 0};

    std::shared_ptr<IMaterial> material = defaultMaterial_;

    if (hitData.material) {
        material = hitData.material;
    }

    const bool wasScattered =
        material->scatter(ray, hitData, attenuation, scatteredRay);

    if (!wasScattered) {
        return Color<float> {.red = 0, .green = 0, .blue = 0};
    }

    const Color<float> newAttenuation = getAttenuationRecursively(
        scatteredRay, scene, interval, depth - 1
    );

    const Color<float> resultColor =
        illuminationColor + (attenuation * newAttenuation);

    return Color<float> {
        .red = std::min(resultColor.red, 1.0F),
        .green = std::min(resultColor.green, 1.0F),
        .blue = std::min(resultColor.blue, 1.0F)
    };
}

MaterialRenderer::MaterialRenderer(
    IProgressIndicator& progressIndicator,
    uint32_t samplesPerPixel,
    std::shared_ptr<IMaterial> defaultMaterial
) :
    progressIndicator_(progressIndicator),
    samplesPerPixel_(samplesPerPixel),
    defaultMaterial_(std::move(defaultMaterial)) {}

void MaterialRenderer::render(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();
    const Interval<float> renderedInterval {
        epsilon, Interval<float>::infinity()
    };

    const uint32_t samplesToTake =
        resolution.getX() * resolution.getY() * samplesPerPixel_;
    uint32_t currentSample = 0;

    HitData hitData {};

    for (uint32_t yIndex = 0; yIndex < resolution.getY(); yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.getX(); xIndex++) {
            const Point2<uint32_t> pixel {xIndex, yIndex};

            Color<float> resultColor {.red = 0, .green = 0, .blue = 0};

            for (uint32_t i = 0; i < samplesPerPixel_; i++) {
                Ray ray = camera.getRandomizedRay(pixel);

                const Color<float> attenuation =
                    getAttenuationRecursively(
                        ray, scene, renderedInterval, MAX_DEPTH
                    );

                resultColor += attenuation;

                currentSample++;

                const float progress =
                    float(currentSample) / float(samplesToTake);

                progressIndicator_.showProgress(progress);
            }

            framebuffer.setColorAt(
                {xIndex, yIndex},
                castColorTo8Bit(
                    resultColor * 255.0F / float(samplesPerPixel_)
                )
            );
        }
    }

    std::cout << "\n";
}
}