#include "Scene.hpp"

namespace RTC {
void Scene::addObject(std::unique_ptr<IHittable> object) {
    objects_.push_back(std::move(object));
}

void Scene::addLight(std::unique_ptr<ILight> light) {
    lights_.push_back(std::move(light));
}

const std::vector<std::unique_ptr<IHittable>>& Scene::getObjects() const {
    return objects_;
}

const std::vector<std::unique_ptr<ILight>>& Scene::getLights() const {
    return lights_;
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

bool Scene::hitAny(
    const Ray& ray,
    const Interval<float>& interval
) const {
    HitData hitData {};

    for (const auto& object : objects_) {
        if (object->isHit(ray, interval, hitData)) {
            return true;
        }
    }

    return false;
}
}