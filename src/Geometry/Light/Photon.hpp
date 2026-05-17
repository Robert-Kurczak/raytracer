#pragma once

#include "Core/Color/Color.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
struct Photon {
    Vector3f position;
    Vector3f direction;
    LinearColor power;
};
}