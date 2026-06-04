#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/ILight.hpp"

namespace RTC {
class DirectionalLight : public ILight {
private:
    const LinearColor emission_;
    const Vector3f direction_;

    Point3f boundingSphereCenter_ {0.0F, 0.0F, 0.0F};
    float boundingSphereRadius_ = 0.0F;
    AxisAlignedBoundingBox boundingBox_ {boundingSphereCenter_};
    LinearColor power_;

public:
    DirectionalLight(
        const LinearColor& emission,
        const Vector3f& direction
    );

    void setup(const AxisAlignedBoundingBox& sceneBoundingBox) override;

    [[nodiscard]] AxisAlignedBoundingBox getBoundingBox() const override;

    [[nodiscard]] LightSample getSample(
        const Point3f& origin
    ) const override;

    [[nodiscard]] LinearColor getPower() const override;

    [[nodiscard]] Photon emitPhoton() const override;
};
}