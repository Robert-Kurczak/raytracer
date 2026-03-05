#pragma once

#include "Math/Point.hpp"

#include <cstdint>

namespace RTC {
struct CameraParameters {
    uint32_t screenWidth = 0;
    double aspectRatio = 0;

    Point3<float> center;
    Vector3<float> direction;
};
}