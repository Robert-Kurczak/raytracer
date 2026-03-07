#pragma once

#include "Math/Point.hpp"

namespace RTC {
struct HitData {
    float rayT = 0.0F;
    Point3<float> hitPoint {0, 0, 0};
    Vector3<float> hitNormal {0, 0, 0};
    bool isFrontFace = false;
};
}