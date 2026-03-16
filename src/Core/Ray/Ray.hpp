#pragma once

#include "Core/Math/Point.hpp"
#include "Core/Math/Vector.hpp"

namespace RTC {
class Ray {
private:
    Point3<float> origin_;
    Vector3<float> direction_;

public:
    Ray(const Point3<float>& origin, const Vector3<float>& direction);

    [[nodiscard]] const Point3<float>& getOrigin() const;
    [[nodiscard]] const Vector3<float>& getDirection() const;

    [[nodiscard]] Point3<float> at(float t) const;
};
}