#include "DefaultRenderer.hpp"

#include "Color/Color.hpp"
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

Color8Bit DefaultRenderer::getSkyColor(
    const Vector3<float>& rayDirectionVersor
) {
    const float a = (0.5F * rayDirectionVersor.getY()) + 1.0F;
    const Color<float> color =
        (1.0F - a) * WHITE_COLOR + a * BLUEISH_COLOR;

    return Color8Bit {
        uint8_t(color.red), uint8_t(color.green), uint8_t(color.blue)
    };
}

Color8Bit DefaultRenderer::calculateColor(Ray& ray) {
    const Vector3<float> rayDirectionVersor =
        ray.getDirection().getVersor();

    return getSkyColor(rayDirectionVersor);
}

void DefaultRenderer::render(
    const Camera& camera,
    Framebuffer& framebuffer
) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();

    for (uint32_t yIndex = 0; yIndex < resolution.getY(); yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.getX(); xIndex++) {
            const Point2<uint32_t> pixel {xIndex, yIndex};

            Ray ray = camera.getRay(pixel);
            const Color8Bit color = calculateColor(ray);

            framebuffer.setColorAt({xIndex, yIndex}, color);
        }
    }
}
}