#include "PointLight.hpp"

#include "Core/Math/Sampling.hpp"
#include "Core/Math/Transformations.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Geometry/Light/Photon.hpp"

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
    const Vector3f toLight = position_ - origin;
    const Vector3f inDirection = toLight.getNormalized();
    const float distandeSquared = toLight.getSquaredLength();
    const LinearColor outLight = emission_ / (distandeSquared * decay_);

    return LightSample {
        .outLight = outLight,
        .toLight = toLight,
        .inDirection = inDirection,
        .pdf = 1
    };
}

LinearColor PointLight::getPower() const  {
    return emission_;
}

Photon PointLight::emitPhoton() const {
    return Photon {
        .position = position_,
        .direction = sampleUnitVector(),
        .power = emission_
    };
}
}