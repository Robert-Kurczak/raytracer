#include "NormalMapRenderer.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Hittable/HitData.hpp"

namespace RTC {
inline constexpr Color<float> WHITE_COLOR {
    .red = 255,
    .green = 255,
    .blue = 255
};
inline constexpr Color<float> BLUEISH_COLOR {
    .red = 128,
    .green = 179,
    .blue = 255
};

Color<float> NormalMapRenderer::getSkyColor(
    const Vector3<float>& rayDirectionVersor
) {
    const float a = (rayDirectionVersor.getY() + 1.0F) / 2.0F;
    return (1.0F - a) * WHITE_COLOR + a * BLUEISH_COLOR;
}

Color<float> NormalMapRenderer::calculateColor(const HitData& hitData) {
    const float red = 255.0F * (hitData.hitNormal.getX() + 1.0F) / 2.0F;
    const float green = 255.0F * (hitData.hitNormal.getY() + 1.0F) / 2.0F;
    const float blue = 255.0F * (hitData.hitNormal.getZ() + 1.0F) / 2.0F;

    return Color<float> {.red = red, .green = green, .blue = blue};
}

NormalMapRenderer::NormalMapRenderer(
    IProgressIndicator& progressIndicator,
    uint32_t samplesPerPixel
) :
    progressIndicator_(progressIndicator),
    samplesPerPixel_(samplesPerPixel) {}

RendererStatistics NormalMapRenderer::render(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) noexcept {
    RendererStatistics statistics {};

    const Vector2<uint32_t> resolution = framebuffer.getResolution();
    const Interval<float> renderedInterval {
        0.0F, Interval<float>::infinity()
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

                const bool objectHit =
                    scene.hitRay(ray, renderedInterval, hitData);

                statistics.rays++;

                Color<float> sampleColor;
                if (objectHit) {
                    sampleColor = calculateColor(hitData);
                } else {
                    sampleColor =
                        getSkyColor(ray.getDirection().getNormalized());
                }

                resultColor += sampleColor;

                currentSample++;
                const float progress =
                    float(currentSample) / float(samplesToTake);

                progressIndicator_.showProgress(progress);
            }

            framebuffer.setColorAt(
                {xIndex, yIndex},
                castColorTo8Bit(resultColor / float(samplesPerPixel_))
            );
        }
    }

    return statistics;
}
}