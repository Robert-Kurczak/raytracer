#include "NormalMapRenderer.hpp"

#include "Color/Color.hpp"
#include "Hittable/HitData.hpp"
#include "Math/Interval.hpp"
#include "Math/Vector.hpp"

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

Color8Bit NormalMapRenderer::getSkyColor(
    const Vector3<float>& rayDirectionVersor
) {
    const float a = (0.5F * rayDirectionVersor.getY()) + 1.0F;
    const Color<float> color =
        (1.0F - a) * WHITE_COLOR + a * BLUEISH_COLOR;

    return Color8Bit {
        uint8_t(color.red), uint8_t(color.green), uint8_t(color.blue)
    };
}

Color8Bit NormalMapRenderer::calculateColor(const HitData& hitData) {
    const float red = 255.0F * (hitData.hitNormal.getX() + 1.0F) / 2.0F;
    const float green = 255.0F * (hitData.hitNormal.getY() + 1.0F) / 2.0F;
    const float blue = 255.0F * (hitData.hitNormal.getZ() + 1.0F) / 2.0F;

    return Color8Bit {uint8_t(red), uint8_t(green), uint8_t(blue)};
}

void NormalMapRenderer::render(
    const Camera& camera,
    const Scene& scene,
    Framebuffer& framebuffer
) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();
    const Interval<float> renderedInterval = Interval<float>::universe();

    HitData hitData {};

    for (uint32_t yIndex = 0; yIndex < resolution.getY(); yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.getX(); xIndex++) {
            const Point2<uint32_t> pixel {xIndex, yIndex};

            Ray ray = camera.getRay(pixel);
            const bool objectHit =
                scene.hitRay(ray, renderedInterval, hitData);

            Color8Bit color;
            if (objectHit) {
                color = calculateColor(hitData);
            } else {
                color = getSkyColor(ray.getDirection().getNormalized());
            }

            framebuffer.setColorAt({xIndex, yIndex}, color);
        }
    }
}
}