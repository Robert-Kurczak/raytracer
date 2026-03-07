#pragma once

#include "Math/Point.hpp"

namespace RTC {
struct HitData {
    float rayT = 0.0F;
    Point3<float> hitPoint;
    Vector3<float> hitNormal;
};
}