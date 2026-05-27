#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Point.hpp"
#include "Geometry/Light/LightSample.hpp"
#include "Photon.hpp"

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

    [[nodiscard]] virtual LightSample getSample(
        const Point3f& origin
    ) const = 0;

    [[nodiscard]] virtual LinearColor getPower() const = 0;

    [[nodiscard]] virtual Photon emitPhoton() const = 0;
};
}