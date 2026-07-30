#pragma once

#include "Core/Math/Interval.hpp"
#include "Core/Math/Numeric.hpp"
#include "Core/Ray/Ray.hpp"

namespace RTC {
// TODO group related intersection algorithm is related `Shape` classes.
// Those should also implement getBoundingBox() method

struct MollerTrumboreResult {
    bool hasSolution = false;
    float t0 = 0.0F;
    float barycentricWeightA = 0.0F;
    float barycentricWeightB = 0.0F;
    float barycentricWeightC = 0.0F;
};

inline MollerTrumboreResult intersectTriangle(
    const Ray& ray,
    const Point3f& vertexA,
    const Point3f& vertexB,
    const Point3f& vertexC
) {
    const Vector3f reversedDirection = -ray.getDirection();
    const Vector3f edge1 = vertexA - vertexC;
    const Vector3f edge2 = vertexB - vertexC;

    const Vector3f crossedEdge2 =
        getCrossProduct(edge2, reversedDirection);
    const Vector3f solution = ray.getOrigin() - vertexC;

    const Interval<float> unitInterval {0.0F, 1.0F};
    const float mainDeterminant = getDotProduct(edge1, crossedEdge2);

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
        edge1, getCrossProduct(solution, reversedDirection)
    );

    const float weight2 = weight2Determinant / mainDeterminant;

    if (not unitInterval.contains(weight2)) {
        return MollerTrumboreResult {};
    }

    if (weight1 + weight2 > 1) {
        return MollerTrumboreResult {};
    }

    const float rayTDeterminant =
        getDotProduct(edge1, getCrossProduct(edge2, solution));

    const float rayT = rayTDeterminant / mainDeterminant;

    return MollerTrumboreResult {
        .hasSolution = true,
        .t0 = rayT,
        .barycentricWeightA = weight1,
        .barycentricWeightB = weight2,
        .barycentricWeightC = 1.0F - weight1 - weight2
    };
}
}