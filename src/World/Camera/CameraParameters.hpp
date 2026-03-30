#pragma once

#include "Core/Math/Point.hpp"

#include <cstdint>

namespace RTC {
struct CameraParameters {
    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;

    Point3<float> position;
    Vector3<float> direction;
    Vector3<float> upDirection;
    float fieldOfViewDegrees = 0;
};
}