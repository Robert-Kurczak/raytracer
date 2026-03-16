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
    Interval<float> tempInterval = interval;
    HitData tempHitData {};
    bool hitAnything = false;

    for (const auto& object : objects_) {
        const bool wasObjectHit =
            object->isHit(ray, tempInterval, tempHitData);

        if (wasObjectHit) {
            hitAnything = true;
            tempInterval.end = tempHitData.rayT;
            hitData = tempHitData;
        }
    }

    return hitAnything;
}
}