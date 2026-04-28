#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
struct LightSample {
    LinearColor intensity;
    Vector3<float> toLight;
};
}