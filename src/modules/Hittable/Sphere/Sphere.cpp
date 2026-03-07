#include "Sphere.hpp"

#include "Math/Vector.hpp"

namespace RTC {

Sphere::Sphere(const Point3<float>& center, float radius) :
    center_(center),
    radius_(radius),
    radiusSquared_(radius_ * radius_) {}

bool Sphere::isHit(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    const Vector3<float> rayDirection = ray.getDirection();
    const Vector3<float> rayDisplacement = center_ - ray.getOrigin();

    const float aTerm = rayDirection.getSquaredLength();
    const float hTerm = getDotProduct(rayDirection, rayDisplacement);
    const float cTerm =
        rayDisplacement.getSquaredLength() - radiusSquared_;

    const float quadraticDelta = (hTerm * hTerm) - (aTerm * cTerm);

    if (quadraticDelta < 0) {
        return false;
    }

    const float minT = (hTerm - std::sqrt(quadraticDelta)) / aTerm;
    const Point3<float> minTPoint = ray.at(minT);
    const Vector3<float> minTNormal =
        (minTPoint - center_).getNormalized();

    hitData.rayT = minT;
    hitData.hitPoint = minTPoint;
    hitData.hitNormal = minTNormal;

    return true;
}

}