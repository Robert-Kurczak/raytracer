#include "TriangleAreaLight.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Light/LightSample.hpp"

namespace RTC {
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
    area_(perpendicular_.getLength()) {}

LightSample TriangleAreaLight::getSample(const Point3f& origin) const {
    const Point3f sample = getRandomSample();
    const Vector3f inDirection = sample - origin;
    const float distanceSquared = inDirection.getSquaredLength();
    const float cosinus =
        getDotProduct(normal_, -inDirection.getNormalized());

    if (cosinus <= 0) {
        return LightSample {
            .outLight = LinearColor::black(),
            .inDirection = inDirection,
            .pdf = 1
        };
    }

    const float solidAreaPdf = distanceSquared / (cosinus * area_);

    return LightSample {
        .outLight = emission_,
        .inDirection = inDirection,
        .pdf = solidAreaPdf
    };
}
}