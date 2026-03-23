#include "Scene.hpp"

#include "Geometry/Light/PointLight/PointLight.hpp"

namespace RTC {
Scene::Scene() {
    std::unique_ptr<ILight> light = std::make_unique<PointLight>(
        Point3<float> {0.0, 1, -2.0},
        Color<float> {.red = 1.0F, .green = 0.3, .blue = 0.5},
        1
    );

    lights_.push_back(std::move(light));
}

void Scene::addObject(std::unique_ptr<IHittable> object) {
    objects_.push_back(std::move(object));
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