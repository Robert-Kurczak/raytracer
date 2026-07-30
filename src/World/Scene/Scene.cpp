#include "Scene.hpp"

#include "Geometry/BoundingVolume/AxisAlignedBoundingBox/AxisAlignedBoundingBox.hpp"
#include "Geometry/Hittable/IHittable.hpp"

#include <memory>

namespace RTC {
Scene::Scene(
    std::unique_ptr<IHittable> sceneRoot,
    std::vector<std::shared_ptr<ILight>> lights,
    std::unique_ptr<IBackground> background_
) :
    sceneRoot_(std::move(sceneRoot)),
    lights_(std::move(lights)),
    background_(std::move(background_)) {}

const std::vector<std::shared_ptr<ILight>>& Scene::getLights() const {
    return lights_;
}

void Scene::setup() {
    const AxisAlignedBoundingBox boundingBox =
        sceneRoot_->getBoundingBox();

    for (const auto& light : lights_) {
        light->setup(boundingBox);
    }
}

LinearColor Scene::sampleBackground(const Ray& ray) const {
    return background_->sample(ray);
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