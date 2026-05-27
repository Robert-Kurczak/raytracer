#include "Triangle.hpp"

#include "Core/Math/Interval.hpp"
#include "Core/Math/Numeric.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Rendering/Material/IMaterial.hpp"

#include <memory>

namespace RTC {
std::optional<Vector3f> Triangle::createFlatNormalIfNeccessary() const {
    if (vertexA_.normal && vertexB_.normal && vertexC_.normal) {
        return std::nullopt;
    }

    return getCrossProduct(edge1_, edge2_).getNormalized();
}

Vector3f Triangle::createSmoothNormal(
    const MollerTrumboreResult& result
) const {
    const Vector3f interpolatedNormal =
        result.barycentricWeightA * vertexA_.normal.value() +
        result.barycentricWeightB * vertexB_.normal.value() +
        result.barycentricWeightC * vertexC_.normal.value();

    return interpolatedNormal.getNormalized();
}

Triangle::MollerTrumboreResult Triangle::solveMollerTrumbore(
    const Ray& ray
) const {
    const Vector3f reversedDirection = -ray.getDirection();
    const Vector3f crossedEdge2 =
        getCrossProduct(edge2_, reversedDirection);
    const Vector3f solution = ray.getOrigin() - vertexC_.position;

    const Interval<float> unitInterval {0.0F, 1.0F};
    const float mainDeterminant = getDotProduct(edge1_, crossedEdge2);

    if (isCloseToZero(mainDeterminant)) {
        return MollerTrumboreResult {};
    }

    const float weight1Determinant =
        getDotProduct(solution, crossedEdge2);

    const float weight1 = weight1Determinant / mainDeterminant;

    if (not unitInterval.contains(weight1)) {
        return MollerTrumboreResult {};
    }

    const float weight2Determinant = getDotProduct(
        edge1_, getCrossProduct(solution, reversedDirection)
    );

    const float weight2 = weight2Determinant / mainDeterminant;

    if (not unitInterval.contains(weight2)) {
        return MollerTrumboreResult {};
    }

    if (weight1 + weight2 > 1) {
        return MollerTrumboreResult {};
    }

    const float rayTDeterminant =
        getDotProduct(edge1_, getCrossProduct(edge2_, solution));

    const float rayT = rayTDeterminant / mainDeterminant;

    return MollerTrumboreResult {
        .hasSolution = true,
        .t0 = rayT,
        .barycentricWeightA = weight1,
        .barycentricWeightB = weight2,
        .barycentricWeightC = 1.0F - weight1 - weight2
    };
}

[[nodiscard]] AxisAlignedBoundingBox Triangle::createBoundingBox(
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

void Triangle::updateHitData(
    const MollerTrumboreResult& result,
    const Ray& ray,
    HitData& hitData
) const {
    const Point3f tPoint = ray.at(result.t0);

    const Vector3f normal =
        flatNormal_ ? flatNormal_.value() : createSmoothNormal(result);

    hitData.rayT = result.t0;
    hitData.hitPoint = tPoint;
    hitData.hitNormal = normal;
    hitData.material = material_;
}

Triangle::Triangle(
    Vertex vertexA,
    Vertex vertexB,
    Vertex vertexC,
    std::shared_ptr<IMaterial> material
) :
    vertexA_(vertexA),
    vertexB_(vertexB),
    vertexC_(vertexC),
    boundingBox_(createBoundingBox(
        vertexA_.position,
        vertexB_.position,
        vertexC_.position
    )),
    material_(std::move(material)),
    edge1_(vertexA_.position - vertexC_.position),
    edge2_(vertexB_.position - vertexC_.position),
    flatNormal_(createFlatNormalIfNeccessary()) {}

[[nodiscard]] const AxisAlignedBoundingBox& Triangle::
    getBoundingBox() const {
    return boundingBox_;
}

bool Triangle::hitClosest(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    const MollerTrumboreResult result = solveMollerTrumbore(ray);

    if (not result.hasSolution) {
        return false;
    }

    if (interval.contains(result.t0)) {
        updateHitData(result, ray, hitData);
        return true;
    }

    return false;
}

bool Triangle::hitAny(
    const Ray& ray,
    const Interval<float>& interval
) const {
    const MollerTrumboreResult result = solveMollerTrumbore(ray);

    return interval.contains(result.t0);
}
}