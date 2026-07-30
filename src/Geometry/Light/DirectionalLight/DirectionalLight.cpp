#include "DirectionalLight.hpp"

#include "Core/Math/Sampling.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Geometry/Light/Photon.hpp"
#include "Geometry/Light/PointLight/PointLight.hpp"

#include <numbers>

namespace RTC {
Point3f DirectionalLight::getRandomPoint() const {
    const Vector3f randomDirection =
        sampleHemisphere(Vector3f {0.0F, 1.0F, 0.0F});

    const Point3f randomPoint =
        boundingSphereCenter_ + (randomDirection * boundingSphereRadius_);

    return randomPoint;
}

DirectionalLight::DirectionalLight(
    const LinearColor& emission,
    const Vector3f& direction
) :
    emission_(emission),
    direction_(direction) {}

void DirectionalLight::setup(
    const AxisAlignedBoundingBox& sceneBoundingBox
) {
    boundingSphereCenter_ = sceneBoundingBox.getBoxCenter();
    boundingSphereRadius_ = sceneBoundingBox.getDiagonal() / 1.5F;

    power_ = emission_ * float(std::numbers::pi) * boundingSphereRadius_ *
             boundingSphereRadius_;

    boundingBox_ = AxisAlignedBoundingBox {
        boundingSphereCenter_ + (-direction_ * boundingSphereRadius_)
    };
}

void DirectionalLight::discretize(
    std::vector<std::shared_ptr<ILight>>& discreteLights,
    uint32_t samples
) const {
    const LinearColor sampleEmission = emission_ / float(samples);

    for (uint32_t i = 0; i < samples; i++) {
        const Point3f randomPoint = getRandomPoint();

        std::shared_ptr<ILight> light = std::make_shared<PointLight>(
            randomPoint, sampleEmission, 1.0F
        );

        discreteLights.emplace_back(light);
    }
}

bool DirectionalLight::isInfinite() const {
    return true;
}

AxisAlignedBoundingBox DirectionalLight::getBoundingBox() const {
    return boundingBox_;
}

LightSample DirectionalLight::getSample(const Point3f& origin) const {
    return LightSample {
        .outLight = emission_,
        .toLight = -direction_ * boundingSphereRadius_,
        .inDirection = -direction_,
        .pdf = 1.0F
    };
}

LinearColor DirectionalLight::getPower() const {
    return power_;
}

Photon DirectionalLight::emitPhoton() const {
    const Point3f randomPoint = getRandomPoint();

    return Photon {
        .position = randomPoint, .direction = direction_, .power = power_
    };
}

float DirectionalLight::calculatePdf(
    const Point3f& origin,
    const Vector3f& inDirection
) const {
    return 0.0F;
}
}