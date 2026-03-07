#include "Sphere.hpp"

#include "Math/Vector.hpp"

namespace RTC {
void Sphere::updateHitData(
    float rayT,
    const Ray& ray,
    HitData& hitData
) const {
    const Point3<float> tPoint = ray.at(rayT);
    const Vector3<float> outwardNormal =
        (tPoint - center_).getNormalized();
    const bool isFrontFace =
        getDotProduct(ray.getDirection(), outwardNormal) < 0;

    const Vector3<float> normal =
        isFrontFace ? outwardNormal : -outwardNormal;

    hitData.rayT = rayT;
    hitData.hitPoint = tPoint;
    hitData.hitNormal = normal;
    hitData.isFrontFace = isFrontFace;
}

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

    const float quadraticDeltaSqrt = std::sqrt(quadraticDelta);

    const float minT = (hTerm - quadraticDeltaSqrt) / aTerm;

    if (interval.contains(minT)) {
        updateHitData(minT, ray, hitData);
        return true;
    }

    const float maxT = (hTerm + quadraticDeltaSqrt) / aTerm;

    if (interval.contains(maxT)) {
        updateHitData(maxT, ray, hitData);
        return true;
    }

    return false;
}

}