#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Point.hpp"
#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Photon.hpp"

#include <memory>
#include <vector>

namespace RTC {
class Scene;

class ILight {
public:
    ILight() = default;
    ILight(const ILight&) = delete;
    ILight(ILight&&) = delete;
    ILight operator=(const ILight&) = delete;
    ILight operator=(ILight&&) = delete;
    virtual ~ILight() = default;

    virtual void setup(
        const AxisAlignedBoundingBox& sceneBoundingBox
    ) = 0;

    virtual void discretize(
        std::vector<std::shared_ptr<ILight>>& discreteLights,
        uint32_t samples
    ) const = 0;

    [[nodiscard]] virtual bool isInfinite() const = 0;

    [[nodiscard]] virtual AxisAlignedBoundingBox
    getBoundingBox() const = 0;

    [[nodiscard]] virtual LightSample getSample(
        const Point3f& origin
    ) const = 0;

    [[nodiscard]] virtual LinearColor getPower() const = 0;

    [[nodiscard]] virtual Photon emitPhoton() const = 0;

    [[nodiscard]] virtual float calculatePdf(
        const Point3f& origin,
        const Vector3f& inDirection
    ) const = 0;
};
}