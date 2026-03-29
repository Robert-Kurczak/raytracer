#pragma once

#include "Core/Math/Interval.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Light/ILight.hpp"

#include <memory>
#include <vector>

namespace RTC {
class Scene {
private:
    std::vector<std::unique_ptr<IHittable>> objects_;
    std::vector<std::unique_ptr<ILight>> lights_;

public:
    void addObject(std::unique_ptr<IHittable> object);
    void addLight(std::unique_ptr<ILight> light);

    [[nodiscard]] const std::vector<std::unique_ptr<IHittable>>&
    getObjects() const;

    [[nodiscard]] const std::vector<std::unique_ptr<ILight>>&
    getLights() const;

    bool hitRay(
        const Ray& ray,
        const Interval<float>& interval,
        HitData& hitData
    ) const;

    [[nodiscard]] bool hitAny(
        const Ray& ray,
        const Interval<float>& interval
    ) const;
};
}