#pragma once

#include "Core/Math/Point.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
class Ray {
private:
    Point3f origin_ {0, 0, 0};
    Vector3f direction_ {0, 0, 0};

public:
    Ray() = default;
    Ray(const Point3<float>& origin, const Vector3<float>& direction);

    [[nodiscard]] const Point3f& getOrigin() const;
    [[nodiscard]] const Vector3f& getDirection() const;

    [[nodiscard]] Point3f at(float t) const;
};
}