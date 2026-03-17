#include "Triangle.hpp"

#include "Core/Math/Interval.hpp"
#include "Core/Math/Numeric.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/Material/IMaterial.hpp"

#include <memory>

namespace RTC {
void Triangle::updateHitData(
    float rayT,
    const Ray& ray,
    HitData& hitData
) const {
    const Point3<float> tPoint = ray.at(rayT);
    const bool isFrontFace =
        getDotProduct(ray.getDirection(), outwardNormal_) < 0;

    const Vector3<float> normal =
        isFrontFace ? outwardNormal_ : -outwardNormal_;

    hitData.rayT = rayT;
    hitData.hitPoint = tPoint;
    hitData.hitNormal = normal;
    hitData.isFrontFace = isFrontFace;
    hitData.material = material_;
}

Triangle::Triangle(
    const Point3<float>& vertexA,
    const Point3<float>& vertexB,
    const Point3<float>& vertexC,
    std::shared_ptr<IMaterial> material
) :
    vertexA_(vertexA),
    vertexB_(vertexB),
    vertexC_(vertexC),
    material_(std::move(material)),
    edge1_(vertexA_ - vertexC_),
    edge2_(vertexB_ - vertexC_),
    outwardNormal_(getCrossProduct(edge1_, edge2_).getNormalized()) {}

bool Triangle::isHit(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    const Vector3<float> reversedDirection = ray.getDirection() * -1.0F;
    const Vector3<float> crossedEdge2 =
        getCrossProduct(edge2_, reversedDirection);
    const Vector3<float> solution = ray.getOrigin() - vertexC_;

    const Interval<float> unitInterval {0.0F, 1.0F};
    const float mainDeterminant = getDotProduct(edge1_, crossedEdge2);

    if (isCloseToZero(mainDeterminant)) {
        return false;
    }

    const float weight1Determinant =
        getDotProduct(solution, crossedEdge2);

    const float weight1 = weight1Determinant / mainDeterminant;

    if (not unitInterval.contains(weight1)) {
        return false;
    }

    const float weight2Determinant = getDotProduct(
        edge1_, getCrossProduct(solution, reversedDirection)
    );

    const float weight2 = weight2Determinant / mainDeterminant;

    if (not unitInterval.contains(weight2)) {
        return false;
    }

    if (weight1 + weight2 > 1) {
        return false;
    }

    const float rayTDeterminant =
        getDotProduct(edge1_, getCrossProduct(edge2_, solution));

    const float rayT = rayTDeterminant / mainDeterminant;

    if (interval.contains(rayT)) {
        updateHitData(rayT, ray, hitData);
        return true;
    }

    return false;
}
}