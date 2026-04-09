#include "SkyBackground.hpp"

namespace RTC {
static constexpr LinearColor WHITE_ATTENUATION {
    .red = 1.0F,
    .green = 1.0F,
    .blue = 1.0F
};

static constexpr LinearColor BLUEISH_ATTENUATION {
    .red = 0.5F,
    .green = 0.7F,
    .blue = 1.0F
};

LinearColor SkyBackground::sample(const Ray& ray) const {
    const float a =
        (ray.getDirection().getNormalized().getY() + 1.0F) / 2.0F;

    return (1.0F - a) * WHITE_ATTENUATION + a * BLUEISH_ATTENUATION;
}
}