#include "Ray.hpp"

#include "Math/Vector.hpp"

#include <cassert>

namespace RTC {
Ray::Ray(const Point3<float>& origin, const Vector3<float>& direction) :
    origin_(origin),
    direction_(direction) {}

[[nodiscard]] const Point3<float>& Ray::getOrigin() const {
    return origin_;
}

[[nodiscard]] const Vector3<float>& Ray::getDirection() const {
    return direction_;
}

[[nodiscard]] Point3<float> Ray::at(float t) const {
    return origin_ + (direction_ * t);
}
}