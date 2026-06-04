#pragma once

#include "Core/Color/Color.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Light/LightSample.hpp"

namespace RTC {
class PointLight : public ILight {
private:
    Point3f position_;
    LinearColor emission_;
    float decay_;
    AxisAlignedBoundingBox boundingBox_;

public:
    PointLight(
        const Point3f& position,
        const LinearColor& emission,
        float decay
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