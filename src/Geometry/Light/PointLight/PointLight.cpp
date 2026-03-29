#include "PointLight.hpp"

#include "Core/Math/Vector.hpp"
#include "Geometry/Light/LightData.hpp"

namespace RTC {
static constexpr float epsilon = 0.001;

PointLight::PointLight(
    const Point3<float>& position,
    const Color<float>& color,
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
        Color<float> {
            .red = float(color.red) / 255.0F,
            .green = float(color.green) / 255.0F,
            .blue = float(color.blue) / 255.0F
        },
        decay
    ) {}

LightData PointLight::getSample(
    const Point3<float>& worldPosition
) const {
    const Vector3<float> toLight = position_ - worldPosition;
    const float distanceSquared = toLight.getSquaredLength();
    Color<float> illumination = color_ / (distanceSquared * decay_);

    return LightData {.toLight = toLight, .illumination = illumination};
}
}