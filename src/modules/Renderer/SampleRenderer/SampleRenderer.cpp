#include "SampleRenderer.hpp"

#include "Color/Color.hpp"

namespace RTC {
void SampleRenderer::render(
    const Camera& camera,
    Framebuffer& framebuffer
) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();

    for (uint32_t yIndex = 0; yIndex < resolution.getY(); yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.getX(); xIndex++) {
            const Point2<uint32_t> pixel {xIndex, yIndex};
            const Vector3<float> rayDirection =
                camera.getRay(pixel).getDirection().getNormalized();

            const Color8Bit color {
                .red = uint8_t(
                    ((rayDirection.getX() + 1.0F) / 2.0F) * 255.0F
                ),
                .green = uint8_t(
                    ((rayDirection.getY() + 1.0F) / 2.0F) * 255.0F
                ),
                .blue = uint8_t(
                    ((rayDirection.getZ() + 1.0F) / 2.0F) * 255.0F
                )
            };

            framebuffer.setColorAt({xIndex, yIndex}, color);
        }
    }
}
}