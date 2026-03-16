#include "Mesh.hpp"

namespace RTC {
void Mesh::addTriangle(std::unique_ptr<Triangle> triangle) {
    triangles_.push_back(std::move(triangle));
}

bool Mesh::isHit(
    const Ray& ray,
    const Interval<float>& interval,
    HitData& hitData
) const {
    Interval<float> tempInterval = interval;
    HitData tempHitData {};
    bool hitAnything = false;

    for (const auto& triangle : triangles_) {
        const bool wasObjectHit =
            triangle->isHit(ray, tempInterval, tempHitData);

        if (wasObjectHit) {
            hitAnything = true;
            tempInterval.end = tempHitData.rayT;
            hitData = tempHitData;
        }
    }

    return hitAnything;
}
}