#pragma once

#include <cstdlib>
#include <numbers>
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

template<typename Type>
requires std::is_floating_point_v<Type>
constexpr double degreesToRadians(Type value) noexcept {
    return value * (std::numbers::pi / 180.0);
}

inline constexpr float EPSILON = 0.00001F;
}