#include "SampleRenderer.hpp"

#include "Color/Color.hpp"

namespace RTC {
void SampleRenderer::render(Framebuffer& framebuffer) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();

    for (uint32_t yIndex = 0; yIndex < resolution.getY(); yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.getX(); xIndex++) {
            const Color8Bit color {
                .red = uint8_t(
                    (float(xIndex) / float(resolution.getX() - 1)) * 255
                ),
                .green = uint8_t(
                    (float(yIndex) / float(resolution.getY() - 1)) * 255
                ),
                .blue = 80
            };

            framebuffer.setColorAt({xIndex, yIndex}, color);
        }
    }
}
}