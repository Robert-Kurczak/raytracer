#pragma once

#include "Core/Math/Interval.hpp"
#include "Core/Ray/Ray.hpp"
#include "Geometry/Background/IBackground.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Light/ILight.hpp"

#include <memory>
#include <vector>

namespace RTC {
class Scene {
private:
    std::unique_ptr<IHittable> sceneRoot_;
    std::vector<std::shared_ptr<ILight>> lights_;
    std::unique_ptr<IBackground> background_;

public:
    Scene(
        std::unique_ptr<IHittable> sceneRoot,
        std::vector<std::shared_ptr<ILight>> lights,
        std::unique_ptr<IBackground> background
    );

    void setup();

    [[nodiscard]] const std::vector<std::shared_ptr<ILight>>&
    getLights() const;

    [[nodiscard]] LinearColor sampleBackground(const Ray& ray) const;

    bool hitClosest(
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