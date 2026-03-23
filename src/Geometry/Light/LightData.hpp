#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
struct LightData {
    Vector3<float> toLight;
    Color<float> illumination;
};
}