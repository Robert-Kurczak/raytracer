#include "PointLight.hpp"

#include "Core/Math/Vector.hpp"
#include "Geometry/Light/LightSample.hpp"

namespace RTC {
static constexpr float epsilon = 0.001;

PointLight::PointLight(
    const Point3f& position,
    const LinearColor& emission,
    float decay
) :
    position_(position),
    emission_(emission),
    decay_(std::max(decay, epsilon)) {}

PointLight::PointLight(
    const Point3<float>& position,
    const Color8Bit& emission,
    float decay
) :
    PointLight(
        position,
        LinearColor {
            .red = float(emission.red) / 255.0F,
            .green = float(emission.green) / 255.0F,
            .blue = float(emission.blue) / 255.0F
        },
        decay
    ) {}

LightSample PointLight::getSample(const Point3f& origin) const {
    const Vector3f inDirection = position_ - origin;
    const float distandeSquared = inDirection.getSquaredLength();
    const LinearColor outLight = emission_ / (distandeSquared * decay_);

    return LightSample {
        .outLight = outLight, .inDirection = inDirection, .pdf = 1
    };
}
}