#include "TriangleAreaLight.hpp"

#include "Core/Color/Color.hpp"
#include "Core/Math/Random.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Light/LightSample.hpp"

namespace RTC {
Vector3<float> TriangleAreaLight::getPerpendicular(
    const Point3<float>& vertexA,
    const Point3<float>& vertexB,
    const Point3<float>& vertexC
) const {
    const Vector3<float> edge1 = vertexB - vertexA;
    const Vector3<float> edge2 = vertexC - vertexA;

    return getCrossProduct(edge1, edge2);
}

Vector3<float> TriangleAreaLight::getRandomSample() const {
    auto barycentricA = getRandomNumber<float>();
    auto barycentricB = getRandomNumber<float>();

    if (barycentricA + barycentricB > 1) {
        barycentricA = 1 - barycentricA;
        barycentricB = 1 - barycentricB;
    }

    const Point3<float> randomSample =
        barycentricA * (vertexA_ - vertexC_) +
        barycentricB * (vertexB_ - vertexC_) + vertexC_;

    return randomSample;
}

TriangleAreaLight::TriangleAreaLight(
    const Point3<float>& vertexA,
    const Point3<float>& vertexB,
    const Point3<float>& vertexC,
    const LinearColor& emission
) :
    vertexA_(vertexA),
    vertexB_(vertexB),
    vertexC_(vertexC),
    emission_(emission),
    perpendicular_(getPerpendicular(vertexA_, vertexB_, vertexC_)),
    normal_(perpendicular_.getNormalized()),
    pdf_(2.0F / perpendicular_.getLength()) {}

LightSample TriangleAreaLight::getSample(
    const Point3<float>& worldPosition,
    const Vector3<float>& worldNormal
) const {
    const Vector3<float> sample = getRandomSample();
    const Vector3<float> toLight = sample - worldPosition;
    const Vector3<float> toSurface = -toLight;
    const float distanceSquared = toSurface.getSquaredLength();

    const float lightCosinus =
        std::max(getDotProduct(normal_, toSurface.getNormalized()), 0.0F);

    const float surfaceCosinus = std::max(
        getDotProduct(worldNormal, toLight.getNormalized()), 0.0F
    );

    const LinearColor intensity =
        (emission_ * lightCosinus * surfaceCosinus) /
        (distanceSquared * pdf_);

    return LightSample {
        .outLight = intensity, .inDirection = toLight, .pdf = pdf_
    };
}

LightSample TriangleAreaLight::getSample(const Point3f& origin) const {
    const Vector3f sample = getRandomSample();
    const Vector3f inDirection = sample - origin;
    const float distanceSquared = inDirection.getSquaredLength();
    const float cosinus =
        getDotProduct(normal_, inDirection.getNormalized());

    const LinearColor outLight = emission_ * cosinus / distanceSquared;
    const float pdfSolidArea = distanceSquared / cosinus * pdf_;

    return LightSample {
        .outLight = outLight,
        .inDirection = inDirection,
        .pdf = pdfSolidArea
    };
}
}