#include "SampleRenderer.hpp"

#include "Color/Color.hpp"

namespace RTC {
void SampleRenderer::render(Framebuffer& framebuffer) noexcept {
    const Vector2<uint32_t> resolution = framebuffer.getResolution();

    for (uint32_t yIndex = 0; yIndex < resolution.y; yIndex++) {
        for (uint32_t xIndex = 0; xIndex < resolution.x; xIndex++) {
            const Color8Bit color {
                .red = uint8_t(
                    (float(xIndex) / float(resolution.x - 1)) * 255
                ),
                .green = uint8_t(
                    (float(yIndex) / float(resolution.y - 1)) * 255
                ),
                .blue = 80
            };

            framebuffer.setColorAt({xIndex, yIndex}, color);
        }
    }
}
}