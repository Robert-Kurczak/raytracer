#include "Sphere.hpp"

#include "Core/Math/Vector.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"

namespace RTC {

Sphere::QuadraticResult Sphere::solveQuadratic(const Ray& ray) const {
    const Vector3f rayDirection = ray.getDirection();
    const Vector3f rayDisplacement = center_ - ray.getOrigin();

    const float aTerm = rayDirection.getSquaredLength();
    const float hTerm = getDotProduct(rayDirection, rayDisplacement);
    const float cTerm =
        rayDisplacement.getSquaredLength() - radiusSquared_;

    const float quadraticDelta = (hTerm * hTerm) - (aTerm * cTerm);

    if (quadraticDelta < 0) {
        return QuadraticResult {};
    }

    const float quadraticDeltaSqrt = std::sqrt(quadraticDelta);
    const float t1 = (hTerm - quadraticDeltaSqrt) / aTerm;
    const float t2 = (hTerm + quadraticDeltaSqrt) / aTerm;

    return QuadraticResult {.hasSolution = true, .t1 = t1, .t2 = t2};
}

AxisAlignedBoundingBox Sphere::createBoundingBox(
    const Point3<float>& center,
    float radius
) const {
    const Vector3f radiusOffset {radius, radius, radius};

    return AxisAlignedBoundingBox {
        center - radiusOffset, center + radiusOffset
    };
}

void Sphere::updateHitData(
    float rayT,
    const Ray& ray,
    HitData& hitData
) const {
    const Point3f tPoint = ray.at(rayT);
    const Vector3f normal = (tPoint - center_).getNormalized();

    hitData.rayT = rayT;
    hitData.hitPoint = tPoint;
    hitData.hitNormal = normal;
    hitData.material = material_;
}

Sphere::Sphere(
    const Point3f& center,
    float radius,
    std::shared_ptr<IMaterial> material
) :
    center_(center),
    radius_(radius),
    material_(std::move(material)),
    radiusSquared_(radius_ * radius_),
    boundingBox_(createBoundingBox(center_, radius_)) {}

const AxisAlignedBoundingBox& Sphere::getBoundingBox() const {
    return boundingBox_;
}

bool Sphere::hitClosest(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    const QuadraticResult result = solveQuadratic(ray);

    if (not result.hasSolution) {
        return false;
    }

    if (interval.contains(result.t1)) {
        updateHitData(result.t1, ray, hitData);
        return true;
    }

    if (interval.contains(result.t2)) {
        updateHitData(result.t2, ray, hitData);
        return true;
    }

    return false;
}

bool Sphere::hitAny(
    const Ray& ray,
    const Interval<float>& interval
) const {
    const QuadraticResult result = solveQuadratic(ray);

    if (not result.hasSolution) {
        return false;
    }

    const bool solutionInInterval =
        interval.contains(result.t1) || interval.contains(result.t2);

    return solutionInInterval;
}

}