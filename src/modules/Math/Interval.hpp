#pragma once

#include <type_traits>

namespace RTC {
template<typename Type>
requires std::is_arithmetic_v<Type>
struct Interval {
    Type start;
    Type stop;
};
}