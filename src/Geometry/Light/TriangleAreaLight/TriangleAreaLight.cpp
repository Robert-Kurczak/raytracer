#include "TriangleAreaLight.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Sampling.hpp"
#include "Core/Math/Transformations.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/LightSample.hpp"

#include <unistd.h>

namespace RTC {
AxisAlignedBoundingBox TriangleAreaLight::createBoundingBox(
    const Point3f& vertexA,
    const Point3f& vertexB,
    const Point3f& vertexC
) const {
    const float minX =
        std::min({vertexA.getX(), vertexB.getX(), vertexC.getX()});
    const float minY =
        std::min({vertexA.getY(), vertexB.getY(), vertexC.getY()});
    const float minZ =
        std::min({vertexA.getZ(), vertexB.getZ(), vertexC.getZ()});

    const float maxX =
        std::max({vertexA.getX(), vertexB.getX(), vertexC.getX()});
    const float maxY =
        std::max({vertexA.getY(), vertexB.getY(), vertexC.getY()});
    const float maxZ =
        std::max({vertexA.getZ(), vertexB.getZ(), vertexC.getZ()});

    const Point3f minPoint {minX, minY, minZ};
    const Point3f maxPoint {maxX, maxY, maxZ};

    return AxisAlignedBoundingBox {minPoint, maxPoint};
}

Vector3f TriangleAreaLight::getPerpendicular(
    const Point3f& vertexA,
    const Point3f& vertexB,
    const Point3f& vertexC
) const {
    const Vector3f edge1 = vertexB - vertexA;
    const Vector3f edge2 = vertexC - vertexA;

    return getCrossProduct(edge1, edge2);
}

Point3f TriangleAreaLight::getRandomSample() const {
    auto barycentricA = getRandomNumber<float>();
    auto barycentricB = getRandomNumber<float>();

    if (barycentricA + barycentricB > 1) {
        barycentricA = 1 - barycentricA;
        barycentricB = 1 - barycentricB;
    }

    const Point3f randomSample = barycentricA * (vertexA_ - vertexC_) +
                                 barycentricB * (vertexB_ - vertexC_) +
                                 vertexC_;

    return randomSample;
}

TriangleAreaLight::TriangleAreaLight(
    const Point3f& vertexA,
    const Point3f& vertexB,
    const Point3f& vertexC,
    const LinearColor& emission
) :
    vertexA_(vertexA),
    vertexB_(vertexB),
    vertexC_(vertexC),
    emission_(emission),
    perpendicular_(getPerpendicular(vertexA_, vertexB_, vertexC_)),
    normal_(perpendicular_.getNormalized()),
    area_(perpendicular_.getLength()),
    power_(area_ * emission_),
    boundingBox_(createBoundingBox(vertexA_, vertexB_, vertexC_)) {}

void TriangleAreaLight::setup(
    const AxisAlignedBoundingBox& sceneBoundingBox
) {}

AxisAlignedBoundingBox TriangleAreaLight::getBoundingBox() const {
    return boundingBox_;
}

LightSample TriangleAreaLight::getSample(const Point3f& origin) const {
    const Point3f sample = getRandomSample();
    const Vector3f toLight = sample - origin;
    const Vector3f inDirection = toLight.getNormalized();
    const float distanceSquared = toLight.getSquaredLength();
    const float cosine = getDotProduct(normal_, -inDirection);

    if (cosine <= 0) {
        return LightSample {
            .outLight = LinearColor::black(),
            .inDirection = inDirection,
            .pdf = 1
        };
    }

    const float solidAreaPdf = distanceSquared / (cosine * area_);

    return LightSample {
        .outLight = emission_,
        .toLight = toLight,
        .inDirection = inDirection,
        .pdf = solidAreaPdf
    };
}

LinearColor TriangleAreaLight::getPower() const {
    return power_;
}

Photon TriangleAreaLight::emitPhoton() const {
    const Vector3f randomDirection =
        transformToWorldSpace(sampleCosineHemisphere(), normal_);

    return Photon {
        .position = getRandomSample(),
        .direction = randomDirection,
        .power = power_
    };
}
}