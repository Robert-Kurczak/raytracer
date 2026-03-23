#pragma once

#include "Core/Math/Point.hpp"
#include "Geometry/Light/LightData.hpp"

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

    [[nodiscard]] virtual LightData getSample(
        const Point3<float>& worldPosition
    ) const = 0;
};
}