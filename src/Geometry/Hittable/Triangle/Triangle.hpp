#pragma once

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

    const Vector3<float> edge1_;
    const Vector3<float> edge2_;
    const Vector3<float> outwardNormal_;

    struct MollerTrumboreResult {
        bool hasSolution = false;
        float t0 = 0.0;
    };

    [[nodiscard]] MollerTrumboreResult solveMollerTrumbore(
        const Ray& ray
    ) const;

    [[nodiscard]] AxisAlignedBoundingBox createBoundingBox(
        const Point3<float>& vertexA,
        const Point3<float>& vertexB,
        const Point3<float>& vertexC
    ) const;

    void updateHitData(
        float rayT,
        const Ray& ray,
        HitData& hitData
    ) const;

public:
    Triangle(
        Vertex vertexA,
        Vertex vertexB,
        Vertex vertexC,
        std::shared_ptr<IMaterial> material
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