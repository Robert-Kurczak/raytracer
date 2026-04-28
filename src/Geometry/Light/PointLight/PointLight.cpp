#include "PointLight.hpp"

#include "Core/Math/Vector.hpp"
#include "Geometry/Light/LightSample.hpp"

namespace RTC {
static constexpr float epsilon = 0.001;

PointLight::PointLight(
    const Point3<float>& position,
    const LinearColor& color,
    float decay
) :
    position_(position),
    color_(color),
    decay_(std::max(decay, epsilon)) {}

PointLight::PointLight(
    const Point3<float>& position,
    const Color8Bit& color,
    float decay
) :
    PointLight(
        position,
        LinearColor {
            .red = float(color.red) / 255.0F,
            .green = float(color.green) / 255.0F,
            .blue = float(color.blue) / 255.0F
        },
        decay
    ) {}

LightSample PointLight::getSample(
    const Point3<float>& worldPosition,
    const Vector3<float>& worldNormal
) const {
    const Vector3<float> toLight = position_ - worldPosition;
    const float distanceSquared = toLight.getSquaredLength();
    const LinearColor intensity = color_ / (distanceSquared * decay_);

    return LightSample {.intensity = intensity, .toLight = toLight};
}
}