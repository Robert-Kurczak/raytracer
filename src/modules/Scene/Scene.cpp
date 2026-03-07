#include "Scene.hpp"

namespace RTC {
void Scene::addObject(std::unique_ptr<IHittable> object) {
    objects_.push_back(std::move(object));
}

bool Scene::hitRay(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    for (const auto& object : objects_) {
        const bool wasObjectHit = object->isHit(ray, interval, hitData);

        if (wasObjectHit) {
            return true;
        }
    }

    return false;
}
}