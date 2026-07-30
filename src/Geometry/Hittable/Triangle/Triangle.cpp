#include "Triangle.hpp"

#include "Core/Math/Interval.hpp"
#include "Core/Math/Vector.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/ILight.hpp"
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

Point2f Triangle::createTextureCoords(
    const MollerTrumboreResult& result
) const {
    if (!vertexA_.texturePosition || !vertexB_.texturePosition ||
        !vertexC_.texturePosition) {
        return Point2f {0.0F, 0.0F};
    }

    return result.barycentricWeightA * vertexA_.texturePosition.value() +
           result.barycentricWeightB * vertexB_.texturePosition.value() +
           result.barycentricWeightC * vertexC_.texturePosition.value();
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

    const Point2f textureCoords = createTextureCoords(result);

    hitData.rayT = result.t0;
    hitData.hitPoint = tPoint;
    hitData.hitNormal = normal;
    hitData.textureCoords = textureCoords;
    hitData.material = material_;
    hitData.light = light_;
}

Triangle::Triangle(
    Vertex vertexA,
    Vertex vertexB,
    Vertex vertexC,
    std::shared_ptr<IMaterial> material,
    std::shared_ptr<ILight> light
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
    light_(std::move(light)),
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
    const MollerTrumboreResult result = intersectTriangle(
        ray, vertexA_.position, vertexB_.position, vertexC_.position
    );

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
    const MollerTrumboreResult result = intersectTriangle(
        ray, vertexA_.position, vertexB_.position, vertexC_.position
    );

    return interval.contains(result.t0);
}
}