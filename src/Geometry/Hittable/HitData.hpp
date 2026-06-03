#pragma once

#include "Core/Math/Point.hpp"

#include <memory>

namespace RTC {
class IMaterial;

struct HitData {
    float rayT = 0.0F;
    Point3f hitPoint {0, 0, 0};
    Vector3f hitNormal {0, 0, 0};
    std::shared_ptr<IMaterial> material;
};
}