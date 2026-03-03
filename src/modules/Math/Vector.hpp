#pragma once

namespace RTC {
template<typename Type>
class Vector2 {
public:
    Type x = 0;
    Type y = 0;
};

template<typename Type>
class Vector3 {
public:
    Type x = 0;
    Type y = 0;
    Type z = 0;
};

template<typename Type>
using Point2 = Vector2<Type>;

template<typename Type>
using Point3 = Vector3<Type>;
}