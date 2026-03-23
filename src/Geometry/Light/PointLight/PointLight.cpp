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

LightData PointLight::getSample(
    const Point3<float>& worldPosition
) const {
    const Vector3<float> toLight = position_ - worldPosition;
    const float distanceSquared = toLight.getSquaredLength();
    Color<float> illumination = color_ / (distanceSquared * decay_);

    return LightData {
        .toLight = toLight,
        .distance = toLight.getLength(),
        .illumination = illumination
    };
}
}