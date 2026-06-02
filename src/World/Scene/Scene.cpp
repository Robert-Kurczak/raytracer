#include "Scene.hpp"

#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Hittable/IHittable.hpp"

#include <memory>

namespace RTC {
Scene::Scene(
    std::unique_ptr<IHittable> sceneRoot,
    std::vector<std::unique_ptr<ILight>> lights
) :
    sceneRoot_(std::move(sceneRoot)),
    lights_(std::move(lights)) {}

const std::vector<std::unique_ptr<ILight>>& Scene::getLights() const {
    return lights_;
}

void Scene::setup() {
    const AxisAlignedBoundingBox boundingBox =
        sceneRoot_->getBoundingBox();

    for (const auto& light : lights_) {
        light->setup(boundingBox);
    }
}

bool Scene::hitClosest(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    return sceneRoot_->hitClosest(ray, interval, hitData);
}

bool Scene::hitAny(
    const Ray& ray,
    const Interval<float>& interval
) const {
    return sceneRoot_->hitAny(ray, interval);
}
}