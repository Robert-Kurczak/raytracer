#pragma once

#include "Core/Math/Point.hpp"

#include <optional>

namespace RTC {
struct Vertex {
    Point3f position;
    std::optional<Point2f> texturePosition;
    std::optional<Vector3f> normal;
};
}