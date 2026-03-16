#pragma once

#include <cstdlib>
#include <type_traits>

namespace RTC {
template<typename Type>
requires std::is_floating_point_v<Type>
constexpr bool isCloseToZero(
    Type value,
    Type epsilon = Type {1e-4}
) noexcept {
    return std::abs(value) < epsilon;
}
}