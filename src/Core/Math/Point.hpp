#pragma once

#include "Vector.hpp"

namespace RTC {
template<typename Type>
using Point3 = Vector3<Type>;

using Point3f = Vector3f;
using Point3i = Vector3i;
using Point3u = Vector3u;

template<typename Type>
using Point2 = Vector2<Type>;

using Point2f = Vector2f;
using Point2i = Vector2i;
using Point2u = Vector2u;
}