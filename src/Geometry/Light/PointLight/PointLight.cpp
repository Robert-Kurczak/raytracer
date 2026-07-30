#include "PointLight.hpp"

#include "Core/Math/Numeric.hpp"
#include "Core/Math/Sampling.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Geometry/Light/Photon.hpp"

namespace RTC {
PointLight::PointLight(
    const Point3f& position,
    const LinearColor& emission,
    float decay
) :
    position_(position),
    emission_(emission),
    decay_(std::max(decay, EPSILON)),
    boundingBox_(AxisAlignedBoundingBox {position_}) {}

void PointLight::setup(const AxisAlignedBoundingBox& sceneBoundingBox) {}

void PointLight::discretize(
    std::vector<std::shared_ptr<ILight>>& discreteLights,
    uint32_t samples
) const {
    std::shared_ptr<ILight> light =
        std::make_shared<PointLight>(position_, emission_, decay_);

    discreteLights.emplace_back(light);
}

bool PointLight::isInfinite() const {
    return false;
}

AxisAlignedBoundingBox PointLight::getBoundingBox() const {
    return boundingBox_;
}

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

LinearColor PointLight::getPower() const {
    return emission_;
}

Photon PointLight::emitPhoton() const {
    return Photon {
        .position = position_,
        .direction = sampleUnitVector(),
        .power = emission_
    };
}

float PointLight::calculatePdf(
    const Point3f& origin,
    const Vector3f& inDirection
) const {
    return 0.0F;
}
}