#pragma once

#include "Core/Math/Intersections.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Vertex.hpp"
#include "Rendering/Material/IMaterial.hpp"

#include <memory>

namespace RTC {
class Triangle : public IHittable {
private:
    const Vertex vertexA_;
    const Vertex vertexB_;
    const Vertex vertexC_;

    const AxisAlignedBoundingBox boundingBox_;
    std::shared_ptr<IMaterial> material_;
    std::shared_ptr<ILight> light_ = nullptr;

    const Vector3f edge1_;
    const Vector3f edge2_;
    const std::optional<Vector3f> flatNormal_;

    [[nodiscard]] std::optional<Vector3f>
    createFlatNormalIfNeccessary() const;

    [[nodiscard]] Vector3f createSmoothNormal(
        const MollerTrumboreResult& result
    ) const;

    [[nodiscard]] Point2f createTextureCoords(
        const MollerTrumboreResult& result
    ) const;

    [[nodiscard]] AxisAlignedBoundingBox createBoundingBox(
        const Point3f& vertexA,
        const Point3f& vertexB,
        const Point3f& vertexC
    ) const;

    [[nodiscard]] Vector3f getShadingNormal() const;

    void updateHitData(
        const MollerTrumboreResult& result,
        const Ray& ray,
        HitData& hitData
    ) const;

public:
    Triangle(
        Vertex vertexA,
        Vertex vertexB,
        Vertex vertexC,
        std::shared_ptr<IMaterial> material,
        std::shared_ptr<ILight> light = nullptr
    );

    [[nodiscard]] const AxisAlignedBoundingBox&
    getBoundingBox() const override;

    bool hitClosest(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const override;

    [[nodiscard]] bool hitAny(
        const Ray& ray,
        const Interval<float>& interval
    ) const override;
};
}